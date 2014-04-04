/*
 * TvOneScaler.h
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#ifndef TVONESCALER_H_
#define TVONESCALER_H_

#include "net/TcpSerialAdapter.h"

#define SOURCES_COUNT   5
#define OUTPUTS_COUNT  20
#define SIZES_COUNT     5

class TvOneScaler {
private:
    static const char* _sources[];
    static const char* _outputs[];
    static const char* _sizes[];

    TcpSerialAdapter _client;

    bool _power;
    int8_t _source;
    int8_t _output;
    int8_t _size;
public:
    TvOneScaler();
    void begin(ip_addr_t addr, uint16_t port);

    bool getPower();
    int8_t getSource();
    int8_t getOutput();
    int8_t getSize();

    void readPower();
    void readSource();
    void readOutput();
    void readSize();

    void setPower(bool power);
    void setSource(uint8_t value);
    void setOutput(uint8_t value);
    void setSize(uint8_t value);
};

#endif /* TVONESCALER_H_ */
