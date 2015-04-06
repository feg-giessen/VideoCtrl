/*
 * HomeScreen.h
 *
 *  Created on: 06.04.2015
 *      Author: Peter
 */

#ifndef HOMESCREEN_H_
#define HOMESCREEN_H_

#include "../../lib/menu/MenuPage.h"

class HomeScreen: public MenuPage {
private:
    char _timestamp[19];
public:
    HomeScreen();
    void setBuildTimestamp(const char* timestamp);
    virtual void draw();
};

#endif /* HOMESCREEN_H_ */
