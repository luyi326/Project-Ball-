#include "IRRim.h"
#include <stdlib.h>
#include <unistd.h>

// Assume the starting address is 0x04 beacause @Tony broke the first two ports.
#define PV_N(n) (1 << (n+2))
#define IRRIM_SEEK_INTERVAL (200000) //interval is in nanoseconds
#define IRRIM_RETRACK_INTERVAL (20000)

//Constructor and destructor
IRRim::IRRim(uint8_t num_of_sensors, pwmName servoPin, gpioName muxResetPin) :
	mux(muxResetPin),
	servo(servoPin),
	state(IRRimState_Seeking),
	servo_current_position(0),
	is_seeking(true),
	seeking_is_upwared(true)
	{
	if (num_of_sensors > 6) {
		cerr << "Number of sensors is " << num_of_sensors << ", maximum is 6" << endl;
		exit(1);
	}


	//Initialize all IRs and check status
	sensors = new PVision[num_of_sensors];
	sensor_count = num_of_sensors;
	for (int i = 0; i < sensor_count; i++) {
		mux.selectChannel(PV_N(i));
		if (!sensors[i].init()) {
			cerr << "Sensor No. " << i + 1 << " not initialized correctly" << endl;
		} else {
			//TODO: Fill this place with apporiate logger: spdLogger, Boost logger etc..
		}
	}
	servo.move_to(0);

	usleep(100); // Allow initial setup time

	//All ready, record clock and start
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time);
}

IRRim::~IRRim() {
	mux.reset();
	// delete sensors;
}

/**
 * [IRRim::reset description]
 */
void IRRim::reset() {
	mux.reset();
}

void IRRim::run() {
	if (is_seeking) {
		seek();
	} else {
		follow();
	}
}

void IRRim::seek() {
	timespec tmp;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tmp);

	timespec diff = time_diff(current_time, tmp);
	if (diff.tv_sec == 0 && diff.tv_nsec >= IRRIM_SEEK_INTERVAL) {

		//Invert seeking direction if ends meet
		if (seeking_is_upwared && servo_current_position == 180) {
			seeking_is_upwared = false;
		}
		if (!seeking_is_upwared && servo_current_position == 0) {
			seeking_is_upwared = true;
		}
		servo_current_position += seeking_is_upwared ? 2 : -2;
		cout << "Moving servo to position " << int(servo_current_position) << endl;
		servo.move_to(servo_current_position);

		read_IR(0);
		// read_IR(1);

		current_time = tmp;
	} else {
		//Time not there yet
	}


}
void IRRim::follow() {
	timespec tmp;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tmp);

	if (read_IR(0)) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &current_time);
		return;
	}

	timespec diff = time_diff(current_time, tmp);
	if (diff.tv_sec == 0 && diff.tv_nsec >= IRRIM_RETRACK_INTERVAL) {
		servo_current_position += seeking_is_upwared ? -2 : 2;
		cout << "Moving servo to position " << int(servo_current_position) << endl;
		servo.move_to(servo_current_position);
	} else {

	}
	// read_IR(0);
}

bool IRRim::read_IR(uint8_t index) {
	mux.selectChannel(PV_N(index));
	uint8_t result = sensors[index].readBlob();
	if (result & BLOB1)
	{
		is_seeking = false;
		cout << index <<  " BLOB1 detected. X:" << sensors[index].Blob1.X << " Y:" << sensors[index].Blob1.Y;
		cout << " Size: " << sensors[index].Blob1.Size << endl;
		return true;
	} else {
		// is_seeking = true;
	}
	return false;
}

void IRRim::nextSensor() {
}

void IRRim::select(uint8_t num) {
}

inline timespec IRRim::time_diff(timespec t1, timespec t2) {
	timespec temp;
	if ((t2.tv_nsec - t1.tv_nsec) < 0) {
		temp.tv_sec = t2.tv_sec - t1.tv_sec - 1;
		temp.tv_nsec = 1000000000 + t2.tv_nsec - t1.tv_nsec;
	} else {
		temp.tv_sec = t2.tv_sec - t1.tv_sec;
		temp.tv_nsec = t2.tv_nsec - t1.tv_nsec;
	}
	return temp;
}