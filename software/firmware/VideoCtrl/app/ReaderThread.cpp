/*
 * ReaderThread.cpp
 *
 *  Created on: 13.11.2013
 *      Author: Peter Schuster
 */

#include "ReaderThread.h"

ReaderThread::ReaderThread() {
    _list = NULL;
    _max_freq = 1;
}

void ReaderThread::add(Buttons* button, uint8_t freq) {
    button_item_t* item = new button_item_t(); //(button_item_t*)chHeapAlloc(NULL, sizeof(button_item_t));
    item->freq = (uint8_t)(1 << freq); // 1, 2, 4, 8, 16, ...
    item->intervall = 0;
    item->item = button;
    item->next = NULL;

    if (item->freq > _max_freq) {
        _max_freq = item->freq;
    }

    item->intervall = _max_freq / item->freq;

    if (_list == NULL) {
        _list = item;
    } else {
        button_item_t* p;
        p = _list;
        while (p->next != NULL) {
            p->intervall = _max_freq / p->freq;
            p = p->next;
        }
        p->intervall = _max_freq / p->freq;
        p->next = item;
    }
}

/**
 * Input reader thread.
 */
msg_t ReaderThread::main(void) {

  setName("inputReader");

  /* Goes to the final priority after initialization.*/
  setPriority(HIGHPRIO);

  uint16_t n = 0;

  uint32_t ticks = (uint32_t)1000000 / (uint32_t)_max_freq;

  systime_t time_d = US2ST(ticks);

  systime_t time = chTimeNow();     // T0

  while(1) {
      time += time_d;            // Next deadline

      _tick(n);

      n = (n + 1) % _max_freq;

      chThdSleepUntil(time);
  }

  return RDY_OK;
}

void ReaderThread::_tick(uint8_t n) {
    button_item_t* p;
    p = _list;
    while (p != NULL) {
        if (n % p->intervall == 0) {
            p->item->readButtonStatus();
        }

        p = (button_item_t*)p->next;
    }
}
