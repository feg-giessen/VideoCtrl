/*
 * MenuController.h
 *
 *  Created on: 03.04.2015
 *      Author: Peter
 */

#ifndef MENUCONTROLLER_H_
#define MENUCONTROLLER_H_

#define MENU_BUFFER_SIZE GLCD_LCD_WIDTH * GLCD_LCD_HEIGHT / 8

#include <stdint.h>

#include "chTypes.h"
#include "MenuPage.h"

extern "C" {
#include "glcd.h"
#include "glcd_graphs.h"
#include "glcd_controllers.h"
#include "glcd_text.h"
#include "glcd_text_tiny.h"
#include "font5x7.h"
}

class MenuController {
private:
    MenuPage*   _currentPage;
    bool        _liveActive;
    uint8_t     _line;
    uint8_t     _glcdAltBuffer[GLCD_LCD_WIDTH * GLCD_LCD_HEIGHT / 8];
    systime_t   _swapToMenu;

public:
    MenuController();
    void init(MenuPage* initialPage);

    void draw();

    void log(char* msg);

    void buttonDown(uint8_t id);
    void rotaryChange(uint8_t id, uint8_t value);

    void liveToggle();
};

#endif /* MENUCONTROLLER_H_ */
