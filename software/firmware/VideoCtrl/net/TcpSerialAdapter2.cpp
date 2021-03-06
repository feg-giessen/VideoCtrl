/*
 * TcpSerialAdapter2.cpp
 *
 *  Created on: 15.03.2014
 *      Author: Peter Schuster
 */

#include <string.h>
#include "TcpSerialAdapter2.h"

TcpSerialAdapter2::TcpSerialAdapter2() {
    _timeout = 0;
    _timeout_count = 0;
    _pcb = NULL;

    _sending_slot = NULL;
    _ack_slot = NULL;
    _recv_slot = NULL;

    _connected = false;
    _connecting = false;
    _timed_out = false;

    _last_error = ERR_OK;
}

void TcpSerialAdapter2::begin(ip_addr_t addr, uint16_t port, uint8_t timeout) {
    _addr = addr;
    _port = port;
    _timeout = timeout;
    _pcb = NULL;

    _send_queue.reset();
    _sending_slot = NULL;
    _ack_slot = NULL;
    _recv_slot = NULL;
}

bool TcpSerialAdapter2::isConnected() {
    return _connected;
}

bool TcpSerialAdapter2::isTimedout() {
    return _timed_out;
}

err_t TcpSerialAdapter2::getLastError() {
    return _last_error;
}

void TcpSerialAdapter2::_createConnection() {
    _timeout_count = 0;

    _pcb = tcp_new();
    tcp_arg(_pcb, this);
    tcp_err(_pcb, &_tcp_err);
    tcp_recv(_pcb, &_tcp_recv);
    tcp_sent(_pcb, &_tcp_sent);
    tcp_poll(_pcb, &_tcp_poll, 1);
}

void TcpSerialAdapter2::_reset() {
    tcp_msg_t* packet = NULL;
    msg_t s;
    err_t error_code;

    if (_connecting) {
        error_code = ERR_TIMEOUT;
        _timed_out = true;
    } else {
        error_code = ERR_CONN;
    }

    // clear queue waiting for responses.
    if (_recv_slot != NULL) {
        if (_recv_slot->cb != NULL) {
            _recv_slot->cb(error_code, _recv_slot->context, (char*)_recv_slot->recv_buf, _recv_slot->recv_ptr, _recv_slot->arg);
        }

        delete _recv_slot->data;
        delete _recv_slot;
        _recv_slot = NULL;
    }

    // clear queue waiting for acks.
    if (_ack_slot != NULL && _ack_slot->ptr >= _ack_slot->length) {
        // only delete if not still in send-queue
        if (_ack_slot->cb != NULL) {
            _ack_slot->cb(error_code, _ack_slot->context, NULL, 0, _ack_slot->arg);
        }

        delete _ack_slot->data;
        delete _ack_slot;
        _ack_slot = NULL;
    }

    // clear queue waiting for sending.
    do {
        s = _send_queue.fetchI((msg_t*)&packet);

        if (packet != NULL && s == ERR_OK) {
            if (packet->cb != NULL) {
                packet->cb(error_code, packet->context, NULL, 0, packet->arg);
            }
            delete packet->data;
            delete packet;
        }
    } while (s == ERR_OK);

    _pcb = NULL;
    _timeout_count = 0;
    _connected = false;
    _connecting = false;
}

tcp_msg_t* TcpSerialAdapter2::_createMsg(const char* data, size_t length, tcp_send_cb cb, void* context, void* arg, int8_t expected_max_length) {
    tcp_msg_t* msg = new tcp_msg_t();

    if (length > 0) {
        msg->data = (char*)chHeapAlloc(NULL, length);
        memcpy((void*)msg->data, data, length);
    } else {
        msg->data = NULL;
    }

    msg->length = length;
    msg->ptr = 0;
    msg->expected_max_length = expected_max_length;
    msg->acked = 0;

    msg->recv_ptr = 0;
    msg->recv_time = 0;

    msg->cb = cb;
    msg->arg = arg;
    msg->context = context;

    return msg;
}

err_t TcpSerialAdapter2::send(const char* data, size_t length, tcp_send_cb cb, void* context, void* arg, int8_t expected_max_length) {

    if (_pcb == NULL) {
        _createConnection();
    }

    if (_pcb == NULL)
        return ERR_MEM;

    if (length > 0) {
        // length == 0 -> no data -> nothing to transmit, nothing to receive.
        tcp_msg_t* msg = _createMsg(data, length, cb, context, arg, expected_max_length);
        _send_queue.post((msg_t)msg, TIME_INFINITE);
    } else if (cb != NULL) {
        cb(ERR_ARG, context, NULL, 0, arg);
    }

    if (!_connected && !_connecting) {

        if (_pcb->state == CLOSED) {
            _connecting = true;
            tcp_connect(_pcb, &_addr, _port, &_tcp_connected);
        } else {
            _reset();
            return ERR_RST;
        }
    }

    // tcp_* functions in lwip MUST only be called from inside lwip thread!
    //if (_connected) {
    //    _processSendQueue();
    //}

    _processRecvQueue();

    return ERR_OK;
}

err_t TcpSerialAdapter2::_processSendQueue() {
    msg_t s;
    tcp_msg_t* packet = NULL;

    if (_sending_slot != NULL) {
        packet = _sending_slot;

    } else if (_ack_slot == NULL && _recv_slot == NULL) {

        // Fetch with timeout (immediate)
        s = _send_queue.fetchI((msg_t*)&packet);

        if (s != RDY_OK)
            return s;
    }

    if (packet == NULL)
        return ERR_ABRT;

    err_t err;

    u8_t apiflags = 0;
    u16_t avail = tcp_sndbuf(_pcb);
    u16_t length = packet->length - packet->ptr;

    if (avail < length) {
        length = avail;
        apiflags |= TCP_WRITE_FLAG_MORE;    // don't set PSH flag
    }

    uint8_t send_count = 4;
    do {
        err = tcp_write(_pcb, (packet->data + packet->ptr), length, apiflags);

        if (err == ERR_OK || err == ERR_MEM) {
            // put data on wire
            tcp_output(_pcb);
            _timeout_count = 0;        // reset connection timeout
        }

        send_count--;
    }
    while (err == ERR_MEM && send_count > 0);

    if (err != ERR_OK)
        return err;

    u16_t ptr_previous = packet->ptr; // save previous packet pointer

    // update pointer only on successful transmission.
    packet->ptr += length;

    if (packet->ptr >= packet->length) {
        // packet fully transmitted
        _sending_slot = NULL;
    }

    if (ptr_previous == 0) {
        // packet transmission started
        _ack_slot = packet;
    }

    packet->recv_time = chTimeNow();

    return err;
}

void TcpSerialAdapter2::_processRecvQueue() {

    tcp_msg_t* packet = _recv_slot;

    if (packet == NULL)
        return;

    u32_t tmo = TCP_SERIAL_RCV_TMO;
    u32_t t_now = chTimeNow();
    u32_t diff = t_now - packet->recv_time;

    // if receive timeout OR already got expected answer --> return to application, free internal queue
    if (diff >= tmo
            || (packet->expected_max_length >= 0 && packet->recv_ptr >= packet->expected_max_length)) {

        packet->cb(ERR_OK, packet->context, (char*)packet->recv_buf, packet->recv_ptr, packet->arg);

        // callback _must_ copy data to local domain if it wants to use it!
        delete packet->data;
        delete packet;
        _recv_slot = NULL;
    }
}

// Disable parameter warnings for callbacks
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/** Function prototype for tcp receive callback functions. Called when data has
 * been received.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb The connection pcb which received data
 * @param p The received data (or NULL when the connection has been closed!)
 * @param err An error code if there has been an error receiving
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
err_t TcpSerialAdapter2::_tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TcpSerialAdapter2* that = (TcpSerialAdapter2*)arg;

    that->_timeout_count = 0;        // reset connection timeout
    that->_timed_out = false;

    if (p == NULL) {
        // indicates closed connection.
        that->_connected = false;
        that->_reset();

        return ERR_OK;
    }

    tcp_msg_t* packet = that->_recv_slot;

    if (packet == NULL) {
        // No packet in waiting queue

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        p = NULL;

        return ERR_OK;  // return OK to LWIP, but there is nothing to do for us...
    }

    if (err != ERR_OK) {
        if(packet->cb != NULL) {
            packet->cb(ERR_CONN, packet->context, NULL, 0, packet->arg);
        }
    } else {

        packet->recv_time = chTimeNow();

        if(packet->cb != NULL && p->tot_len > 0) {
            size_t len;
            char* data;

            data = (char*)chHeapAlloc(NULL, p->tot_len);            // allocate mem for payload.
            len = pbuf_copy_partial(p, (void*)data, p->tot_len, 0); // copy data to app domain

            if ((packet->recv_ptr + len) > TCP_SERIAL_RCV_BUF) {
                packet->recv_time -= TCP_SERIAL_RCV_TMO;  // --> simulate timeout condition -> release packet from buffer.
            } else {
                memcpy(packet->recv_buf + packet->recv_ptr, data, len);
                packet->recv_ptr += len;
            }

            delete data;
        }
    }

    that->_processRecvQueue();

    if (p != NULL) {
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        p = NULL;
    }

    return ERR_OK;
}

/** Function prototype for tcp sent callback functions. Called when sent data has
 * been acknowledged by the remote side. Use it to free corresponding resources.
 * This also means that the pcb has now space available to send new data.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb The connection pcb for which data has been acknowledged
 * @param len The amount of bytes acknowledged
 * @return ERR_OK: try to send some data by calling tcp_output
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
err_t TcpSerialAdapter2::_tcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    TcpSerialAdapter2* that = (TcpSerialAdapter2*)arg;

    that->_timed_out = false;

    // packet send -> free space in send-buf -> try to send waiting packets.
    that->_processSendQueue();

    tcp_msg_t* packet = that->_ack_slot;

    if (packet == NULL)
        return ERR_OK;  // return OK to LWIP, but there is nothing to do for us...

    packet->acked += len;

    if (packet->acked >= packet->length) {
        // transmission done
        that->_recv_slot = packet;
        that->_ack_slot = NULL;
    }

    return ERR_OK;
}

/** Function prototype for tcp poll callback functions. Called periodically as
 * specified by @see tcp_poll.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb tcp pcb
 * @return ERR_OK: try to send some data by calling tcp_output
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
err_t TcpSerialAdapter2::_tcp_poll(void *arg, struct tcp_pcb *tpcb) {
    TcpSerialAdapter2* that = (TcpSerialAdapter2*)arg;

    that->_processRecvQueue();

    if (that->_connected) {

        that->_processSendQueue();

        // connection keep-alive timeout configured?
        if (that->_timeout > 0) {
            that->_timeout_count++;

            // timeout count reached? -> close
            if (that->_timeout_count >= that->_timeout) {
                err_t msg = tcp_close(tpcb);
                if (msg == ERR_OK) {
                    that->_reset();
                }

                return msg;
            }
        }
    }

    return ERR_OK;
}

/** Function prototype for tcp error callback functions. Called when the pcb
 * receives a RST or is unexpectedly closed for any other reason.
 *
 * @note The corresponding pcb is already freed when this callback is called!
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param err Error code to indicate why the pcb has been closed
 *            ERR_ABRT: aborted through tcp_abort or by a TCP timer
 *            ERR_RST: the connection was reset by the remote host
 */
void TcpSerialAdapter2::_tcp_err(void *arg, err_t err) {
    TcpSerialAdapter2* that = (TcpSerialAdapter2*)arg;

    if (that != NULL) {
        that->_last_error = err;
        that->_reset();
    }
}

/** Function prototype for tcp connected callback functions. Called when a pcb
 * is connected to the remote side after initiating a connection attempt by
 * calling tcp_connect().
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb The connection pcb which is connected
 * @param err An unused error code, always ERR_OK currently ;-)
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 *
 * @note When a connection attempt fails, the error callback is currently called!
 */
err_t TcpSerialAdapter2::_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {

    // err is always ERR_OK (04/2014 PS)

    TcpSerialAdapter2* that = (TcpSerialAdapter2*)arg;
    that->_connected = true;
    that->_connecting = false;
    that->_timed_out = false;

    that->_timeout_count = 0;   // reset connection keep-alive timeout counter.

    return ERR_OK;
}

#pragma GCC diagnostic pop
