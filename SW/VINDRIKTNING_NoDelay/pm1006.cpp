#include <Arduino.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "pm1006.h"

#define DEFAULT_TIMEOUT 1000

PM1006::PM1006(Stream * serial, bool debug)
{
    _serial = serial;
    _debug = debug;

    _state = PM1006_HEADER;
    _rxlen = 0;
    _index = 0;
    memset(_rxbuf, 0, sizeof(_rxbuf));
    memset(_txbuf, 0, sizeof(_txbuf));
    _checksum = 0;
}

bool PM1006::read_pm25(uint16_t *pm)
{
    uint8_t cmd[] = {0x0B, 0x01};
    if (send_command(2, cmd) && (_rxlen > 4) && (_rxbuf[0] == cmd[0])) {
        *pm = (_rxbuf[3] << 8) + _rxbuf[4];
        return true;
    }
    return false;
}

// sends a command and waits for response, returns length of response
bool PM1006::send_command(size_t cmd_len, const uint8_t *cmd_data)
{
    // build and send command
    int txlen = build_tx(cmd_len, cmd_data);
    _serial->write(_txbuf, txlen);

    // wait for response
    unsigned long start = millis();
    while ((millis() - start) < DEFAULT_TIMEOUT) {
        while (_serial->available()) {
            char c = _serial->read();
            if (process_rx(c)) {
                return true;
            }
        }
        yield();
    }

    // timeout
    return false;
}

// builds a tx buffer, returns length of tx data
int PM1006::build_tx(size_t cmd_len, const uint8_t *cmd_data)
{
    int len = 0;
    _txbuf[len++] = 0x11;
    _txbuf[len++] = cmd_len;
    for (size_t i = 0; i < cmd_len; i++) {
        _txbuf[len++] = cmd_data[i];
    }
    uint8_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += _txbuf[i];
    }
    _txbuf[len++] = (256 - sum) & 0xFF;
    return len;
}

// processes one rx character, returns true if a valid frame was found
bool PM1006::process_rx(uint8_t c)
{
    switch (_state) {
    case PM1006_HEADER:
        _checksum = c;
        if (c == 0x16) {
            _state = PM1006_LENGTH;
        }
        break;

    case PM1006_LENGTH:
        _checksum += c;
        if (c <= sizeof(_rxbuf)) {
            _rxlen = c;
            _index = 0;
            _state = (_rxlen > 0) ? PM1006_DATA : PM1006_CHECK;
        } else {
            _state = PM1006_HEADER;
        }
        break;

    case PM1006_DATA:
        _checksum += c;
        _rxbuf[_index++] = c;
        if (_index == _rxlen) {
            _state = PM1006_CHECK;
        }
        break;

    case PM1006_CHECK:
        _checksum += c;
        _state = PM1006_HEADER;
        return (_checksum == 0);

    default:
        _state = PM1006_HEADER;
        break;
    }
    return false;
}

