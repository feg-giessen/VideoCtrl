/*
 * Stream.h
 *
 *  Created on: 29.09.2013
 *      Author: Peter Schuster
 */

#ifndef STREAM_H_
#define STREAM_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

class Stream {
private:
	size_t 		_len;
	uint16_t 	_offset;
	void* 		_ptr;

	void _free();

public:
	Stream();
	virtual ~Stream();
	void load(void* ptr, size_t len);
	uint16_t read(uint8_t* &ptr, uint16_t bytes);
	uint16_t copy(uint8_t* ptr, uint16_t bytes);
	void flush();
	size_t getLength();
	uint16_t getOffset();
	void reset();
};

#endif /* STREAM_H_ */
