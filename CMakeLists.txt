cmake_minimum_required(VERSION 3.7)

project(roach2_software)

add_executable(obc_firmware
        Roach2_OBC/main.cpp
        Roach2_OBC/FSM_OBC.cpp
        Roach2_Control/CommandParser.cpp
        Roach2_Control/FSM_Controller.cpp
        Roach2_Control/Sensor_Manager.cpp
        Roach2_Actuators/Actuator.cpp
        Roach2_Actuators/Roach2_Actuators_Actuator_Rover.cpp
        Roach2_Actuators/Roach2_Actuators_Actuator_GoPro.cpp
        Roach2_Sensor/Roach2_Sensor_Sensor.cpp
        Roach2_Sensor/OBC_Systemstatus.cpp
        Roach2_Sensor/RCU_Systemstatus.cpp
        Roach2_Sensor/temp.cpp
        Roach2_Sensor/Roach2_Sensor_rotEnc.cpp
        Roach2_Sensor/IMU.cpp
        Roach2_Sensor/Roach2_Sensor_adc.cpp
        Roach2_Sensor/ARM_Systeminfo.cpp
        Roach2_Comm/UART.cpp
        Roach2_Comm/RocketSignals.cpp
        Roach2_Comm/EthernetClient.cpp
        Roach2_Comm/EthernetServer.cpp
        Roach2_Comm/clientsock.cpp
        Roach2_Comm/serversock.cpp
        Roach2_Comm/socket.cpp
        Roach2_DataStore/data.cpp
        Roach2_DataStore/Data_simple.cpp
        Roach2_DataStore/serialization.cpp)

target_link_libraries(obc_firmware wiringPi pthread)
include_directories(src)