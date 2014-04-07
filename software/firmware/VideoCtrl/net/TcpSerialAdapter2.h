/*
 * TcpSerialAdapter2.h
 *
 *  Created on: 15.03.2014
 *      Author: Peter Schuster
 */

#ifndef TCPSERIALADAPTER2_H_
#define TCPSERIALADAPTER2_H_

#include <stdint.h>
#include "ch.hpp"
#include "lwip/ip4_addr.h"
#include "lwip/err.h"
#include "lwip/tcp.h"

// Internal receive buffer per packet.
#define TCP_SERIAL_RCV_BUF  255

// Receive timeout in ms
#define TCP_SERIAL_RCV_TMO  100

typedef void (*tcp_send_cb)(err_t err, void* context, char* result, size_t length, void* arg);

typedef struct {
	char* data;     // pointer to data of packet.
	size_t length;  // data length
	u16_t ptr;      // pointer to current position in packet data.
	u16_t acked;    // packet is acknowledged.
	u8_t recv_buf[TCP_SERIAL_RCV_BUF];
	u8_t recv_ptr;
	u32_t recv_time;
	tcp_send_cb cb; // callback after packet was sent.
	void* context;  // context
	void* arg;      // argument for callback.
} tcp_msg_t;

class TcpSerialAdapter2 {
private:
	ip_addr_t _addr;
	uint16_t _port;
	uint8_t _timeout;
	uint8_t _timeout_count;
	bool _connected;
	tcp_pcb* _pcb;
	chibios_rt::MailboxBuffer<8> _send_queue;

	tcp_msg_t* _sending;
	tcp_msg_t* _ack_queue;
	tcp_msg_t* _recv_queue;

public:
	TcpSerialAdapter2();
	void begin(ip_addr_t _addr, uint16_t _port, uint8_t timeout);
	err_t send(const char* data, size_t* length, tcp_send_cb cb, void* context, void* arg);

private:
	void _createConnection();
	/**
	 * Resets local connection variables and states.
	 */
	void _reset();
	tcp_msg_t* _createMsg(const char* data, size_t* length, tcp_send_cb cb, void* context, void* arg);
	err_t _processSendQueue();
	void _processRecvQueue();

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
	err_t static _tcp_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

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
	err_t static _tcp_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);

	/** Function prototype for tcp poll callback functions. Called periodically as
	 * specified by @see tcp_poll.
	 *
	 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
	 * @param tpcb tcp pcb
	 * @return ERR_OK: try to send some data by calling tcp_output
	 *            Only return ERR_ABRT if you have called tcp_abort from within the
	 *            callback function!
	 */
	err_t static _tcp_poll(void *arg, struct tcp_pcb *tpcb);

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
	void static _tcp_err(void *arg, err_t err);

	/** Function prototype for tcp connected callback functions. Called when a pcb
	 * is connected to the remote side after initiating a connection attempt by
	 * calling tcp_connect().
	 *
	 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
	 * @param tpcb The connection pcb which is connected
	 * @param err An unused error code, always ERR_OK currently ;-) TODO!
	 *            Only return ERR_ABRT if you have called tcp_abort from within the
	 *            callback function!
	 *
	 * @note When a connection attempt fails, the error callback is currently called!
	 */
	err_t static _tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
};

#endif /* TCPSERIALADAPTER2_H_ */
