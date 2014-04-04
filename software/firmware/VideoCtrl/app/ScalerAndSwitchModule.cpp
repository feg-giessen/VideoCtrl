/*
 * ScalerAndSwitchModule.cpp
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#include "ScalerAndSwitchModule.h"

ScalerAndSwitchModule::ScalerAndSwitchModule() {
    _bi8 = NULL;
}

void ScalerAndSwitchModule::begin(
        ip_addr_t switch_ip, uint16_t switch_port,
        ip_addr_t scaler_ip, uint16_t scaler_port,
        SkaarhojBI8* bi8) {

    _bi8 = bi8;

    _switch.begin(switch_ip, scaler_port);
    _scaler.begin(scaler_ip, scaler_port);
}

void ScalerAndSwitchModule::run() {
    uint16_t down = _bi8->buttonDownAll();

    //
    // Switch

    if (((down >> 3) & 0x01) == 0x01) {  // Button 4
        _switch.setInput(1);
    }
    if (((down >> 2) & 0x01) == 0x01) {  // Button 3
        _switch.setInput(2);
    }
    if (((down >> 1) & 0x01) == 0x01) {  // Button 2
        _switch.setInput(3);
    }
    if (((down >> 0) & 0x01) == 0x01) {  // Button 1
        _switch.setInput(4);
    }

    uint8_t input;
    input = _switch.getInput();

    _bi8->setButtonColor(4, input == 1 ? BI8_COLOR_ON : BI8_COLOR_BACKLIGHT);
    _bi8->setButtonColor(3, input == 2 ? BI8_COLOR_ON : BI8_COLOR_BACKLIGHT);
    _bi8->setButtonColor(2, input == 3 ? BI8_COLOR_ON : BI8_COLOR_BACKLIGHT);
    _bi8->setButtonColor(1, input == 4 ? BI8_COLOR_ON : BI8_COLOR_BACKLIGHT);

    //
    // Scaler

    if (((down >> 7) & 0x01) == 0x01) {                        // Button 8
        _scaler.setPower(!_scaler.getPower());
        _scaler.readPower();
    }
    if (((down >> 6) & 0x01) == 0x01 && _scaler.getPower()) {  // Button 7
        _scaler.setSource(3);   // VGA
        _scaler.readSource();
    }
    if (((down >> 5) & 0x01) == 0x01 && _scaler.getPower()) {  // Button 6
        _scaler.setSource(4);   // HDMI
        _scaler.readSource();
    }
    if (((down >> 4) & 0x01) == 0x01 && _scaler.getPower()) {  // Button 5
        _scaler.setSource(2);   // COMP
        _scaler.readSource();
    }

    if (_scaler.getPower()) {
        int8_t source = _scaler.getSource();

        _bi8->setButtonColor(8, BI8_COLOR_BACKLIGHT);
        _bi8->setButtonColor(7, source == 3 ? BI8_COLOR_ON : BI8_COLOR_BACKLIGHT);
        _bi8->setButtonColor(6, source == 4 ? BI8_COLOR_ON : BI8_COLOR_BACKLIGHT);
        _bi8->setButtonColor(5, source == 2 ? BI8_COLOR_ON : BI8_COLOR_BACKLIGHT);
    } else {
        _bi8->setButtonColor(8, BI8_COLOR_RED);
        _bi8->setButtonColor(7, BI8_COLOR_OFF);
        _bi8->setButtonColor(6, BI8_COLOR_OFF);
        _bi8->setButtonColor(5, BI8_COLOR_OFF);
    }
}

void ScalerAndSwitchModule::update() {
    _scaler.readPower();

    if (_scaler.getPower()) {
        _scaler.readSource();
        _scaler.readOutput();
        _scaler.readSize();
    }
}
