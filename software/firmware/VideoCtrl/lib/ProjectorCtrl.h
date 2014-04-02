/*
 * ProjectorCtrl.h
 *
 *  Created on: 18.11.2013
 *      Author: Peter Schuster
 */

#ifndef PROJECTORCTRL_H_
#define PROJECTORCTRL_H_

#include <stdint.h>
#include <string.h>
#include "lwip/ip_addr.h"
#include "net/TcpSerialAdapter.h"

class ProjectorCtrl {
    static const uint8_t _statusCodes[];
    static const char* _statusMessages[];
    static const uint8_t _statusCodesLength;
    static const uint8_t _statusInvalid;
private:
    TcpSerialAdapter _client;

    uint8_t _status;
    char    _temp1[4];
    char    _temp2[4];
    char    _temp3[4];

    bool	_online;
public:
    ProjectorCtrl();
    void begin(ip_addr_t addr, uint16_t port);

    bool readStatus();
    bool readTemperatures();

    uint8_t getStatus();
    char* getTemperature(uint8_t number);

    bool hasPower();
    bool isPrePhase();
    bool isPostPhase();
    bool isErrorStatus();

    void setPower(bool value);
    void setVideoMute(bool value);
};

#endif /* PROJECTORCTRL_H_ */
