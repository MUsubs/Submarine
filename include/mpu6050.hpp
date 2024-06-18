#ifndef R2D2_MPU6050_HPP
#define R2D2_MPU6050_HPP

#include "Arduino.h"
#include "Wire.h"
#include <MPU6050_light.h>
#include <VarSpeedServo.h>
#include "Kalman.h"

namespace asn
{

    class Mpu6050
    {
    public:
        Mpu6050(VarSpeedServo &my_servo, MPU6050 &mpu, Kalman &kalmanFilter);
        float PID();
        float getServo_pos();
        float getSetpoint();
        void setSetpoint(float s);
        float highPassFilter(float current_value, float previous_value);
        void setGyroUp();
        float getCurrent_z();
        void Move();
        void kalman();
        float mean();

    private:
        VarSpeedServo &my_servo;
        MPU6050 &mpu;
        Kalman &kalmanFilter;
        float servo_pos = 0.0;
        float pos_prev = 0;
        float previous_z = 0.0;
        const float alpha = 0.8;
        double setpoint = 0.0;
        double error = 0.0;
        double error_sum = 0.0;
        double error_prev = 0.0;
        double error_div = 0.0;
        const double kp = 0.725;
        const double ki = 1.02;
        const double kd = 0.01;
        const double dt = 0.1;
        unsigned long current_time = 0;
        float prev_time = 0;
        float filtered_angle = 0;
        int int_count = 0;
    };

}

#endif // R2D2_MPU6050_HPP