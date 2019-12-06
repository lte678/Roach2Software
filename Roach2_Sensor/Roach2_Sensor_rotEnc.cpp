#include "Roach2_Sensor_rotEnc.h"

ROT_AS5601::ROT_AS5601()
{
	this->measurement[0] = 0;
	this->measurement[1] = 0;
}

ROT_AS5601::~ROT_AS5601()
{
}

void ROT_AS5601::init()
{
}

void ROT_AS5601::update()
{
	measurement[0] = ((read8(RAW1_reg)<< 8) + read8(RAW2_reg)); /*masking?*/
	
	measurement[0] = binaryToDecimal(measurement[0]);

	measurement[1] = ((read8(ANG_reg) << 8) + read8(ANG2_reg)); /*masking?*/

	measurement[1] = binaryToDecimal(measurement[1]);
}

Data* ROT_AS5601::getData() {
	Data* data_ptr = new Data();
	data_ptr->setId((int)SENSOR_TYPES::ROT_ENC);
	data_ptr->addValue("ROTRAW", measurement[0]);
	data_ptr->addValue("ROT", measurement[1]);
	return data_ptr;
	/*Effekt der eigenerhitzung wird vernachlässigt*/
}

int ROT_AS5601::getI2CAddr() {
	return AS5601_DEVICE_ID;
}

int ROT_AS5601::getSensorType()
{
	return SENSOR_TYPES::ROT_ENC;
}

