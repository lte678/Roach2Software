cmake_minimum_required(VERSION 3.7)

project(roach2_software)

add_executable(obc_firmware
        Roach2_OBC/main.cpp
        Roach2_OBC/FSM_OBC.cpp
        Roach2_Control/CommandParser.cpp
        Roach2_Control/FSM_Controller.cpp
        Roach2_Control/Sensor_Manager.cpp
        Roach2_Actuators/Actuator.cpp
        Roach2_Actuators/Rover.cpp
        Roach2_Actuators/GoPro.cpp
        Roach2_Sensor/Roach2_Sensor_Sensor.cpp
        Roach2_Sensor/OBC_Systemstatus.cpp
        Roach2_Sensor/RCU_Systemstatus.cpp
        Roach2_Sensor/Temp.cpp
        Roach2_Sensor/Roach2_Sensor_rotEnc.cpp
        Roach2_Sensor/IMU.cpp
        Roach2_Sensor/Roach2_Sensor_adc.cpp
        Roach2_Sensor/ARM_Systeminfo.cpp
        Roach2_Sensor/RocketSignals.cpp
        Roach2_Comm/UART.cpp
        Roach2_Comm/EthernetClient.cpp
        Roach2_Comm/EthernetServer.cpp
        Roach2_Comm/clientsock.cpp
        Roach2_Comm/serversock.cpp
        Roach2_Comm/socket.cpp
        Roach2_DataStore/data.cpp
        Roach2_DataStore/Data_simple.cpp
        Roach2_DataStore/Data_super.cpp
        Roach2_DataStore/Data_Raw.cpp
        Roach2_DataStore/serialization.cpp
        Roach2_DataStore/Data_Raw.cpp
        Roach2_Hardware/I2CDevice.cpp
        Roach2_Hardware/GPIODevice.cpp)

target_link_libraries(obc_firmware pthread)

add_executable(rcu_firmware Roach2_RCU/main.cpp
        Roach2_RCU/FSM_RCU.cpp
        Roach2_Control/FSM_Controller.cpp
        Roach2_Control/Sensor_Manager.cpp
        Roach2_Control/CommandParser.cpp
        Roach2_Actuators/Actuator.cpp
        Roach2_Actuators/PWM.cpp
        Roach2_Actuators/HV.cpp
        Roach2_Comm/UART.cpp
        Roach2_Comm/EthernetClient.cpp
        Roach2_Comm/EthernetServer.cpp
        Roach2_Comm/clientsock.cpp
        Roach2_Comm/serversock.cpp
        Roach2_Comm/socket.cpp
        Roach2_Sensor/Roach2_Sensor_Sensor.cpp
        Roach2_Sensor/OBC_Systemstatus.cpp
        Roach2_Sensor/RCU_Systemstatus.cpp
        Roach2_Sensor/Temp.cpp
        Roach2_Sensor/Roach2_Sensor_rotEnc.cpp
        Roach2_Sensor/IMU.cpp
        Roach2_Sensor/Roach2_Sensor_adc.cpp
        Roach2_Sensor/ARM_Systeminfo.cpp
        Roach2_Sensor/RocketSignals.cpp
        Roach2_DataStore/data.cpp
        Roach2_DataStore/Data_simple.cpp
        Roach2_DataStore/Data_super.cpp
        Roach2_DataStore/Data_Raw.cpp
        Roach2_DataStore/serialization.cpp
        Roach2_Hardware/GPIODevice.cpp
        Roach2_Hardware/I2CDevice.cpp)

target_link_libraries(rcu_firmware pthread)

add_executable(syscheck Roach2_SysCheckUtil/main.cpp
        Roach2_Sensor/Roach2_Sensor_Sensor.cpp
        Roach2_Sensor/OBC_Systemstatus.cpp
        Roach2_Sensor/RCU_Systemstatus.cpp
        Roach2_Sensor/Temp.cpp
        Roach2_Sensor/Roach2_Sensor_rotEnc.cpp
        Roach2_Sensor/IMU.cpp
        Roach2_Sensor/Roach2_Sensor_adc.cpp
        Roach2_Sensor/ARM_Systeminfo.cpp
        Roach2_Sensor/RocketSignals.cpp
        Roach2_DataStore/data.cpp
        Roach2_DataStore/Data_Raw.cpp
        Roach2_DataStore/serialization.cpp
        Roach2_Comm/EthernetClient.cpp
        Roach2_Comm/EthernetServer.cpp
        Roach2_Comm/serversock.cpp
        Roach2_Comm/clientsock.cpp
        Roach2_Comm/socket.cpp)
target_link_libraries(syscheck pthread)

if(LOCAL_DEV)
    add_compile_definitions(LOCAL_DEV)
else()
    target_link_libraries(obc_firmware wiringPi)
    target_link_libraries(rcu_firmware wiringPi)
    target_link_libraries(syscheck wiringPi)
endif()