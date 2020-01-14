//
// Created by leon on 13.01.20.
//

#include "GPIODevice.h"

#ifndef LOCAL_DEV
GPIODevice::GPIODevice(unsigned int pinNumber, GPIODevice::IO_MODE _mode) {
    mode = _mode;
    pinStr = std::to_string(pinNumber);
    pin = pinNumber;

    pinValid = false;

    // Export GPIO pin
    std::fstream export_file("/sys/class/gpio/export", std::fstream::out);
    if (export_file) {
        export_file << pinStr;
        export_file.close();

        // Control direction
        std::fstream dir_file("/sys/class/gpio/gpio" + pinStr + "/direction", std::fstream::out);
        if (!dir_file) {
            return;
        }
        if(mode == IO_MODE::OUTPUT) {
            dir_file << "out";
        } else {
            dir_file << "in";
        }
        dir_file.close();

        // Open the device
        if(mode == IO_MODE::OUTPUT) {
            pinValue.open("/sys/class/gpio/gpio" + pinStr + "/value", std::fstream::out);
        } else {
            pinValue.open("/sys/class/gpio/gpio" + pinStr + "/value", std::fstream::in);
        }
        if(pinValue) {
            // We made it this far, the pin is finally valid!
            pinValid = true;
        }

    }
}

GPIODevice::~GPIODevice() {
    // Unexport GPIO pins
    std::fstream unexport_file("/sys/class/gpio/unexport", std::fstream::out);
    if (unexport_file) {
        unexport_file << pinStr; // GPIO HV left RCU pin
        unexport_file.close();
    }
}

#else
// Dummy interfaces

GPIODevice::GPIODevice(unsigned int pinNumber, IO_MODE _mode) {
    pinValid = false;
    mode = _mode;
    pinNumber = pinNumber;
}

GPIODevice::~GPIODevice() {

}
#endif

bool GPIODevice::read() {
    if(mode == IO_MODE::INPUT && pinValid) {
        pinValue.seekg(0, std::fstream::beg);
        char state;
        pinValue.read(&state, 1);
        return state == '1';
    } else {
        return false;
    }
}

void GPIODevice::write(bool state) {
    if(mode == IO_MODE::OUTPUT && pinValid) {
        pinValue.seekg(0, std::fstream::beg);
        if(state) {
            pinValue << '1';
        } else {
            pinValue << '0';
        }
    }
}

void GPIODevice::setMode(IO_MODE _mode) {
    if(pinValid) {
        // Control direction
        std::fstream dir_file("/sys/class/gpio/gpio" + pinStr + "/direction", std::fstream::out);
        if (!dir_file) {
            return;
        }
        mode = _mode;
        if(mode == IO_MODE::OUTPUT) {
            dir_file << "out";
        } else {
            dir_file << "in";
        }
        dir_file.close();
    }
}