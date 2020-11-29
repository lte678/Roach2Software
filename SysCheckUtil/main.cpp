//
// Created by leon on 14.01.20.
//

#include "../Control/Defines.h"
#include "../DataStore/data.h"

// Sensor includes
#include "../Sensor/Sensor.h"
#include "../Sensor/ARM_Systeminfo.h"
#include "../Sensor/IMU.h"
#include "../Sensor/Temp.h"
#include "../Sensor/ADC.h"
#include "../Sensor/RocketSignals.h"
#include "../Sensor/RotaryEncoder.h"
#include "../Sensor/LaserDist_VL530LX.h"
#include "../Actuators/PWM.h"
#include "../Actuators/Rover.h"

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

        cout << "[TEST] MCP3428 - ADC..." << endl;
        ADC_MCP3428 sensor5(10.0f);
        printSensorData(&sensor5);

        cout << "[TEST] Power cycling rover..." << endl;
        Actuator_Rover actuator1;
        actuator1.enable(true);
        usleep(1000 * 1000 * 2); // power for 5 seconds
        actuator1.disable(true);
        cout << "Done!" << endl << endl;
    } else {
        cout << "Running hardware tests for RCU" << endl << endl;

        cout << "[TEST] ARM Systeminfo..." << endl;
        ARM_Systeminfo sensor1(10.0f);
        printSensorData(&sensor1);

        cout << "[TEST] BNO055 - IMU..." << endl;
        BNO055_IMU sensor2(10.0f);
        printSensorData(&sensor2);

        cout << "[TEST] MCP3428 - ADC..." << endl;
        ADC_MCP3428 sensor3(10.0f);
        printSensorData(&sensor3);

        cout << "[TEST] AS5601 - Rotary Encoder..." << endl;
        ROT_AS5601 sensor4(10.0f);
        printSensorData(&sensor4);

        cout << "[TEST] STVL53L0X - Laser Dist..." << endl;
        LaserDist_VL530LX sensor5(10.0f, SensorType::LASERDIST_0, 0x29);
        printSensorData(&sensor5);


        cout << "[TEST] Starting motor..." << endl;
        PWM_PCA985 actuator2;
        actuator2.init();
        actuator2.drive();
        //usleep(1000 * 1000 * 2); // Run for 2 seconds
        actuator2.stop();
        cout << "Done!" << endl << endl;
    }

    cout << "Tests executed successfully" << endl;
    cout << "Validate results manually!" << endl;

    return 0;
}