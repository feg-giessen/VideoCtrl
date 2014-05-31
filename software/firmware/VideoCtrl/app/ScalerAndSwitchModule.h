/*
 * ScalerAndSwitchModule.h
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#ifndef SCALERANDSWITCHMODULE_H_
#define SCALERANDSWITCHMODULE_H_

#include "../lib/HdmiSwitch.h"
#include "../lib/TvOneScaler.h"
#include "../lib/SkaarhojBI8.h"

class ScalerAndSwitchModule {
private:
    HdmiSwitch _switch;
    TvOneScaler _scaler;
    SkaarhojBI8* _bi8;

    uint8_t _run;
    uint8_t _update;
public:
    ScalerAndSwitchModule();
    void begin(
            ip_addr_t switch_ip, uint16_t switch_port,
            ip_addr_t scaler_ip, uint16_t scaler_port,
            SkaarhojBI8* bi8);

    void run();
    void update();

    void setFormatFromAtem(uint8_t vidM);
};

#endif /* SCALERANDSWITCHMODULE_H_ */
