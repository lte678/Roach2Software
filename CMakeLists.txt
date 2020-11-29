cmake_minimum_required(VERSION 3.7)

project(roach2_software)

add_executable(obc_firmware
        OBC/main.cpp
        OBC/FSM_OBC.cpp
        Control/CommandParser.cpp
        Control/FSM_Controller.cpp
        Control/Sensor_Manager.cpp
        Actuators/Actuator.cpp
        Actuators/Rover.cpp
        Actuators/GoPro.cpp
        Sensor/Sensor.cpp
        Sensor/OBC_Systemstatus.cpp
        Sensor/RCU_Systemstatus.cpp
        Sensor/Temp.cpp
        Sensor/LaserDist_VL530LX.cpp
        Sensor/RotaryEncoder.cpp
        Sensor/IMU.cpp
        Sensor/ADC.cpp
        Sensor/ARM_Systeminfo.cpp
        Sensor/RocketSignals.cpp
        Comm/UART.cpp
        Comm/EthernetClient.cpp
        Comm/EthernetServer.cpp
        Comm/clientsock.cpp
        Comm/serversock.cpp
        Comm/socket.cpp
        DataStore/data.cpp
        DataStore/Data_simple.cpp
        DataStore/Data_super.cpp
        DataStore/Data_Raw.cpp
        DataStore/serialization.cpp
        DataStore/Data_Raw.cpp
        Hardware/I2CDevice.cpp
        Hardware/GPIODevice.cpp)

target_link_libraries(obc_firmware pthread atomic)

add_executable(rcu_firmware 
        RCU/main.cpp
        RCU/FSM_RCU.cpp
        Control/FSM_Controller.cpp
        Control/Sensor_Manager.cpp
        Control/CommandParser.cpp
        Actuators/Actuator.cpp
        Actuators/PWM.cpp
        Actuators/HV.cpp
        Comm/UART.cpp
        Comm/EthernetClient.cpp
        Comm/EthernetServer.cpp
        Comm/clientsock.cpp
        Comm/serversock.cpp
        Comm/socket.cpp
        Sensor/Sensor.cpp
        Sensor/OBC_Systemstatus.cpp
        Sensor/RCU_Systemstatus.cpp
        Sensor/Temp.cpp
        Sensor/LaserDist_VL530LX.cpp
        Sensor/RotaryEncoder.cpp
        Sensor/IMU.cpp
        Sensor/ADC.cpp
        Sensor/ARM_Systeminfo.cpp
        Sensor/RocketSignals.cpp
        DataStore/data.cpp
        DataStore/Data_simple.cpp
        DataStore/Data_super.cpp
        DataStore/Data_Raw.cpp
        DataStore/serialization.cpp
        Hardware/GPIODevice.cpp
        Hardware/I2CDevice.cpp)

target_link_libraries(rcu_firmware pthread atomic)

add_executable(syscheck
        SysCheckUtil/main.cpp
        Sensor/Sensor.cpp
        Sensor/OBC_Systemstatus.cpp
        Sensor/RCU_Systemstatus.cpp
        Sensor/Temp.cpp
        Sensor/RotaryEncoder.cpp
        Sensor/LaserDist_VL530LX.cpp
        Sensor/IMU.cpp
        Sensor/ADC.cpp
        Sensor/ARM_Systeminfo.cpp
        Sensor/RocketSignals.cpp
        Actuators/PWM.cpp
        Actuators/Actuator.cpp
        Actuators/Rover.cpp
        Hardware/GPIODevice.cpp
        Hardware/I2CDevice.cpp
        DataStore/data.cpp
        DataStore/Data_Raw.cpp
        DataStore/serialization.cpp
        Control/FSM_Controller.cpp
        Control/Sensor_Manager.cpp
        Comm/UART.cpp
        Comm/EthernetClient.cpp
        Comm/EthernetServer.cpp
        Comm/serversock.cpp
        Comm/clientsock.cpp
        Comm/socket.cpp)
target_link_libraries(syscheck pthread atomic)

if(LOCAL_DEV)
    add_definitions(-DLOCAL_DEV)
else()
    target_link_libraries(obc_firmware wiringPi)
    target_link_libraries(rcu_firmware wiringPi)
    target_link_libraries(syscheck wiringPi)
endif()