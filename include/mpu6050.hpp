#ifndef R2D2_MPU6050_HPP
#define R2D2_MPU6050_HPP

#include <MPU6050_light.h>

#include "Arduino.h"
#include "Kalman.h"
#include "Wire.h"

class Mpu6050 {
public:
    Mpu6050( MPU6050& mpu, Kalman& kalmanFilter );
    float PID();
    float getSetpoint();
    void setSetpoint( float s );
    float highPassFilter( float current_value, float previous_value );
    void setGyroUp();
    float getCurrent_z();
    void kalman();

private:
    MPU6050& mpu;
    Kalman& kalmanFilter;
    float output = 0;
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
    unsigned long currentTime = 0;
    float prevTime = 0;
    float filteredAngle = 0;
    int int_count = 0;
};

#endif  // R2D2_MPU6050_HPP