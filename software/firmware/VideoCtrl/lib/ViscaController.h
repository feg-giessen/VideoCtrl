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

#define VISCA_BUFFER 20

#define VISCA_SOCKET_COUNT 2

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

    ViscaStates _state;
    ViscaSocketStates _sockets[VISCA_SOCKET_COUNT];

    uint8_t _cam_addr;
public:
    ViscaController();
    void begin(SerialDriver* sdp);

    void processPackets();
    int8_t getPacket();

    /**
     * Auto reply to cam messages
     */
    void autoReply();

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
