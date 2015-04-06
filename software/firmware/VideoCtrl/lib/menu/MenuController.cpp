/*
 * MenuController.cpp
 *
 *  Created on: 03.04.2015
 *      Author: Peter
 */

#include "MenuController.h"

MenuController::MenuController() {
    _currentPage = NULL;
    _swapToMenu = 0;
    _liveActive = false;
}

void MenuController::init(MenuPage* initialPage) {
    _currentPage = initialPage;
    memset(_glcdAltBuffer, 0, MENU_BUFFER_SIZE);

    glcd_tiny_set_font(Font5x7,5,7,32,127);
}

void MenuController::draw() {
    if (_currentPage == NULL)
        return;

    if (_swapToMenu > 0 && chTimeNow() >= _swapToMenu) {
        glcd_buffer_selected = glcd_buffer;
        glcd_bbox_refresh();
        _swapToMenu = 0;
    }

    if (glcd_buffer_selected == glcd_buffer) {
        glcd_clear_buffer();
        _currentPage->draw();
    }
}

void MenuController::log(char* msg) {
    if (_swapToMenu == 0) {
        memcpy(_glcdAltBuffer, glcd_buffer, MENU_BUFFER_SIZE);
        _line = 0;

        glcd_buffer_selected = _glcdAltBuffer;
        glcd_bbox_refresh();

        glcd_fill_rect(3, 3, MENU_WIDTH - 6, MENU_HEIGHT - 6, WHITE);
        glcd_draw_rect(2, 2, MENU_WIDTH - 4, MENU_HEIGHT - 4, BLACK);

        _swapToMenu = chTimeNow() + S2ST(2);
    }

    if (_line == 7) {
        _line = 0;
    }

    glcd_draw_string_xy(4, 4 + (_line * 8), msg);

    _line += 1;
}

void MenuController::buttonDown(uint8_t id) {
    if (_currentPage == NULL)
        return;

    _currentPage->buttonDown(id);

    MenuPage* next = _currentPage->getPage(id);

    if (next != NULL) {
        _currentPage = next;
    }
}

void MenuController::rotaryChange(uint8_t id, uint8_t value) {
    if (_currentPage == NULL)
        return;

    _currentPage->rotaryChange(id, value);
}

void MenuController::liveToggle() {
    glcd_set_pixel(127, 0, _liveActive ? BLACK : WHITE);
    glcd_set_pixel(127, 1, _liveActive ? BLACK : WHITE);
    glcd_set_pixel(126, 0, _liveActive ? WHITE : BLACK);
    glcd_set_pixel(126, 1, _liveActive ? WHITE : BLACK);
    glcd_write();

    _liveActive = !_liveActive;
}
