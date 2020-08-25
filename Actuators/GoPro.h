#ifndef _Actuator_GoPro_h_
#define _Actuator_GoPro_h_

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

#include "Actuator.h"
#include "../Hardware/GPIODevice.h"

class Actuator_GoPro : public Actuator
{
private:
    GPIODevice* gopro1;
    GPIODevice* gopro2;
    GPIODevice* camsupply;
    GPIODevice* lights1;
    GPIODevice* lights2;
    std::atomic<bool> goproEnabled;
    std::mutex camControlLock;

    Actuator_GoPro();

    // Dont let the programmer mess around with threads, we'll do that with caller functions
    void enableCameraThread();
	void disableCameraThread();
public:
    static Actuator_GoPro& getInstance();

	~Actuator_GoPro();
	int getActuatorType() override;
	void enable(bool debug) override;
	void disable(bool debug) override;
    void enableLights(bool debug);
    void disableLights(bool debug);
	void disableGoPro(bool debug);

	// Singleton class: prevent copy operators
    Actuator_GoPro(Actuator_GoPro const&) = delete;
    void operator=(Actuator_GoPro const&) = delete;
};

#endif
