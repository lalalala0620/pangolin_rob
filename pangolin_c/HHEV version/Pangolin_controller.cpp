#include "Pangolin_controller.h"

namespace Pangolin {

    PWMServo::PWMServo(int motor_id, int servo_pwm_threshold_max, int servo_pwm_threshold_min) {
        motor_id_ = motor_id;
        servo_pwm_threshold_max_ = servo_pwm_threshold_max;
        servo_pwm_threshold_min_ = servo_pwm_threshold_min;
        if (gpioInitialise() < 0) {
            printf("Initial Fail \r\n");
        }
        for (const auto& servo_config : pwm_servo_io_dict_) {
            servo_pwm_duty_now_[servo_config.first] = gpioGetServoPulsewidth(servo_config.second);
            servo_pwm_duty_set_[servo_config.first] = gpioGetServoPulsewidth(servo_config.second);
            servo_pwm_duty_inc_[servo_config.first] = 0;
            servo_pwm_deviation_[servo_config.first] = 0;
        }   

        fstream f("ServoDeviation.json");
        json config = json::parse(f);
        for (const auto& key : config) {
            servo_pwm_deviation_[int(key)] = config[key];
        }
    }

    bool PWMServo::setPulse(int p, int servo_run_time) {
        if (p < 500 || p > 2500) {
            printf("Angle is out of range");
            return false;
        }
        if (p < servo_pwm_threshold_min_) {
            p = servo_pwm_threshold_min_;
        }
        if (p > servo_pwm_threshold_max_) {
            p = servo_pwm_threshold_max_;
        }

        servo_run_time_ = servo_run_time;
        if (servo_run_time >= 100) {
            updatePulse();
            if (servo_pwm_duty_now_[motor_id_] < 400) {
                servo_pwm_duty_now_[motor_id_] = p;
            }
        }

        servo_pwm_duty_set_[motor_id_] = p;
        servo_pwm_duty_have_chaged_ = true;

        if (servo_run_time != 0 && !call_back_thread_active_) {
            call_back_thread_active_ = true;
            thread call_back_thread(&PWMServo::callBack, this);
            call_back_thread.detach();
        }
        if (!call_back_thread_active_ || servo_run_time <= 20) {
            servo_pwm_duty_now_[motor_id_] = p;
            if (p != 0) {
                p += servo_pwm_deviation_[motor_id_];
                if (p < 500) {p = 500;}
                else if (p > 2500) {p = 2500;}
                gpioServo(pwm_servo_io_dict_[motor_id_], p);
            }
        }
        return true;
    }

    void PWMServo::updatePulse() {
        servo_pwm_duty_now_[motor_id_] = gpioGetServoPulsewidth(pwm_servo_io_dict_[motor_id_]) - servo_pwm_deviation_[motor_id_];
    }

    void PWMServo::setThreshold(int min, int max) {
        servo_pwm_threshold_max_ = max;
        servo_pwm_threshold_min_ = min;
    }

    void PWMServo::callBack() {
        int time_steps = 0;
        time_t time_last = time(NULL);
        map<int, int> servo_pwm_duty_set = servo_pwm_duty_set_;
        while (call_back_thread_active_) 
        {
            usleep(1000); // 1ms
            if (time(NULL) - time_last >= 0.02) {
                time_last = time(NULL);
                time_steps += 1;
                if (servo_pwm_duty_have_chaged_) {
                    servo_pwm_duty_have_chaged_ = false;
                    if (servo_run_time_ <= 20) {
                        servo_pwm_duty_inc_times_ = 1;
                    }
                    else {
                        servo_pwm_duty_inc_times_ = int(servo_run_time_ / 20) ;
                    }
                    for (const auto& config : servo_pwm_duty_inc_) {
                        servo_pwm_duty_inc_[config.first] = (servo_pwm_duty_set_[config.first] - servo_pwm_duty_now_[config.first]) / servo_pwm_duty_inc_times_;
                    }
                    servo_running_ = true;
                    servo_pwm_duty_set = servo_pwm_duty_set_;
                }

                if (servo_running_) {
                    servo_pwm_duty_inc_times_ -= 1;
                    for (const auto& key : servo_pwm_duty_inc_) {
                        if (servo_pwm_duty_inc_times_ == 0) {
                            servo_pwm_duty_now_[key.first] = servo_pwm_duty_set[key.first];
                            servo_running_ = false;
                        }
                        else {
                            if (servo_pwm_duty_now_[key.first] == 0 || servo_pwm_duty_now_[key.first] == servo_pwm_duty_set[key.first]) {
                                servo_pwm_duty_now_[key.first] = servo_pwm_duty_set[key.first];
                            }
                            else {
                                servo_pwm_duty_now_[key.first] += servo_pwm_duty_inc_[key.first];
                            }
                        }
                        if (servo_pwm_duty_now_[key.first] >= 500 && servo_pwm_duty_now_[key.first] <= 2500) {
                            int p = int(servo_pwm_duty_now_[key.first]);
                            if (p != 0) {
                                p += servo_pwm_deviation_[key.first];
                                if (p < 500) {
                                    p = 500;
                                }
                                else if (p > 2500) {
                                    p = 2500;
                                }
                                gpioServo(pwm_servo_io_dict_[key.first], p);
                            }
                        }
                    }
                }
            }
            if (time_steps >= 50) {
                time_steps = 0;
                fstream f("ServoDeviation.json");
                json config = json::parse(f);
                for (const auto& key : config) {
                    servo_pwm_deviation_[int(key)] = config[key];
                }
            }
        }
        
    }
    
} // namespace Pangolin