//
// Created by leon on 13.01.20.
//

#ifndef ROACH2_SOFTWARE_GPIODEVICE_H
#define ROACH2_SOFTWARE_GPIODEVICE_H

#include <fstream>
#include <string>


class GPIODevice {
public:
    enum IO_MODE {
        INPUT,
        OUTPUT
    };
private:
    std::fstream pinValue;
    IO_MODE mode;
    bool pinValid;
    unsigned int pin;
    std::string pinStr;
public:
    GPIODevice(unsigned int pinNumber, IO_MODE _mode);
    ~GPIODevice();
    bool isValid() { return pinValid; };
    bool read();
    void setMode(IO_MODE _mode);
    void write(bool state);
};


#endif //ROACH2_SOFTWARE_GPIODEVICE_H
