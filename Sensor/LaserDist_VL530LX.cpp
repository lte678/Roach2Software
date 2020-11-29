#include "LaserDist_VL530LX.h"
#include "wiringPi.h"


// Record the current time to check an upcoming timeout against
#define startTimeout() (timeout_start_ms = millis())
#define checkTimeoutExpired() (io_timeout > 0 && ((uint16_t)(millis() - timeout_start_ms) > io_timeout))
// Decode VCSEL (vertical cavity surface emitting laser) pulse period in PCLKs
// from register value
// based on VL53L0X_decode_vcsel_period()
#define decodeVcselPeriod(reg_val)      (((reg_val) + 1) << 1)

// Encode VCSEL pulse period register value from period in PCLKs
// based on VL53L0X_encode_vcsel_period()
#define encodeVcselPeriod(period_pclks) (((period_pclks) >> 1) - 1)

// Calculate macro period in *nanoseconds* from VCSEL period in PCLKs
// based on VL53L0X_calc_macro_period_ps()
// PLL_period_ps = 1655; macro_period_vclks = 2304
#define calcMacroPeriod(vcsel_period_pclks) ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)

LaserDist_VL530LX::LaserDist_VL530LX(float updateFreq, SensorType _sensor_type, int i2c_address) : Sensor(updateFreq), sensorType(_sensor_type)
{
    std::cout << "[Sensor|Laser Distance] Initializing" << std::endl;
    i2cAddress = i2c_address;
    deviceHandle = i2cConnect(i2cAddress);
    io_timeout = 0;
    did_timeout = false;
    distance = 0.0f;
}

void LaserDist_VL530LX::init() {

    if(initLaserSensor()){
        std::cout << "[Sensor|Laser Distance] id: " << (int)sensorType << ", address: " << i2cAddress << ", deviceHandle: " << deviceHandle <<std::endl;
    }else{
        std::cout << "[Sensor|Laser Distance] Initialization failed" << std::endl;
    }
    //optimize sensor settings here
    //setMeasurementTimingBudget(200000);
}

void LaserDist_VL530LX::update() {
    distance = readRangeSingleMillimeters();
}

std::unique_ptr<Data> LaserDist_VL530LX::getData()
{
    std::unique_ptr<Data> data_ptr(new Data());
    data_ptr->setId((int)sensorType);
    data_ptr->addValue("DIST", distance);

    return data_ptr;
}

int LaserDist_VL530LX::getI2CAddr()
{
	return i2cAddress;
}

SensorType LaserDist_VL530LX::getSensorType()
{
	return sensorType;
}

uint16_t LaserDist_VL530LX::readRangeSingleMillimeters()
{
    write8(0x80, 0x01);
    write8(0xFF, 0x01);
    write8(0x00, 0x00);
    write8(0x91, stop_variable);
    write8(0x00, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x00);
    write8(SYSRANGE_START, 0x01);

    // "Wait until start bit has been cleared"
    startTimeout();
    while (read8(SYSRANGE_START) & 0x01)
    {
        if (checkTimeoutExpired())
        {
            std::cout << "Timeout - Initialize" << std::endl;
            did_timeout = true;
            return 65535;
        }
    }
    return readRangeContinuousMillimeters();
}

// Returns a range reading in millimeters when continuous mode is active
// (readRangeSingleMillimeters() also calls this function after starting a
// single-shot range measurement)
uint16_t LaserDist_VL530LX::readRangeContinuousMillimeters()
{
    startTimeout();
    while ((read8(RESULT_INTERRUPT_STATUS) & 0x07) == 0)
    {

        if (checkTimeoutExpired())
        {
            std::cout << "Timeout - ReadMillimeters" << std::endl;
            did_timeout = true;
            return 65535;
        }
    }
    // assumptions: Linearity Corrective Gain is 1000 (default);
    // fractional ranging is not enabled
    uint16_t range = read16(RESULT_RANGE_STATUS + 10);

    write8(SYSTEM_INTERRUPT_CLEAR, 0x01);

    return range;
}

// based on VL53L0X_perform_single_ref_calibration()
bool LaserDist_VL530LX::performSingleRefCalibration(uint8_t vhv_init_byte)
{
    write8(SYSRANGE_START, 0x01 | vhv_init_byte); // VL53L0X_REG_SYSRANGE_MODE_START_STOP

    startTimeout();
    while ((read8(RESULT_INTERRUPT_STATUS) & 0x07) == 0)
    {
        if (checkTimeoutExpired()) { return false; }
    }

    write8(SYSTEM_INTERRUPT_CLEAR, 0x01);

    write8(SYSRANGE_START, 0x00);

    return true;
}

// Set the measurement timing budget in microseconds, which is the time allowed
// for one measurement; the ST API and this library take care of splitting the
// timing budget among the sub-steps in the ranging sequence. A longer timing
// budget allows for more accurate measurements. Increasing the budget by a
// factor of N decreases the range measurement standard deviation by a factor of
// sqrt(N). Defaults to about 33 milliseconds; the minimum is 20 ms.
// based on VL53L0X_set_measurement_timing_budget_micro_seconds()
bool LaserDist_VL530LX::setMeasurementTimingBudget(uint32_t budget_us)
{
    SequenceStepEnables enables;
    SequenceStepTimeouts timeouts;

    uint16_t const StartOverhead     = 1910;
    uint16_t const EndOverhead        = 960;
    uint16_t const MsrcOverhead       = 660;
    uint16_t const TccOverhead        = 590;
    uint16_t const DssOverhead        = 690;
    uint16_t const PreRangeOverhead   = 660;
    uint16_t const FinalRangeOverhead = 550;

    uint32_t const MinTimingBudget = 20000;

    if (budget_us < MinTimingBudget) { return false; }

    uint32_t used_budget_us = StartOverhead + EndOverhead;

    getSequenceStepEnables(&enables);
    getSequenceStepTimeouts(&enables, &timeouts);

    if (enables.tcc)
    {
        used_budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
    }

    if (enables.dss)
    {
        used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
    }
    else if (enables.msrc)
    {
        used_budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
    }

    if (enables.pre_range)
    {
        used_budget_us += (timeouts.pre_range_us + PreRangeOverhead);
    }

    if (enables.final_range)
    {
        used_budget_us += FinalRangeOverhead;

        // "Note that the final range timeout is determined by the timing
        // budget and the sum of all other timeouts within the sequence.
        // If there is no room for the final range timeout, then an error
        // will be set. Otherwise the remaining time will be applied to
        // the final range."

        if (used_budget_us > budget_us)
        {
            // "Requested timeout too big."
            return false;
        }

        uint32_t final_range_timeout_us = budget_us - used_budget_us;

        // set_sequence_step_timeout() begin
        // (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

        // "For the final range timeout, the pre-range timeout
        //  must be added. To do this both final and pre-range
        //  timeouts must be expressed in macro periods MClks
        //  because they have different vcsel periods."

        uint32_t final_range_timeout_mclks =
                timeoutMicrosecondsToMclks(final_range_timeout_us,
                                           timeouts.final_range_vcsel_period_pclks);

        if (enables.pre_range)
        {
            final_range_timeout_mclks += timeouts.pre_range_mclks;
        }

        write16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                      encodeTimeout(final_range_timeout_mclks));

        // set_sequence_step_timeout() end

        measurement_timing_budget_us = budget_us; // store for internal reuse
    }
    return true;
}

// Get the measurement timing budget in microseconds
// based on VL53L0X_get_measurement_timing_budget_micro_seconds()
// in us
uint32_t LaserDist_VL530LX::getMeasurementTimingBudget()
{
    SequenceStepEnables enables;
    SequenceStepTimeouts timeouts;

    uint16_t const StartOverhead     = 1910;
    uint16_t const EndOverhead        = 960;
    uint16_t const MsrcOverhead       = 660;
    uint16_t const TccOverhead        = 590;
    uint16_t const DssOverhead        = 690;
    uint16_t const PreRangeOverhead   = 660;
    uint16_t const FinalRangeOverhead = 550;

    // "Start and end overhead times always present"
    uint32_t budget_us = StartOverhead + EndOverhead;

    getSequenceStepEnables(&enables);
    getSequenceStepTimeouts(&enables, &timeouts);

    if (enables.tcc)
    {
        budget_us += (timeouts.msrc_dss_tcc_us + TccOverhead);
    }

    if (enables.dss)
    {
        budget_us += 2 * (timeouts.msrc_dss_tcc_us + DssOverhead);
    }
    else if (enables.msrc)
    {
        budget_us += (timeouts.msrc_dss_tcc_us + MsrcOverhead);
    }

    if (enables.pre_range)
    {
        budget_us += (timeouts.pre_range_us + PreRangeOverhead);
    }

    if (enables.final_range)
    {
        budget_us += (timeouts.final_range_us + FinalRangeOverhead);
    }

    measurement_timing_budget_us = budget_us; // store for internal reuse
    return budget_us;
}

// Get sequence step enables
// based on VL53L0X_GetSequenceStepEnables()
void LaserDist_VL530LX::getSequenceStepEnables(SequenceStepEnables * enables)
{
    uint8_t sequence_config = read8(SYSTEM_SEQUENCE_CONFIG);

    enables->tcc          = (sequence_config >> 4) & 0x1;
    enables->dss          = (sequence_config >> 3) & 0x1;
    enables->msrc         = (sequence_config >> 2) & 0x1;
    enables->pre_range    = (sequence_config >> 6) & 0x1;
    enables->final_range  = (sequence_config >> 7) & 0x1;
}

// Get sequence step timeouts
// based on get_sequence_step_timeout(),
// but gets all timeouts instead of just the requested one, and also stores
// intermediate values
void LaserDist_VL530LX::getSequenceStepTimeouts(SequenceStepEnables const * enables, SequenceStepTimeouts * timeouts)
{
    timeouts->pre_range_vcsel_period_pclks = getVcselPulsePeriod(VcselPeriodPreRange);

    timeouts->msrc_dss_tcc_mclks = read8(MSRC_CONFIG_TIMEOUT_MACROP) + 1;
    timeouts->msrc_dss_tcc_us =
            timeoutMclksToMicroseconds(timeouts->msrc_dss_tcc_mclks,
                                       timeouts->pre_range_vcsel_period_pclks);

    timeouts->pre_range_mclks =
            decodeTimeout(read16(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI));
    timeouts->pre_range_us =
            timeoutMclksToMicroseconds(timeouts->pre_range_mclks,
                                       timeouts->pre_range_vcsel_period_pclks);

    timeouts->final_range_vcsel_period_pclks = getVcselPulsePeriod(VcselPeriodFinalRange);

    timeouts->final_range_mclks =
            decodeTimeout(read16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI));

    if (enables->pre_range)
    {
        timeouts->final_range_mclks -= timeouts->pre_range_mclks;
    }

    timeouts->final_range_us =
            timeoutMclksToMicroseconds(timeouts->final_range_mclks,
                                       timeouts->final_range_vcsel_period_pclks);
}

// Decode sequence step timeout in MCLKs from register value
// based on VL53L0X_decode_timeout()
// Note: the original function returned a uint32_t, but the return value is
// always stored in a uint16_t.
uint16_t LaserDist_VL530LX::decodeTimeout(uint16_t reg_val)
{
    // format: "(LSByte * 2^MSByte) + 1"
    return (uint16_t)((reg_val & 0x00FF) <<
                                         (uint16_t)((reg_val & 0xFF00) >> 8)) + 1;
}

// Encode sequence step timeout register value from timeout in MCLKs
// based on VL53L0X_encode_timeout()
uint16_t LaserDist_VL530LX::encodeTimeout(uint32_t timeout_mclks)
{
    // format: "(LSByte * 2^MSByte) + 1"

    uint32_t ls_byte = 0;
    uint16_t ms_byte = 0;

    if (timeout_mclks > 0)
    {
        ls_byte = timeout_mclks - 1;

        while ((ls_byte & 0xFFFFFF00) > 0)
        {
            ls_byte >>= 1;
            ms_byte++;
        }

        return (ms_byte << 8) | (ls_byte & 0xFF);
    }
    else { return 0; }
}

// Convert sequence step timeout from MCLKs to microseconds with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_us()
uint32_t LaserDist_VL530LX::timeoutMclksToMicroseconds(uint16_t timeout_period_mclks, uint8_t vcsel_period_pclks)
{
    uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

    return ((timeout_period_mclks * macro_period_ns) + 500) / 1000;
}

// Convert sequence step timeout from microseconds to MCLKs with given VCSEL period in PCLKs
// based on VL53L0X_calc_timeout_mclks()
uint32_t LaserDist_VL530LX::timeoutMicrosecondsToMclks(uint32_t timeout_period_us, uint8_t vcsel_period_pclks)
{
    uint32_t macro_period_ns = calcMacroPeriod(vcsel_period_pclks);

    return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}




// Set the VCSEL (vertical cavity surface emitting laser) pulse period for the
// given period type (pre-range or final range) to the given value in PCLKs.
// Longer periods seem to increase the potential range of the sensor.
// Valid values are (even numbers only):
//  pre:  12 to 18 (initialized default: 14)
//  final: 8 to 14 (initialized default: 10)
// based on VL53L0X_set_vcsel_pulse_period()
bool LaserDist_VL530LX::setVcselPulsePeriod(vcselPeriodType type, uint8_t period_pclks)
{
    uint8_t vcsel_period_reg = encodeVcselPeriod(period_pclks);

    SequenceStepEnables enables;
    SequenceStepTimeouts timeouts;

    getSequenceStepEnables(&enables);
    getSequenceStepTimeouts(&enables, &timeouts);

    // "Apply specific settings for the requested clock period"
    // "Re-calculate and apply timeouts, in macro periods"

    // "When the VCSEL period for the pre or final range is changed,
    // the corresponding timeout must be read from the device using
    // the current VCSEL period, then the new VCSEL period can be
    // applied. The timeout then must be written back to the device
    // using the new VCSEL period.
    //
    // For the MSRC timeout, the same applies - this timeout being
    // dependant on the pre-range vcsel period."


    if (type == VcselPeriodPreRange)
    {
        // "Set phase check limits"
        switch (period_pclks)
        {
            case 12:
                write8(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x18);
                break;

            case 14:
                write8(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x30);
                break;

            case 16:
                write8(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x40);
                break;

            case 18:
                write8(PRE_RANGE_CONFIG_VALID_PHASE_HIGH, 0x50);
                break;

            default:
                // invalid period
                return false;
        }
        write8(PRE_RANGE_CONFIG_VALID_PHASE_LOW, 0x08);

        // apply new VCSEL period
        write8(PRE_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

        // update timeouts

        // set_sequence_step_timeout() begin
        // (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE)

        uint16_t new_pre_range_timeout_mclks =
                timeoutMicrosecondsToMclks(timeouts.pre_range_us, period_pclks);

        write16(PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                      encodeTimeout(new_pre_range_timeout_mclks));

        // set_sequence_step_timeout() end

        // set_sequence_step_timeout() begin
        // (SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)

        uint16_t new_msrc_timeout_mclks =
                timeoutMicrosecondsToMclks(timeouts.msrc_dss_tcc_us, period_pclks);

        write8(MSRC_CONFIG_TIMEOUT_MACROP,
                 (new_msrc_timeout_mclks > 256) ? 255 : (new_msrc_timeout_mclks - 1));

        // set_sequence_step_timeout() end
    }
    else if (type == VcselPeriodFinalRange)
    {
        switch (period_pclks)
        {
            case 8:
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x10);
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
                write8(GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
                write8(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);
                write8(0xFF, 0x01);
                write8(ALGO_PHASECAL_LIM, 0x30);
                write8(0xFF, 0x00);
                break;

            case 10:
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x28);
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
                write8(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
                write8(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);
                write8(0xFF, 0x01);
                write8(ALGO_PHASECAL_LIM, 0x20);
                write8(0xFF, 0x00);
                break;

            case 12:
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x38);
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
                write8(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
                write8(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);
                write8(0xFF, 0x01);
                write8(ALGO_PHASECAL_LIM, 0x20);
                write8(0xFF, 0x00);
                break;

            case 14:
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_HIGH, 0x48);
                write8(FINAL_RANGE_CONFIG_VALID_PHASE_LOW,  0x08);
                write8(GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
                write8(ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);
                write8(0xFF, 0x01);
                write8(ALGO_PHASECAL_LIM, 0x20);
                write8(0xFF, 0x00);
                break;

            default:
                // invalid period
                return false;
        }

        // apply new VCSEL period
        write8(FINAL_RANGE_CONFIG_VCSEL_PERIOD, vcsel_period_reg);

        // update timeouts

        // set_sequence_step_timeout() begin
        // (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE)

        // "For the final range timeout, the pre-range timeout
        //  must be added. To do this both final and pre-range
        //  timeouts must be expressed in macro periods MClks
        //  because they have different vcsel periods."

        uint16_t new_final_range_timeout_mclks =
                timeoutMicrosecondsToMclks(timeouts.final_range_us, period_pclks);

        if (enables.pre_range)
        {
            new_final_range_timeout_mclks += timeouts.pre_range_mclks;
        }

        write16(FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
                      encodeTimeout(new_final_range_timeout_mclks));

        // set_sequence_step_timeout end
    }
    else
    {
        // invalid type
        return false;
    }

    // "Finally, the timing budget must be re-applied"

    setMeasurementTimingBudget(measurement_timing_budget_us);

    // "Perform the phase calibration. This is needed after changing on vcsel period."
    // VL53L0X_perform_phase_calibration() begin

    uint8_t sequence_config = read8(SYSTEM_SEQUENCE_CONFIG);
    write8(SYSTEM_SEQUENCE_CONFIG, 0x02);
    performSingleRefCalibration(0x0);
    write8(SYSTEM_SEQUENCE_CONFIG, sequence_config);

    // VL53L0X_perform_phase_calibration() end

    return true;
}

// Get the VCSEL pulse period in PCLKs for the given period type.
// based on VL53L0X_get_vcsel_pulse_period()
uint8_t LaserDist_VL530LX::getVcselPulsePeriod(vcselPeriodType type)
{
    if (type == VcselPeriodPreRange)
    {
        return decodeVcselPeriod(read8(PRE_RANGE_CONFIG_VCSEL_PERIOD));
    }
    else if (type == VcselPeriodFinalRange)
    {
        return decodeVcselPeriod(read8(FINAL_RANGE_CONFIG_VCSEL_PERIOD));
    }
    else { return 255; }
}

// Set the return signal rate limit check value in units of MCPS (mega counts
// per second). "This represents the amplitude of the signal reflected from the
// target and detected by the device"; setting this limit presumably determines
// the minimum measurement necessary for the sensor to report a valid reading.
// Setting a lower limit increases the potential range of the sensor but also
// seems to increase the likelihood of getting an inaccurate reading because of
// unwanted reflections from objects other than the intended target.
// Defaults to 0.25 MCPS as initialized by the ST API and this library.
bool LaserDist_VL530LX::setSignalRateLimit(float limit_Mcps)
{
    if (limit_Mcps < 0 || limit_Mcps > 511.99) { return false; }

    // Q9.7 fixed point format (9 integer bits, 7 fractional bits)
    write16(FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, limit_Mcps * (1 << 7));
    return true;
}

// Write an arbitrary number of bytes from the given array to the sensor,
// starting at the given register
void LaserDist_VL530LX::writeMulti(uint8_t reg, uint8_t const * src, uint8_t count)
{
    while (count-- > 0)
    {
        write16(reg,*(src++));
    }
}

// Read an arbitrary number of bytes from the sensor, starting at the given
// register, into the given array
void LaserDist_VL530LX::readMulti(uint8_t reg, uint8_t * dst, uint8_t count)
{

    /**
    bus->beginTransmission(address);
    bus->write(reg);
    last_status = bus->endTransmission();
    bus->requestFrom(address, count);
     **/
     write8(reg, count);

    while (count-- > 0)
    {
        *(dst++) = simpleRead();
    }
}

// Get reference SPAD (single photon avalanche diode) count and type
// based on VL53L0X_get_info_from_device(),
// but only gets reference SPAD count and type
bool LaserDist_VL530LX::getSpadInfo(uint8_t * count, bool * type_is_aperture)
{
    uint8_t tmp;

    write8(0x80, 0x01);
    write8(0xFF, 0x01);
    write8(0x00, 0x00);

    write8(0xFF, 0x06);
    write8(0x83, read8(0x83) | 0x04);
    write8(0xFF, 0x07);
    write8(0x81, 0x01);

    write8(0x80, 0x01);

    write8(0x94, 0x6b);
    write8(0x83, 0x00);
    startTimeout();
    while (read8(0x83) == 0x00)
    {
        if (checkTimeoutExpired()) { return false; }
    }
    write8(0x83, 0x01);
    tmp = read8(0x92);

    *count = tmp & 0x7f;
    *type_is_aperture = (tmp >> 7) & 0x01;

    write8(0x81, 0x00);
    write8(0xFF, 0x06);
    write8(0x83, read8(0x83)  & ~0x04);
    write8(0xFF, 0x01);
    write8(0x00, 0x01);

    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    return true;
}

bool LaserDist_VL530LX::initLaserSensor(bool io_2v8) {
    // check model ID register (value specified in datasheet)
    if (read8(IDENTIFICATION_MODEL_ID) != 0xEE) { return false; }

    // VL53L0X_DataInit() begin

    // sensor uses 1V8 mode for I/O by default; switch to 2V8 mode if necessary
    if (io_2v8)
    {
        write8(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV,
                 read8(VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV) | 0x01); // set bit 0
    }

    // "Set I2C standard mode"
    write8(0x88, 0x00);

    write8(0x80, 0x01);
    write8(0xFF, 0x01);
    write8(0x00, 0x00);
    stop_variable = read8(0x91);
    write8(0x00, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    // disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
    write8(MSRC_CONFIG_CONTROL, read8(MSRC_CONFIG_CONTROL) | 0x12);

    // set final range signal rate limit to 0.25 MCPS (million counts per second)
    setSignalRateLimit(0.25);

    write8(SYSTEM_SEQUENCE_CONFIG, 0xFF);

    // VL53L0X_DataInit() end

    // VL53L0X_StaticInit() begin

    uint8_t spad_count;
    bool spad_type_is_aperture;
    if (!getSpadInfo(&spad_count, &spad_type_is_aperture)) { return false; }

    // The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
    // the API, but the same data seems to be more easily readable from
    // GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
    uint8_t ref_spad_map[6];
    readMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

    // -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)

    write8(0xFF, 0x01);
    write8(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
    write8(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
    write8(0xFF, 0x00);
    write8(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

    uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
    uint8_t spads_enabled = 0;

    for (uint8_t i = 0; i < 48; i++)
    {
        if (i < first_spad_to_enable || spads_enabled == spad_count)
        {
            // This bit is lower than the first one that should be enabled, or
            // (reference_spad_count) bits have already been enabled, so zero this bit
            ref_spad_map[i / 8] &= ~(1 << (i % 8));
        }
        else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
        {
            spads_enabled++;
        }
    }

    writeMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

    // -- VL53L0X_set_reference_spads() end

    // -- VL53L0X_load_tuning_settings() begin
    // DefaultTuningSettings from vl53l0x_tuning.h

    write8(0xFF, 0x01);
    write8(0x00, 0x00);

    write8(0xFF, 0x00);
    write8(0x09, 0x00);
    write8(0x10, 0x00);
    write8(0x11, 0x00);

    write8(0x24, 0x01);
    write8(0x25, 0xFF);
    write8(0x75, 0x00);

    write8(0xFF, 0x01);
    write8(0x4E, 0x2C);
    write8(0x48, 0x00);
    write8(0x30, 0x20);

    write8(0xFF, 0x00);
    write8(0x30, 0x09);
    write8(0x54, 0x00);
    write8(0x31, 0x04);
    write8(0x32, 0x03);
    write8(0x40, 0x83);
    write8(0x46, 0x25);
    write8(0x60, 0x00);
    write8(0x27, 0x00);
    write8(0x50, 0x06);
    write8(0x51, 0x00);
    write8(0x52, 0x96);
    write8(0x56, 0x08);
    write8(0x57, 0x30);
    write8(0x61, 0x00);
    write8(0x62, 0x00);
    write8(0x64, 0x00);
    write8(0x65, 0x00);
    write8(0x66, 0xA0);

    write8(0xFF, 0x01);
    write8(0x22, 0x32);
    write8(0x47, 0x14);
    write8(0x49, 0xFF);
    write8(0x4A, 0x00);

    write8(0xFF, 0x00);
    write8(0x7A, 0x0A);
    write8(0x7B, 0x00);
    write8(0x78, 0x21);

    write8(0xFF, 0x01);
    write8(0x23, 0x34);
    write8(0x42, 0x00);
    write8(0x44, 0xFF);
    write8(0x45, 0x26);
    write8(0x46, 0x05);
    write8(0x40, 0x40);
    write8(0x0E, 0x06);
    write8(0x20, 0x1A);
    write8(0x43, 0x40);

    write8(0xFF, 0x00);
    write8(0x34, 0x03);
    write8(0x35, 0x44);

    write8(0xFF, 0x01);
    write8(0x31, 0x04);
    write8(0x4B, 0x09);
    write8(0x4C, 0x05);
    write8(0x4D, 0x04);

    write8(0xFF, 0x00);
    write8(0x44, 0x00);
    write8(0x45, 0x20);
    write8(0x47, 0x08);
    write8(0x48, 0x28);
    write8(0x67, 0x00);
    write8(0x70, 0x04);
    write8(0x71, 0x01);
    write8(0x72, 0xFE);
    write8(0x76, 0x00);
    write8(0x77, 0x00);

    write8(0xFF, 0x01);
    write8(0x0D, 0x01);

    write8(0xFF, 0x00);
    write8(0x80, 0x01);
    write8(0x01, 0xF8);

    write8(0xFF, 0x01);
    write8(0x8E, 0x01);
    write8(0x00, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    // -- VL53L0X_load_tuning_settings() end

    // "Set interrupt config to new sample ready"
    // -- VL53L0X_SetGpioConfig() begin

    write8(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
    write8(GPIO_HV_MUX_ACTIVE_HIGH, read8(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
    write8(SYSTEM_INTERRUPT_CLEAR, 0x01);

    // -- VL53L0X_SetGpioConfig() end

    measurement_timing_budget_us = getMeasurementTimingBudget();

    // "Disable MSRC and TCC by default"
    // MSRC = Minimum Signal Rate Check
    // TCC = Target CentreCheck
    // -- VL53L0X_SetSequenceStepEnable() begin

    write8(SYSTEM_SEQUENCE_CONFIG, 0xE8);

    // -- VL53L0X_SetSequenceStepEnable() end

    // "Recalculate timing budget"
    setMeasurementTimingBudget(measurement_timing_budget_us);

    // VL53L0X_StaticInit() end

    // VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())

    // -- VL53L0X_perform_vhv_calibration() begin

    write8(SYSTEM_SEQUENCE_CONFIG, 0x01);
    if (!performSingleRefCalibration(0x40)) { return false; }

    // -- VL53L0X_perform_vhv_calibration() end

    // -- VL53L0X_perform_phase_calibration() begin

    write8(SYSTEM_SEQUENCE_CONFIG, 0x02);
    if (!performSingleRefCalibration(0x00)) { return false; }

    // -- VL53L0X_perform_phase_calibration() end

    // "restore the previous Sequence Config"
    write8(SYSTEM_SEQUENCE_CONFIG, 0xE8);

    // VL53L0X_PerformRefCalibration() end

    return true;
}

