#include <Arduino.h>

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PM1006_HEADER,
    PM1006_LENGTH,
    PM1006_DATA,
    PM1006_CHECK
} pm1006_state_t;

class PM1006 {

private:
    Stream *_serial;
    bool _debug;

    pm1006_state_t _state;
    size_t _rxlen;
    size_t _index;
    uint8_t _txbuf[16];
    uint8_t _rxbuf[20];
    uint8_t _checksum;
    
    bool send_command(size_t cmd_len, const uint8_t *cmd_data);
    int build_tx(size_t cmd_len, const uint8_t *cmd_data);
    bool process_rx(uint8_t c);

public:
    static const int BIT_RATE = 9600;

    /**
     * Constructor.
     *
     * @param serial the serial port, NOTE: the serial port has to be configured for a bit rate of PM1006::BIT_RATE !
     */
    explicit PM1006(Stream *serial, bool debug = false);
 
     /**
     * Reads the PM2.5 value (plus some other yet unknown parameters)
     *
     * @param pm the PM2.5 value
     * @return true if the value was read successfully
     */
    bool read_pm25(uint16_t *pm);

};

