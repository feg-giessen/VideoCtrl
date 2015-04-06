/*
 * MenuPage.cpp
 *
 *  Created on: 05.01.2015
 *      Author: Peter
 */

#include "MenuPage.h"

extern "C" {
#include "glcd.h"
#include "glcd_graphs.h"
#include "glcd_controllers.h"
#include "glcd_text.h"
#include "glcd_text_tiny.h"
}

MenuPage::MenuPage() {
    uint8_t i;

    _updateInterval = 0;

    for (i = 0; i < 4; i++) {
        _btnAvail[i] = false;
        _navigation[i] = NULL;
        _btnActions[i] = NULL;

        memset(_btnTexts[i], 0, 6);
    }
}

void MenuPage::setText(uint8_t button, char* text) {
    if (button >= 4) {
        chDbgAssert(false, "button no > 3",(void));
        return;
    }

    // clear button text
    memset(_btnTexts[button], 0, 6);

    // copy
    for (uint8_t i = 0; i < 6; i++) {
        char c = *(text);

        if (c == 0)
            break;

        _btnTexts[button][i] = c;
        text++;
    }
}

void MenuPage::setAction(uint8_t button, btn_action action) {
    if (button >= 4) {
        chDbgAssert(false, "button no > 3",(void));
        return;
    }

    _btnActions[button] = action;
}

MenuPage* MenuPage::getPage(uint8_t button) {
    if (button >= 4)
        return NULL;

    return _navigation[button];
}

void MenuPage::setPage(uint8_t button, MenuPage* page) {
    if (button >= 4)
        return;

    _navigation[button] = page;
}

uint16_t MenuPage::getUpdateInterval() {
    return _updateInterval;
}

void MenuPage::buttonDown(uint8_t button) {
    if (button >= 4) {
        chDbgAssert(false, "button no > 3",(void));
        return;
    }

    // execute button action
    if (_btnActions[button] != NULL) {
        _btnActions[button]();
    }
}

void MenuPage::rotaryChange(uint8_t id, uint8_t value) {
    // none
}

void MenuPage::drawButtonFrame() {
    uint8_t var = 0;
    uint8_t width = MENU_WIDTH >> 2;

    // blank lower screen
    var = MENU_BUTTON_TXT_START_Y - 1;
    glcd_fill_rect(0, var, MENU_WIDTH, MENU_HEIGHT - MENU_BUTTON_TXT_START_Y - 1, WHITE);

    // horizontal
    var = MENU_BUTTON_TXT_START_Y - 2;
    glcd_draw_line(0, var, MENU_WIDTH - 1, var, BLACK);

    // vertical lines
    var = 0;
    for (uint8_t i = 0; i < 3; i++) {
        var += width;
        glcd_draw_line(var, MENU_BUTTON_TXT_START_Y - 1, var, MENU_HEIGHT - 1, BLACK);
    }

    // button texts
    for (var = 0; var < 4; var++) {
        drawButton(var, _btnTexts[var]);
    }
}

void MenuPage::drawButton(uint8_t btn_number, char* text) {
    uint8_t width = MENU_WIDTH >> 2;
    uint8_t start = MENU_BUTTON_TXT_START_X + (width * btn_number);

    glcd_draw_string_xy(start, MENU_BUTTON_TXT_START_Y, text);
}
