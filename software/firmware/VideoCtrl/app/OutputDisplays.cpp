/*
 * OutputDisplays.cpp
 *
 *  Created on: 02.04.2014
 *      Author: Peter Schuster
 */

#include "OutputDisplays.h"

OutputDisplays::OutputDisplays() {
    _projetorLi_vmute = false;
    _projetorRe_vmute = false;
    _blink = 0;
    _run = 0;

    uint8_t i;
    for (i = 0; i < 8; i++) {
        _led_color[i] = BI8_COLOR_OFF;
    }
}

void OutputDisplays::begin(
        ip_addr_t addr_proj_li, uint16_t port_proj_li,
        ip_addr_t addr_proj_re, uint16_t port_proj_re,
        ip_addr_t addr_klSaal_li, uint16_t port_klSaal_li,
        ip_addr_t addr_klSaal_re, uint16_t port_klSaal_re,
        ip_addr_t addr_stage, uint16_t port_stage,
        SkaarhojBI8* bi8) {

    _bi8 = bi8;

    _projectorLi.begin(addr_proj_li, port_proj_li);
    _projectorRe.begin(addr_proj_re, port_proj_re);
}

void OutputDisplays::run() {
    bool projLiOnline = _projectorLi.isRemoteAvailable();
    bool projReOnline = _projectorRe.isRemoteAvailable();

    _run += 1;

    if (_run % 1000 == 0) {
        _projectorLi.readStatus();
        _projectorRe.readStatus();
    } else if (_run % 200 == 0) {
        if (projLiOnline) _projectorLi.readStatus();
        if (projReOnline) _projectorRe.readStatus();
    }

    // reset video mute on device OFF
    if (!_projectorLi.hasPower()) {
        _projetorLi_vmute = false;
    }
    if (!_projectorRe.hasPower()) {
        _projetorRe_vmute = false;
    }

    uint16_t down = _bi8->buttonDownAll();

    //
    // Button presses

    if (projLiOnline && ((down >> 4) & 0x01) == 0x01) {  // Button 5 -> LI
        _projectorLi.setPower(!_projectorLi.hasPower());
    }
    if (projReOnline && ((down >> 5) & 0x01) == 0x01) {  // Button 6 -> RE
        _projectorRe.setPower(!_projectorRe.hasPower());
    }
    if (((down >> 6) & 0x01) == 0x01) {  // Button 7 -> Kl.Saal
    }
    if (((down >> 7) & 0x01) == 0x01) {  // Button 8 -> StageDisplay
    }
    if (projLiOnline && (down & 0x01) == 0x01) {         // Button 1 -> BLK LI
        _projectorLi.setVideoMute(!_projetorLi_vmute);
        _projetorLi_vmute = !_projetorLi_vmute;
    }
    if (projReOnline && ((down >> 1) & 0x01) == 0x01) {  // Button 2 -> BLK RE
        _projectorRe.setVideoMute(!_projetorRe_vmute);
        _projetorRe_vmute = !_projetorRe_vmute;
    }
    if (((down >> 2) & 0x01) == 0x01) {  // Button 3 -> BLK Kl.Saal
    }
    if (((down >> 3) & 0x01) == 0x01) {  // Button 4 -> BLK StageDisplay
    }
}

void OutputDisplays::doBlink() {

    if (_bi8 == NULL)
        return;         // Module is not initialized yet!

    _blink += 1;

    _processProjectorLeds(&_projectorLi, 4);
    _processProjectorLeds(&_projectorRe, 5);

    if (_projectorLi.isRemoteAvailable() && _projectorLi.hasPower()) {
        _led_color[0] = _projetorLi_vmute ? BI8_COLOR_RED : BI8_COLOR_BACKLIGHT;
    } else {
        _led_color[0] = BI8_COLOR_OFF;
    }

    if (_projectorRe.isRemoteAvailable() && _projectorRe.hasPower()) {
        _led_color[1] = _projetorRe_vmute ? BI8_COLOR_RED : BI8_COLOR_BACKLIGHT;
    } else {
        _led_color[1] = BI8_COLOR_OFF;
    }

    uint8_t i;
    for (i = 0; i < 8; i++) {
        _bi8->setButtonColor(i + 1, _led_color[i]);
    }
}

void OutputDisplays::_processProjectorLeds(ProjectorCtrl* proj, uint8_t button) {
    if (!proj->isRemoteAvailable() || proj->getStatus() == 99) {
        // no status available --> offline
        _led_color[button] = BI8_COLOR_OFF;
    } else if (proj->isErrorStatus()) {
        _led_color[button] = _led_color[button] == BI8_COLOR_RED
                ? BI8_COLOR_BACKLIGHT
                : BI8_COLOR_RED;
    } else {
        if (proj->isPrePhase()) {
            if (_blink % 2 == 0) {
                _led_color[button] = (_blink % 4 == 0)
                        ? BI8_COLOR_BACKLIGHT
                        : BI8_COLOR_GREEN;
            }
        } else if (proj->isPostPhase()) {
            if (_blink % 2 == 0) {
                _led_color[button] = (_blink % 4 == 0)
                    ? BI8_COLOR_BACKLIGHT
                    : BI8_COLOR_RED;
            }
        } else if (proj->hasPower()) {
            _led_color[button] = BI8_COLOR_GREEN;
        } else {
            _led_color[button] = BI8_COLOR_BACKLIGHT;
        }
    }
}
