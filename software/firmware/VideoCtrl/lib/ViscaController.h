/*
 * ViscaController.h
 *
 *  Created on: 03.02.2014
 *      Author: Peter Schuster
 */

#ifndef VISCACONTROLLER_H_
#define VISCACONTROLLER_H_

#include "hal.h"
#include "visca.h"
#include "debugger.h"

#define VISCA_BUFFER        20
#define VISCA_SOCKET_COUNT  2

#if DEBUG
#define PTZ_DEBUG(...) debug_printf((uint8_t*)"PTZ ", __VA_ARGS__)
#else
#define PTZ_DEBUG(...)
#endif

enum ViscaStates {
    ViscaState_Initial = 0,
    ViscaState_NetworkChangeReq,
    ViscaState_AddressSet,
    ViscaState_Active
};

enum ViscaSocketStates {
    ViscaSocketState_Empty = 0,
    ViscaSocketState_Executing
};

class ViscaController {
private:
    SerialDriver* _sdp;

    uint8_t _buffer[VISCA_BUFFER];
    uint8_t _buffer_index;

    ViscaStates _state;
    ViscaSocketStates _sockets[VISCA_SOCKET_COUNT];

    uint8_t _cam_addr;
    uint16_t _sent_count;
    uint16_t _recv_count;
public:
    ViscaController();
    void begin(SerialDriver* sdp);

    void processPackets();
    int8_t getPacket();

    /**
     * Auto reply to cam messages
     */
    void autoReply();

    void setPower(bool power);
    void setInfoDisplay(bool show);
    void camMemory(uint8_t memory, bool setMem);
    void camDrive(uint8_t x, uint8_t y, uint8_t d1, uint8_t d2);
    void camZoom(uint8_t z);
    void camFocus(uint8_t z);
    void camMan();
    void camAuto();

private:
    /**
     * Parse packet from input buffer
     */
    void _parsePacket(uint8_t* buffer, size_t length);

    /*
     * Clear input queue buffer till next packet.
     */
    void _clearIQ();
};

#endif /* VISCACONTROLLER_H_ */
