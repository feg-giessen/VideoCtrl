/*
 * HomeScreen.cpp
 *
 *  Created on: 06.04.2015
 *      Author: Peter
 */

#include "HomeScreen.h"

#include <string.h>

extern "C" {
#include "glcd.h"
#include "glcd_graphs.h"
#include "glcd_controllers.h"
#include "glcd_text.h"
#include "glcd_text_tiny.h"
}

HomeScreen::HomeScreen() {
}

void HomeScreen::setBuildTimestamp(const char* timestamp) {
    memcpy(_timestamp, timestamp, 19);
}

void HomeScreen::draw() {
    glcd_draw_string_xy(30,17,(char*)"FeG Giessen");
    glcd_draw_string_xy(20,32,(char*)"VideoController");
    //glcd_draw_string_xy(0,42,_timestamp);

    drawButtonFrame();
}
