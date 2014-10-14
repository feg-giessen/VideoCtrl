/*
 * SplitTvExecuter.cpp
 *
 *  Created on: 07.10.2014
 *      Author: Peter
 */

#include "SplitTvExecuter.h"

SplitTvExecuter::SplitTvExecuter() {
    _tv1 = NULL;
    _tv2 = NULL;

    _stablePower = false;
    _nextPower = true;
    _nextPowerInitialized = false;

    _stableVideoMute = false;
    _nextVideoMute = false;
    _nextVideoMuteInitialized = false;
}

void SplitTvExecuter::begin(LgTv* tv1, LgTv* tv2) {
    _tv1 = tv1;
    _tv2 = tv2;
}

void SplitTvExecuter::run() {
    bool tv1Power;
    bool tv2Power;

    if (_tv1 != NULL) {
        tv1Power = _tv1->getPower();
        if (_nextPowerInitialized && _nextPower != tv1Power && _tv1->isRemoteAvailable()) {
            _tv1->setPower(_nextPower);
        }
    } else {
        tv1Power = false;
    }

    if (_tv2 != NULL) {
        tv2Power = _tv2->getPower();
        if (_nextPowerInitialized && _nextPower != tv2Power && _tv2->isRemoteAvailable()) {
            _tv2->setPower(_nextPower);
        }
    } else {
        tv2Power = false;
    }

    if (!_nextPowerInitialized && (_tv1->isPowerInitialized() || _tv2->isPowerInitialized())) {
        // first run
        _nextPower = tv1Power || tv2Power; // if one is ON the other one should follow.
        _nextPowerInitialized = true;
    } else if (_nextPower == tv1Power && _nextPower == tv2Power) {
        _stablePower = _nextPower;
    }

    bool tv1VideoMute;
    bool tv2VideoMute;

    if (_tv1 != NULL) {
        tv1VideoMute = _tv1->getVideoMute();
        if (_nextVideoMuteInitialized && _nextVideoMute != tv1VideoMute && _tv1->isRemoteAvailable()) {
            _tv1->setVideoMute(_nextVideoMute);
        }
    } else {
        tv1VideoMute = false;
    }

    if (_tv2 != NULL) {
        tv2VideoMute = _tv2->getVideoMute();
        if (_nextVideoMuteInitialized && _nextVideoMute != tv2VideoMute && _tv2->isRemoteAvailable()) {
            _tv2->setVideoMute(_nextVideoMute);
        }
    } else {
        tv2VideoMute = false;
    }

    if (!_nextVideoMuteInitialized && (_tv1->isVideoMuteInitialized() || _tv2->isVideoMuteInitialized())) {
        // first run
        _nextVideoMute = tv1VideoMute || tv2VideoMute; // if one is ON the other one should follow.
        _nextVideoMuteInitialized = true;
    } else if (_nextVideoMute == tv1VideoMute && _nextVideoMute == tv2VideoMute) {
        _stableVideoMute = _nextVideoMute;
    }
}

bool SplitTvExecuter::isActive() {
    return _nextPowerInitialized;
}

bool SplitTvExecuter::getPower() {
    if (_tv1 == NULL && _tv2 == NULL)
        return false;
    if (_tv1 == NULL)
        return _tv2->getPower();
    if (_tv2 == NULL)
        return _tv1->getPower();

    return _stablePower;
}

bool SplitTvExecuter::getVideoMute() {
    if (_tv1 == NULL && _tv2 == NULL)
        return false;
    if (_tv1 == NULL)
        return _tv2->getVideoMute();
    if (_tv2 == NULL)
        return _tv1->getVideoMute();

    return _stableVideoMute;
}

void SplitTvExecuter::setPower(bool value) {
    _nextPower = value;
    _nextPowerInitialized = true;

    if (_tv1 != NULL) _tv1->setPower(value);
    if (_tv2 != NULL) _tv2->setPower(value);
}

void SplitTvExecuter::setVideoMute(bool value) {
    _nextVideoMute = value;
    _nextVideoMuteInitialized = true;

    if (_tv1 != NULL) _tv1->setVideoMute(value);
    if (_tv2 != NULL) _tv2->setVideoMute(value);
}
