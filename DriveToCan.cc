
#include "acfw/execute/EventBasedRunnable.h"
#include "acfw/execute/Timer.h"
#include "acfw/log/Logger.h"
#include <iostream>
#include <cmath>

#include "ackermann_msgs/AckermannDrive.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Bool.h"
#include "can_msgs/Frame.h"

#include "TinyJson.h"

using namespace acfw::core;
using namespace acfw::execute;
using namespace acfw::cm;

class DriveToCan : public EventBasedRunnable
{
    Timer m_Timer;
    std::vector<can_msgs::Frame> m_canSendGrp;

    can_msgs::FrameConsumer m_canReader;
    ackermann_msgs::AckermannDriveConsumer m_ackerReader;
    std_msgs::Int32Consumer m_brakeReader;
    std_msgs::BoolConsumer m_switchReader;

    can_msgs::FrameProvider m_canSender;
    std_msgs::Float32Provider m_steeringSender;
    std_msgs::Float32Provider m_rearRpmSender;
    std_msgs::Float32Provider m_rearSpeedSender;

public:
    DriveToCan()
    {
        // Register message handlers (on() methods below)
        registerMsg<Service::State, Service &>(*this);

        registerMsg<can_msgs::Frame, can_msgs::FrameConsumer&>(*this);
        registerMsg<ackermann_msgs::AckermannDrive, ackermann_msgs::AckermannDriveConsumer&>(*this);
        registerMsg<std_msgs::Int32, std_msgs::Int32Consumer&>(*this);
        registerMsg<std_msgs::Bool, std_msgs::BoolConsumer&>(*this);
    }

    // Service State
    void on(const Service::State &state, Service &service)
    {
        InfoLog("Event %s state changed to %d", service.getInstanceId(), (int)state);
    }

    // can callback
    void on(const can_msgs::Frame& msg, can_msgs::FrameConsumer& instance)
    {
        
        InfoLog("CAN");
        (void)instance;
        if (msg.id == 0x191)
        {
            std_msgs::Float32 st_angle;

            static float str_data;
            str_data = (float)(pow(-1, msg.data[0]) * msg.data[1]) / 180 * M_PI;

            st_angle.data = str_data;

            m_steeringSender.sendEvent(&st_angle);
        }

        else if (msg.id == 0x192)
        {
            double cur_rear_rpm_ = 0.0;
            double cur_rear_vel_ = 0.0;
            std_msgs::Float32 rear_rpm;
            std_msgs::Float32 rear_speed;
            static uint32_t rpm;
            static double encoder_d = 0.05;
            static double rear_wheel_d = 0.66;

            // If Big endian, uncomment this
            // std::reverse(std::begin(msg->data), std::end(msg->data));

            rpm = static_cast<uint32_t>(msg.data[0]) | (static_cast<uint32_t>(msg.data[1]) << 8);

            cur_rear_rpm_ = ((double)rpm) * encoder_d / rear_wheel_d;
            rear_rpm.data = cur_rear_rpm_;

            cur_rear_vel_ = cur_rear_rpm_ * 2 * M_PI * (rear_wheel_d/2) / 60;
            rear_speed.data = cur_rear_vel_;

            // RCLCPP_INFO(this->get_logger(), "rear_vel=%lf", cur_rear_vel_);
            m_rearRpmSender.sendEvent(&rear_rpm);
            m_rearSpeedSender.sendEvent(&rear_speed);
        }
    }

    // acker cmd callback
    void on(const ackermann_msgs::AckermannDrive& msg, ackermann_msgs::AckermannDriveConsumer& instance)
    {
        InfoLog("acker");
        (void)instance;
        static can_msgs::Frame can_frame;
        static int max_rpm = 15000;
        static double motor_rpm;
        static int32_t rnd_motor_rpm;
        uint8_t middr_message[4];

        motor_rpm = (msg.speed/255*7) * 1500.0 + 150;

        motor_rpm = std::max(std::min((int)motor_rpm, max_rpm), 0);
        rnd_motor_rpm = (int32_t)round(motor_rpm);
        memcpy(middr_message, &rnd_motor_rpm, sizeof(rnd_motor_rpm));

        can_frame.is_extended = true;
        can_frame.id = 0x0361;
        can_frame.dlc = 8;
        can_frame.data[0] = middr_message[3];
        can_frame.data[1] = middr_message[2];
        can_frame.data[2] = middr_message[1];
        can_frame.data[3] = middr_message[0];
        can_frame.data[4] = 0;
        can_frame.data[5] = 0;
        can_frame.data[6] = 0;
        can_frame.data[7] = 0;
    
        m_canSendGrp.push_back(can_frame);

        static uint8_t steering_angle = 1;
        bool sign = (msg.steering_angle < 0);
        steering_angle = static_cast<uint8_t>(std::abs(msg.steering_angle)/M_PI*180);        
	    can_frame.is_extended = false;
        can_frame.id = 0x211;
        can_frame.dlc = 2;
        can_frame.data[0] = static_cast<uint8_t>(sign);
        can_frame.data[1] = steering_angle;
        can_frame.data[2] = 0;
        can_frame.data[3] = 0;
        can_frame.data[4] = 0;
        can_frame.data[5] = 0;
        can_frame.data[6] = 0;
        can_frame.data[7] = 0;

        m_canSendGrp.push_back(can_frame);
    }

    // brake cmd callback
    void on(const std_msgs::Int32& msg, std_msgs::Int32Consumer& instance)
    {
        InfoLog("brake");
        (void)instance;
        static can_msgs::Frame can_frame;
        can_frame.id = 0x20D;
        can_frame.dlc = 2;
        can_frame.data[0] = static_cast<uint8_t>(msg.data);
	can_frame.data[1] = 0;
        m_canSendGrp.push_back(can_frame);
    }

    // switch callback
    void on(const std_msgs::Bool& msg, std_msgs::BoolConsumer& instance)
    {
        InfoLog("switch");
        (void)instance;
        static can_msgs::Frame can_frame;
        can_frame.id = 0x13;
        can_frame.dlc = 2;
        can_frame.data[0] = static_cast<uint8_t>(10);
        can_frame.data[1] = static_cast<uint8_t>(msg.data);
        m_canSendGrp.push_back(can_frame);
    }

    bool init(const std::string& config) noexcept override
    {
        EventBasedRunnable::init(config);
        // Create services
        std::string error;
        Json configObj = Json::parse(config, error);
        if (!error.empty())
        {
            ErrorLog("Couldn't parse runnable config: %s", error.c_str());
            return false;
        }

        bool start = true;

        // Create m_canReader
        const auto specCanReader = configObj["services"]["0"];
        auto r = m_canReader.create(id(), specCanReader.dump().c_str());
        if (r != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_canReader.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }

        // Create m_ackerReader
        const auto specAckerReader = configObj["services"]["1"];
        auto r_ackerReader = m_ackerReader.create(id(), specAckerReader.dump().c_str());
        if (r_ackerReader != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_ackerReader.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }

        // Create m_brakeReader
        const auto specBrakeReader = configObj["services"]["2"];
        auto r_brakeReader = m_brakeReader.create(id(), specBrakeReader.dump().c_str());
        if (r_brakeReader != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_brakeReader.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }

        // Create m_switchReader
        const auto specSwitchReader = configObj["services"]["3"];
        auto r_switchReader = m_switchReader.create(id(), specSwitchReader.dump().c_str());
        if (r_switchReader != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_switchReader.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }

        // Create m_canSender
        const auto specCanSender = configObj["services"]["4"];
        auto r_canSender = m_canSender.create(id(), specCanSender.dump().c_str());
        if (r_canSender != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_canSender.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }

        // Create m_steeringSender
        const auto specSteeringSender = configObj["services"]["5"];
        auto r_steeringSender = m_steeringSender.create(id(), specSteeringSender.dump().c_str());
        if (r_steeringSender != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_steeringSender.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }

        // Create m_rearRpmSender
        const auto specRearRpmSender = configObj["services"]["6"];
        auto r_rearRpmSender = m_rearRpmSender.create(id(), specRearRpmSender.dump().c_str());
        if (r_rearRpmSender != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_rearRpmSender.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }


        // Create m_rearSpeedSender
        const auto specRearSpeedSender = configObj["services"]["7"];
        auto r_rearSpeedSender = m_rearSpeedSender.create(id(), specRearSpeedSender.dump().c_str());
        if (r_rearSpeedSender != Result::OK)
        {
            ErrorLog("Create m__point_inputConsumer failed");
            return false;
        }
        if (m_rearSpeedSender.start() != Result::OK)
        {
            ErrorLog("Start m__point_inputConsumer failed");
            return false;
        }


        InfoLog("Service create %s", (start?"success":"fail"));

        m_Timer.reset(Millisec(100), [this]() {
            periodControl();
        });
        m_Timer.startAfter(Millisec(100));

        std::clog << "Finish initialize" << std::endl;

        return true;
    }

    void periodControl()
    {
        for (auto msg : m_canSendGrp)
        {
            m_canSender.sendEvent(&msg);
        }
        m_canSendGrp.clear();
    }
};

DEFINE_RUNNABLE_PLUGIN(DriveToCan)
