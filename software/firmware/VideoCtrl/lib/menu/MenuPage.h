/*
 * MenuPage.h
 *
 *  Created on: 05.01.2015
 *      Author: Peter
 */

#ifndef MENUPAGE_H_
#define MENUPAGE_H_

#include <stdint.h>
#include <string.h>

#define MENU_BTN_0  0
#define MENU_BTN_1  1
#define MENU_BTN_2  2
#define MENU_BTN_3  3

#define MENU_ROT_0  0
#define MENU_ROT_1  1

#define MENU_WIDTH  128
#define MENU_HEIGHT 64

#define MENU_BUTTON_TXT_START_X 2
#define MENU_BUTTON_TXT_START_Y 56

typedef void (*btn_action)(void);

/*
 * BACK
 * ZURÜCK
 * <--
 *
 * OK
 *
 * LADEN
 * LOAD
 *
 * SPEICHERN
 * STORE
 * SAVE
 *
 * ABBR.
 * ESCAPE
 */

class MenuPage {
protected:
    bool        _btnAvail[4];
    MenuPage*   _navigation[4];
    uint16_t    _updateInterval;
    char        _btnTexts[4][6];
    btn_action  _btnActions[4];
public:
    MenuPage();

    MenuPage* getPage(uint8_t button);
    void setPage(uint8_t button, MenuPage* page);

    void setText(uint8_t button, char* text);
    void setAction(uint8_t button, btn_action action);

    virtual void draw() = 0;

    virtual void buttonDown(uint8_t id);
    virtual void rotaryChange(uint8_t id, uint8_t value);

    /**
     * ms after which view should be re-drawn.
     */
    uint16_t getUpdateInterval();
protected:
    /**
     * Blanks the lower screen, draws the button lines
     * and texts.
     */
    void drawButtonFrame();

private:
    /**
     * Draws the button text (max 5 chars).
     */
    void drawButton(uint8_t btn_number, char* text);
};

#endif /* MENUPAGE_H_ */
