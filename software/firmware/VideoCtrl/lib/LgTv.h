/*
 * LgTv.h
 *
 *  Created on: 04.10.2014
 *      Author: Peter
 */

#ifndef LGTV_H_
#define LGTV_H_

#include <stdint.h>
#include <string.h>
#include "lwip/ip_addr.h"
#include "RemoteAdapter.h"

#define LGTV_CMD_NONE      0
#define LGTV_CMD_PWR       1
#define LGTV_CMD_VMU       2

class LgTv : public RemoteAdapter {
private:
    bool _power;
    bool _videoMute;
    bool _msg_in_process;
public:
    LgTv();
    void begin(ip_addr_t addr, uint16_t port);

    bool getPower();
    bool getVideoMute();

    void readPower();
    void readVideoMute();

    void setPower(bool value);
    void setVideoMute(bool value);

private:
    void static _recv_cb(err_t err, void* context, char* result, size_t length, void* arg);
};

#endif /* LGTV_H_ */