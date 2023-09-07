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
#include <future>

//  Pigpio
#include <wiringPi.h>
#include <softServo.h>
#include <condition_variable>

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
        PWMServo(int motor_id, int servo_pwm_threshold_max, int servo_pwm_threshold_min,  int servo_now_pulse); // max: 1800 min: 1000
        ~PWMServo() {call_back_thread_active_ = false;}

        bool setPulse(int p, int servo_run_time = 0);
        void setThreshold(int min, int max);

        private:

        void callBack(int delay_time, int p);
        void updatePulse(int now_pulse);
        bool call_back_thread_active_ = false;
        int motor_id_;
        int servo_run_time_ = 20;
        bool servo_pwm_duty_have_chaged_ = false;
        int servo_pwm_duty_inc_times_ = 0;
        bool servo_running_ = false;
        int servo_now_pulse_;
        int servo_pwm_threshold_max_;
        int servo_pwm_threshold_min_;
        
        map<int, int> servo_pwm_duty_now_;
        map<int, int> servo_pwm_duty_set_;
        map<int, int> servo_pwm_duty_inc_;
        map<int, int> servo_pwm_deviation_;
        map<int, int> pwm_servo_io_dict_ = {{1, 7},
                                            {2, 1},
                                            {3, 2},
                                            {4, 12},
                                            {5, 28},
                                            {6, 24},
                                            {7, 23},
                                            {8, 22},
                                            {9, 14},
                                            {10, 21}};
    };

    class PangolinController {

        public:
        PangolinController();
        ~PangolinController();

        // void forward(array<4, int> servo_angle, int dt, array<2, int> servo_rate);

        void initServoStatus();
        void motorMove(std::vector<int> servoAngle, int servo_use_time, std::vector<float> servoRate);
        // void cmdProcessLoop(std::vector<float> linear, std::vector<float> angular);
        bool cmdProcessLoop(float linear, float angular);
        void taskThread(float linear, float angular);
        void controlThread(float linear, float angular);
        private:
        // void motorMove(std::vector<int> servoAngle, int servo_use_time, std::vector<int> servoRate);
        vector<int> deviation;
        void initJoystick();
        bool joyIsConnect();
        clock_t a,b, c;
        bool task_done_ = true;

        void joyProcessLoop();

        unique_ptr<PWMServo> servo_pwm_9;
        unique_ptr<PWMServo> servo_pwm_10;
        unique_ptr<PWMServo> servo_pwm_6;
        unique_ptr<PWMServo> servo_pwm_8;

    };
}
#endif // PANGOLIN_CONTROLLER_H_