//
// Created by leon on 14.01.20.
//

#include "../Roach2_Control/Defines.h"
#include "../Roach2_DataStore/data.h"

// Sensor includes
#include "../Roach2_Sensor/Roach2_Sensor_Sensor.h"
#include "../Roach2_Sensor/ARM_Systeminfo.h"
#include "../Roach2_Sensor/IMU.h"
#include "../Roach2_Sensor/Temp.h"
#include "../Roach2_Sensor/RocketSignals.h"

#include <iostream>
#include <cstring>

using namespace std;

void printSensorData(Sensor *sensor) {
    sensor->init();
    sensor->update();
    cout << "--- dump start ---" << endl;
    sensor->getData()->print();
    cout << "--- dump end ---" << endl;
    cout << endl;
}

int main(int argc, char* argv[]) {
    PLATFORM platform = PLATFORM::UNKNOWN;

    if(argc == 2) {
        if(strcmp(argv[1], "obc") == 0) {
            platform = PLATFORM::OBC;
        } else if(strcmp(argv[1], "rcu") == 0) {
            platform = PLATFORM::RCU;
        } else if(strcmp(argv[1], "rover") == 0) {
            platform = PLATFORM::RCU;
        } else {
            cout << "Unknown target " << argv[1] << endl;
            return 1;
        }
    } else {
        cout << "Please specify a target!" << endl;
        return 1;
    }

    // Start analysis
    if(platform == PLATFORM::OBC) {
        cout << "Running hardware tests for OBC" << endl << endl;

        cout << "[TEST] ARM Systeminfo..." << endl;
        ARM_Systeminfo sensor1(10.0f);
        printSensorData(&sensor1);

        cout << "[TEST] BNO055 - IMU..." << endl;
        BNO055_IMU sensor2(10.0f);
        printSensorData(&sensor2);

        cout << "[TEST] LM75B - Temp sensor..." << endl;
        TEMP_LM75B sensor3(10.0f);
        printSensorData(&sensor3);

        cout << "[TEST] REXUS signals..." << endl;
        RocketSignals sensor4(10.0f);
        printSensorData(&sensor4);
    } else {
        cout << "Running hardware tests for RCU" << endl << endl;

        cout << "[TEST] ARM Systeminfo..." << endl;
        ARM_Systeminfo sensor1(10.0f);
        printSensorData(&sensor1);

        cout << "[TEST] BNO055 - IMU..." << endl;
        BNO055_IMU sensor2(10.0f);
        printSensorData(&sensor2);
    }

    cout << "Tests executed successfully" << endl;
    cout << "Validate results manually!" << endl;

    return 0;
}