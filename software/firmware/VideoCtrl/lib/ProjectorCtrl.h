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
#include "RemoteAdapter.h"

#define PROJECTOR_CMD_NONE      0
#define PROJECTOR_CMD_STAT      1
#define PROJECTOR_CMD_TEMP      2

class ProjectorCtrl : public RemoteAdapter {
    static const uint8_t _statusCodes[];
    static const char* _statusMessages[];
    static const uint8_t _statusCodesLength;
    static const uint8_t _statusInvalid;
private:
    uint8_t _status;
    char    _temp1[4];
    char    _temp2[4];
    char    _temp3[4];

    bool    _reading_stat;
    bool    _reading_temp;
public:
    ProjectorCtrl();
    void begin(ip_addr_t addr, uint16_t port);

    void readStatus();
    void readTemperatures();

    uint8_t getStatus();
    const char* getStatusMessage();
    char* getTemperature(uint8_t number);

    bool hasPower();
    bool isPrePhase();
    bool isPostPhase();
    bool isErrorStatus();

    void setPower(bool value);
    void setVideoMute(bool value);

private:
    void static _recv_cb(err_t err, void* context, char* result, size_t length, void* arg);
    void _parseStatus(char* result, size_t length);
    void _parseTemperatures(char* result, size_t length);
};

#endif /* PROJECTORCTRL_H_ */
