#include "Pangolin_controller.h"
#include <ctime>

namespace Pangolin {

    PWMServo::PWMServo(int motor_id, int servo_pwm_threshold_max, int servo_pwm_threshold_min, int servo_now_pulse) {
        motor_id_ = motor_id;
        servo_pwm_duty_now_[motor_id_] = servo_now_pulse;
        servo_pwm_threshold_max_ = servo_pwm_threshold_max;
        servo_pwm_threshold_min_ = servo_pwm_threshold_min;
        if (wiringPiSetup() == -1) {
            fprintf(stdout, "oops: %s\n", strerror(errno));
        }
        softServoSetup(14, 21, 2, 12, 28, 24, 23, 22);

        // for (const auto& servo_config : pwm_servo_io_dict_) {
        //     servo_pwm_duty_now_[servo_config.first] = gpioGetServoPulsewidth(servo_config.second);
        //     servo_pwm_duty_set_[servo_config.first] = gpioGetServoPulsewidth(servo_config.second);
        //     servo_pwm_duty_inc_[servo_config.first] = 0;
        //     servo_pwm_deviation_[servo_config.first] = 0;
        // }   

        // fstream f("ServoDeviation.json");
        // json config = json::parse(f);
        // for (const auto& key : config) {
        //     servo_pwm_deviation_[int(key)] = config[key];
        // }
    }

    bool PWMServo::setPulse(int p, int servo_run_time) {
        // if (p < 500 || p > 2500) {
        //     printf("Angle is out of range");
        //     return false;
        // }
        // cout << servo_pwm_duty_now_[motor_id_] << "P:"<< p << endl;
        if (p < servo_pwm_threshold_min_) {
            p = servo_pwm_threshold_min_;
        }
        if (p > servo_pwm_threshold_max_) {
            p = servo_pwm_threshold_max_;
        }

        int delay_time  = servo_run_time*50 / abs((p - servo_pwm_duty_now_[motor_id_]));

        thread t([this, delay_time, p]() { callBack(delay_time, p); });
        t.detach();
        // cout << servo_pwm_duty_now_[motor_id_] << "P:"<< pwm_servo_io_dict_[motor_id_] << endl;
        // if (servo_pwm_duty_now_[motor_id_] < p){
        //     printf("1");
        //     for (int i=servo_pwm_duty_now_[motor_id_]; i<p; i+=50){
        //         cout << i << endl;
        //         softServoWrite(pwm_servo_io_dict_[motor_id_], i);
        //         updatePulse(i);
                
        //         delay(delay_time);
        //     }
        // }
        // else{
        //     printf("2");
        //     for (int i=servo_pwm_duty_now_[motor_id_]; i>p; i-=50){
        //         cout << i << endl;
        //         softServoWrite(pwm_servo_io_dict_[motor_id_], i);
        //         updatePulse(i);
                
        //         delay(delay_time);
        //     }
        // }

        // if (servo_run_time >= 100) {
        //     // updatePulse();
        //     if (servo_pwm_duty_now_[motor_id_] < 400) {
        //         servo_pwm_duty_now_[motor_id_] = p;
        //     }
        // }

        // servo_pwm_duty_set_[motor_id_] = p;
        // servo_pwm_duty_have_chaged_ = true;

        // if (servo_run_time != 0 && !call_back_thread_active_) {
        //     call_back_thread_active_ = true;
        //     thread call_back_thread(&PWMServo::callBack, this);
        //     call_back_thread.detach();
        // }
        // if (!call_back_thread_active_ || servo_run_time <= 20) {
        //     servo_pwm_duty_now_[motor_id_] = p;
        //     if (p != 0) {
        //         p += servo_pwm_deviation_[motor_id_];
        //         if (p < 500) {p = 500;}
        //         else if (p > 2500) {p = 2500;}
        //         softServoWrite(pwm_servo_io_dict_[motor_id_], p);
        //     }
        // }
        return true;
    }

    void PWMServo::updatePulse(int now_pulse) {
        servo_pwm_duty_now_[motor_id_] = now_pulse;
        // cout << servo_pwm_duty_now_[motor_id_] << endl;
    }

    void PWMServo::setThreshold(int min, int max) {
        servo_pwm_threshold_max_ = max;
        servo_pwm_threshold_min_ = min;
    }

    void PWMServo::callBack(int delay_time, int p) {
        if (servo_pwm_duty_now_[motor_id_] < p){
            // printf("1");
            for (int i=servo_pwm_duty_now_[motor_id_]; i<p; i+=50){
                // cout << i << endl;
                softServoWrite(pwm_servo_io_dict_[motor_id_], i);
                updatePulse(i);
                delay(delay_time);
            }
        }
        else{
            // printf("2");
            for (int i=servo_pwm_duty_now_[motor_id_]; i>p; i-=50){
                // cout << i << endl;
                softServoWrite(pwm_servo_io_dict_[motor_id_], i);
                updatePulse(i);
        
                delay(delay_time);
            }
        }
    }

    PangolinController::PangolinController(){
        std::vector<int> deviation = {150 ,-100, 100, 120};

        servo_pwm_6 = make_unique<PWMServo>(6, 1000, 0, 500 + deviation[0]);
        servo_pwm_8 = make_unique<PWMServo>(8, 1000, 0, 500 + deviation[1]);
        servo_pwm_9 = make_unique<PWMServo>(9, 1000, 0, 500 + deviation[2]);
        servo_pwm_10 = make_unique<PWMServo>(10, 1000, 0, 500 + deviation[3]);
        delay(1000);
        // softServoWrite(1, 1750);
        // sleep(1);
    }
    PangolinController::~PangolinController(){
    }

    void PangolinController::initServoStatus(){  
        // std::vector<int> deviation = {100, 120, 150 ,-100};
        // std::vector<int> deviation = {150 ,-100, 100, 120};
        // servo_pwm_6->setPulse(500 + deviation[0], 500);
        // servo_pwm_8->setPulse(500 + deviation[1], 500);         
        // servo_pwm_9->setPulse(500 + deviation[2], 500);
        // servo_pwm_10->setPulse(500 + deviation[3], 500);
       
        delay(1000);
    }

    void PangolinController::motorMove(std::vector<int> servoAngle, int servo_use_time, std::vector<float> servoRate){  
        std::vector<int> deviation = {0 ,-100, 30, 50};

        servo_pwm_6->setPulse(servoRate[0]*servoAngle[0]*6.25+500 + deviation[0], servo_use_time);
        servo_pwm_8->setPulse(servoRate[1]*servoAngle[1]*6.25+500 + deviation[1], servo_use_time);
        servo_pwm_9->setPulse(servoRate[0]*servoAngle[2]*6.25+500 + deviation[2], servo_use_time);
        servo_pwm_10->setPulse(servoRate[1]*servoAngle[3]*6.25+500 + deviation[3], servo_use_time);
        delay(servo_use_time);
    }

    bool PangolinController::cmdProcessLoop(float linear, float angular){
        std::vector<int> motorValues;
        std::vector<float> cmdVel;
        motorValues = {20, 0, 0 ,-20};
        // cmdVel = {linear[0] - angular[2], linear[0] + angular[2]};
        cmdVel = {linear - angular, linear + angular};
        int servoUseTime = 40;
        motorMove(motorValues, servoUseTime, cmdVel);

        motorValues = {-38, 0, 0, 38};
        motorMove(motorValues, servoUseTime, cmdVel);

        motorValues = {-40, -40, 40, 40};
        motorMove(motorValues, servoUseTime, cmdVel);

        motorValues = {0, -20, 20, 0};
        motorMove(motorValues, servoUseTime, cmdVel);

        motorValues = {0, 38, -38, 0};
        motorMove(motorValues, servoUseTime, cmdVel);

        motorValues = {40, 40, -40, -40};
        motorMove(motorValues, servoUseTime, cmdVel);

        return true;
    }



    void PangolinController::taskThread(float linear, float angular){
        future<bool> task = async(std::launch::async, &PangolinController::cmdProcessLoop, this, linear, angular);
        PangolinController::task_done_ = task.get(); 
    }


    void PangolinController::controlThread(float linear, float angular){
        cout << PangolinController::task_done_ << endl;
        if (PangolinController::task_done_ == true){
            PangolinController::task_done_ = false;
            thread controlTask(&PangolinController::taskThread, this, linear, angular);
            controlTask.detach();
        }
        else{
            
        }


        // thread t(pangolin_ctrl.cmdProcessLoop, event->linear, event->angular);
    }

    
} // namespace Pangolin