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

    _tvKlSaalLi.begin(addr_klSaal_li, port_klSaal_li);
    _tvKlSaalRe.begin(addr_klSaal_re, port_klSaal_re);
    _tvStageDisplay.begin(addr_stage, port_stage);
}

void OutputDisplays::run() {
    bool projLiOnline = _projectorLi.isRemoteAvailable();
    bool projReOnline = _projectorRe.isRemoteAvailable();

    bool tvKlSaalLiOnline = _tvKlSaalLi.isRemoteAvailable();
    bool tvKlSaalReOnline = _tvKlSaalRe.isRemoteAvailable();
    bool tvStageOnline = _tvStageDisplay.isRemoteAvailable();

    _run += 1;

    if (_run % 2000 == 0) {
        _projectorLi.readStatus();
        _projectorRe.readStatus();
    } else if (_run % 400 == 0) {
        if (projLiOnline) _projectorLi.readStatus();
        if (projReOnline) _projectorRe.readStatus();
    }


    if (_run % 2400 == 0) {
        _tvKlSaalLi.readPower();
        _tvKlSaalRe.readPower();
        _tvStageDisplay.readPower();
    }
    if (_run % 1400 == 0) {
         if (tvKlSaalLiOnline) _tvKlSaalLi.readPower();
         if (tvKlSaalReOnline) _tvKlSaalRe.readPower();
         if (tvStageOnline) _tvStageDisplay.readPower();
     } else if (_run % 700 == 0) {
         if (tvKlSaalLiOnline) _tvKlSaalLi.readVideoMute();
         if (tvKlSaalReOnline) _tvKlSaalRe.readVideoMute();
         if (tvStageOnline) _tvStageDisplay.readVideoMute();
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
        bool tempVal = !(_tvKlSaalLi.getPower() && _tvKlSaalRe.getPower());
        _tvKlSaalLi.setPower(tempVal);
        _tvKlSaalRe.setPower(tempVal);
    }
    if (((down >> 7) & 0x01) == 0x01) {  // Button 8 -> StageDisplay
        _tvStageDisplay.setPower(!_tvStageDisplay.getPower());
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
        bool tempVal = !(_tvKlSaalLi.getVideoMute() && _tvKlSaalRe.getVideoMute());
        _tvKlSaalLi.setVideoMute(tempVal);
        _tvKlSaalRe.setVideoMute(tempVal);
    }
    if (((down >> 3) & 0x01) == 0x01) {  // Button 4 -> BLK StageDisplay
        _tvStageDisplay.setVideoMute(!_tvStageDisplay.getVideoMute());
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

    bool klSaalAvail = _tvKlSaalLi.isRemoteAvailable() || _tvKlSaalRe.isRemoteAvailable();

    if (klSaalAvail) {
        bool klSaalPwr = false;
        bool klSaalVmu = false;

        if (_tvKlSaalLi.isRemoteAvailable()) {
            klSaalPwr = _tvKlSaalLi.getPower();
            klSaalVmu = _tvKlSaalLi.getVideoMute();

            if (_tvKlSaalRe.isRemoteAvailable()) {
                klSaalPwr = klSaalPwr && _tvKlSaalRe.getPower();
                klSaalVmu = klSaalVmu && _tvKlSaalRe.getVideoMute();
            }
        }
        else if (_tvKlSaalRe.isRemoteAvailable()) {
            klSaalPwr = _tvKlSaalRe.getPower();
            klSaalVmu = _tvKlSaalRe.getVideoMute();
        }

        if (klSaalPwr) {
            _led_color[6] = BI8_COLOR_GREEN;
            _led_color[2] = klSaalVmu ? BI8_COLOR_RED : BI8_COLOR_BACKLIGHT;
        } else {
            _led_color[6] = BI8_COLOR_BACKLIGHT;
            _led_color[2] = BI8_COLOR_OFF;
        }
    } else {
        _led_color[6] = BI8_COLOR_OFF;
        _led_color[2] = BI8_COLOR_OFF;
    }

    if (_tvStageDisplay.isRemoteAvailable()) {
        if (_tvStageDisplay.getPower()) {
            _led_color[7] = BI8_COLOR_GREEN;
            _led_color[3] = _tvStageDisplay.getVideoMute() ? BI8_COLOR_RED : BI8_COLOR_BACKLIGHT;
        } else {
            _led_color[7] = BI8_COLOR_BACKLIGHT;
            _led_color[3] = BI8_COLOR_OFF;
        }
    } else {
        _led_color[7] = BI8_COLOR_OFF;
        _led_color[3] = BI8_COLOR_OFF;
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
