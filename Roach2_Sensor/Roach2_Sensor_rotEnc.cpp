#include "Roach2_Sensor_rotEnc.h"

ROT_AS5601::ROT_AS5601(float updateFreq) : Sensor(updateFreq)
{
    std::cout << "[Sensor|Rot Enc] Initializing" << std::endl;
    deviceHandle = i2cConnect(AS5601_DEVICE_ID); // Get file/I2C handle
    angle = 0.0f;
}

ROT_AS5601::~ROT_AS5601() = default;

void ROT_AS5601::init()
{
    startAngle = getAngle();
}

float ROT_AS5601::getAngle() {
    uint16_t rawAngle = read16(RAW_REG);
    rawAngle = (rawAngle << 8) | (rawAngle >> 8);

    return (float)rawAngle * PRESCALER;
}

void ROT_AS5601::update()
{
	angle = getAngle() - startAngle;
	angle = angle < 0 ? angle + 360.0f : angle;
}

std::unique_ptr<Data> ROT_AS5601::getData() {
    std::unique_ptr<Data> data_ptr(new Data());
	data_ptr->setId((int)SensorType::ROT_ENC);
	data_ptr->addValue("ROTRAW", angle);
	data_ptr->addValue("ROT", 0.0f);
	return data_ptr;
	/*Effekt der eigenerhitzung wird vernachl�ssigt*/
}

int ROT_AS5601::getI2CAddr() {
	return AS5601_DEVICE_ID;
}

SensorType ROT_AS5601::getSensorType()
{
	return SensorType::ROT_ENC;
}

