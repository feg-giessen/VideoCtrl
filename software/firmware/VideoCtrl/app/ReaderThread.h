/*
 * ReaderThread.h
 *
 *  Created on: 13.11.2013
 *      Author: Peter Schuster
 */

#ifndef READERTHREAD_H_
#define READERTHREAD_H_

#include "ch.hpp"
#include "../lib/Buttons.h"

#define FREQ_DIV 1000

typedef struct button_item_t {
    uint8_t         freq;
    uint16_t        intervall;
    button_item_t*  next;
    Buttons*        item;
    bool            online_cached;
} button_item_t;

class ReaderThread : public chibios_rt::BaseStaticThread<1024> {
private:
    uint8_t         _max_freq;
    button_item_t*  _list;

public:
	ReaderThread();
	void add(Buttons* button, uint8_t freq);
protected:
    virtual msg_t main(void);
private:
    void _tick(uint8_t n);
};

#endif /* READERTHREAD_H_ */
