#ifndef PANGOLIN_CONTROLLER_H_
#define PANGOLIN_CONTROLLER_H_
// Standard includes
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <sys/time.h>

//  Pigpio
#include <pigpio.h> 

// Joystick
#include "joystick.hh"

#include "json.hpp"

#include "BlockQueue.h"

// using std namespaces
using namespace std;
using json = nlohmann::json;

namespace Pangolin {

    class PWMServo {

        public:
        PWMServo(int motor_id, int servo_pwm_threshold_max, int servo_pwm_threshold_min); // max: 1800 min: 1000
        ~PWMServo() {call_back_thread_active_ = false;}

        bool setPulse(int p, int servo_run_time = 0);
        void setThreshold(int min, int max);

        private:

        void callBack();
        void updatePulse();
        bool call_back_thread_active_ = false;
        int motor_id_;
        int servo_run_time_ = 20;
        bool servo_pwm_duty_have_chaged_ = false;
        int servo_pwm_duty_inc_times_ = 0;
        bool servo_running_ = false;
        int servo_pwm_threshold_max_;
        int servo_pwm_threshold_min_;
        
        map<int, int> servo_pwm_duty_now_;
        map<int, int> servo_pwm_duty_set_;
        map<int, int> servo_pwm_duty_inc_;
        map<int, int> servo_pwm_deviation_;
        map<int, int> pwm_servo_io_dict_ = {{1, 4},
                                        {2, 14},
                                        {3, 27},
                                        {4, 10},
                                        {5, 20},
                                        {6, 19},
                                        {7, 13},
                                        {8, 6},
                                        {9, 11},
                                        {10, 5}};
    };

    class PangolinController {

        public:
        PangolinController();
        ~PangolinController();

        // void forward(array<4, int> servo_angle, int dt, array<2, int> servo_rate);

        private:

        void initServoStatus();
        void initJoystick();
        bool joyIsConnect();
        void cmdProcessLoop();
        void joyProcessLoop();



    };
}
#endif // PANGOLIN_CONTROLLER_H_