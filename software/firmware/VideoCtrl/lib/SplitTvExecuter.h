/*
 * SplitTvExecuter.h
 *
 *  Created on: 07.10.2014
 *      Author: Peter
 */

#ifndef SPLITTVEXECUTER_H_
#define SPLITTVEXECUTER_H_

#include "LgTv.h"

class SplitTvExecuter {
private:
    LgTv* _tv1;
    LgTv* _tv2;

    bool _stablePower;
    bool _stableVideoMute;
    bool _nextPower;
    bool _nextVideoMute;

    bool _nextPowerInitialized;
    bool _nextVideoMuteInitialized;
public:
    SplitTvExecuter();
    void begin(LgTv* tv1, LgTv* tv2);

    void run();

    bool getPower();
    bool getVideoMute();

    void setPower(bool value);
    void setVideoMute(bool value);
};

#endif /* SPLITTVEXECUTER_H_ */
