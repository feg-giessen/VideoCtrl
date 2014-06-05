/*
 * HdmiSwitch.h
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#ifndef HDMISWITCH_H_
#define HDMISWITCH_H_

#include "RemoteAdapter.h"

class HdmiSwitch : public RemoteAdapter {
private:
    unsigned char _status;
public:
    HdmiSwitch();
    void begin(ip_addr_t addr, uint16_t port);
    void setInput(u8_t input);
    bool enableButtons(bool enabled);
    u8_t getInput();

private:
    void static _recv_cb(err_t err, void* context, char* result, size_t length, void* arg);
};

#endif /* HDMISWITCH_H_ */
