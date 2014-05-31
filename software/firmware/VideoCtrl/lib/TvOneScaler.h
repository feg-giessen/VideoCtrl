/*
 * TvOneScaler.h
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#ifndef TVONESCALER_H_
#define TVONESCALER_H_

#include "net/TcpSerialAdapter2.h"

#define SOURCES_COUNT   5
#define OUTPUTS_COUNT  20
#define SIZES_COUNT     5

#define SCALER_CMD_NONE    0
#define SCALER_CMD_PWR     1
#define SCALER_CMD_SRCS    2
#define SCALER_CMD_OUT     3
#define SCALER_CMD_SIZE    4

class TvOneScaler {
private:
    static const char* _sources[];
    static const char* _outputs[];
    static const char* _sizes[];

    TcpSerialAdapter2 _client;

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

private:
    void static _recv_cb(err_t err, void* context, char* result, size_t length, void* arg);
    void _parsePower(char* result, size_t len);
    void _parseSource(char* result, size_t len);
    void _parseOutput(char* result, size_t len);
    void _parseSize(char* result, size_t len);
};

#endif /* TVONESCALER_H_ */
