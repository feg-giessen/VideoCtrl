/*
 * HdmiSwitch.h
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#ifndef HDMISWITCH_H_
#define HDMISWITCH_H_

#include "net/TcpSerialAdapter.h"

class HdmiSwitch {
private:
    TcpSerialAdapter _serial;
    unsigned char _status;
public:
    HdmiSwitch();
    void begin(ip_addr_t addr, uint16_t port);
    void setInput(u8_t input);
    u8_t getInput();
};

#endif /* HDMISWITCH_H_ */
