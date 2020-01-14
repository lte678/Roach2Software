#ifndef ROCKET_SIGNALS_HEADER_FILE
#define ROCKET_SIGNALS_HEADER_FILE

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <thread>
#include <atomic>
#include <cmath>
#include <mutex>

#include "Roach2_Sensor_Sensor.h"

class RocketSignals :
        public Sensor
{
private:
    bool sods;
	bool soe;
	bool lo;
	int fd_sods;
	int fd_soe;
	int fd_lo;
	std::atomic<bool> changed;
	std::atomic<bool> running;
	std::mutex access_limit;
	double sig_lo_acc;
	double sig_sods_acc;
	double sig_soe_acc;
	int sig_counter;
	int max_sig_counter = 100; // 100 samples
	int sig_sample_interval = 500; // 100 ns zwischen samples
public:
	explicit RocketSignals(float updateFreq);
	~RocketSignals() = default;
    void init() override;
    void update() override;
    std::unique_ptr<Data> getData() override;
    int getI2CAddr() override;
    SensorType getSensorType() override;
	bool signalChanged();
	bool getLO();
	bool getSOE();
	bool getSODS();
};

#endif

