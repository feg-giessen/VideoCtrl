/*
 * ScalerAndSwitchModule.cpp
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#include "ScalerAndSwitchModule.h"

ScalerAndSwitchModule::ScalerAndSwitchModule() {
    _bi8 = NULL;
    _run = 0;
}

void ScalerAndSwitchModule::begin(
        ip_addr_t switch_ip, uint16_t switch_port,
        ip_addr_t scaler_ip, uint16_t scaler_port,
        SkaarhojBI8* bi8) {

    _bi8 = bi8;
    _run = 33;  // random seed

    _switch.begin(switch_ip, switch_port);
    _scaler.begin(scaler_ip, scaler_port);
}

void ScalerAndSwitchModule::run() {
    _run++;

    uint16_t down = _bi8->buttonDownAll();

    //
    // Switch

    if (_switch.isRemoteAvailable()) {

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

        _bi8->setButtonColor(4, input == 1 ? BI8_COLOR_GREEN : BI8_COLOR_BACKLIGHT);
        _bi8->setButtonColor(3, input == 2 ? BI8_COLOR_GREEN : BI8_COLOR_BACKLIGHT);
        _bi8->setButtonColor(2, input == 3 ? BI8_COLOR_GREEN : BI8_COLOR_BACKLIGHT);
        _bi8->setButtonColor(1, input == 4 ? BI8_COLOR_GREEN : BI8_COLOR_BACKLIGHT);

    } else if (_run % 300 == 0) {

        _switch.enableButtons(true);

        _bi8->setButtonColor(4, BI8_COLOR_OFF);
        _bi8->setButtonColor(3, BI8_COLOR_OFF);
        _bi8->setButtonColor(2, BI8_COLOR_OFF);
        _bi8->setButtonColor(1, BI8_COLOR_OFF);
    }

    //
    // Scaler

    if (_scaler.isRemoteAvailable()) {

        //
        // Button handling

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

        //
        // Regular updates

        if (_run % 300 == 0) {
            _scaler.readPower();

            if (_scaler.getPower()) {
                _scaler.readSource();
            }
        }

        if (_run % 500 == 0 && _scaler.getPower()) {
            _scaler.readOutput();
            _scaler.readSize();
        }
    } else if (_run % 900 == 0) {
        // scaler is offline, check again...
        _scaler.readPower();

        _bi8->setButtonColor(8, BI8_COLOR_OFF);
        _bi8->setButtonColor(7, BI8_COLOR_OFF);
        _bi8->setButtonColor(6, BI8_COLOR_OFF);
        _bi8->setButtonColor(5, BI8_COLOR_OFF);
    }
}

void ScalerAndSwitchModule::setFormatFromAtem(uint8_t vidM) {
    // ATEM Video format:
    //   525i59.94 NTSC (0), 625i50 PAL (1), 720p50 (2), 720p59.94 (3), 1080i50 (4), 1080i59.94 (5)

    if (!_scaler.getPower())
        return;

    uint8_t output;

    switch (vidM) {
    case 0:          // 525i59.94 NTSC (0)
        output = 6;  // 480I
        break;
    case 1:          // 625i50 PAL (1)
        output = 11; // 576I
        break;
    case 2:          // 720p50 (2)
        output = 13; // 720P
        break;
    case 3:          // 720p59.94 (3)
        output = 8;  // 720P
        break;
    case 4:          // 1080i50 (4)
        output = 14; // 1080I50
        break;
    case 5:          // 1080i59.94 (5)
        output = 9;  // 1080I
        break;
    default:
        return;
    }

    if (_scaler.getOutput() != output) {
        _scaler.setOutput(output);
        _scaler.readOutput();
    }
}
