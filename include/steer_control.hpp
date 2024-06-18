#ifndef R2D2_STEERCONTROL_HPP
#define R2D2_STEERCONTROL_HPP

#include "mpu6050.hpp"
#include "motor_control.hpp"

namespace asn {

class SteerControl
{
public:
    SteerControl(Mpu6050 &mpu);
    void setSetpoint(float s);
    float PID();
    void sendMove();

private:
    float steer_action;
    float pos_prev = 0.0;
    double setpoint = 0.0;
    double error = 0.0;
    double error_sum = 0.0;
    double error_prev = 0.0;
    double error_div = 0.0;
    const double kp = 0.725;
    const double ki = 1.02;
    const double kd = 0.01;
    const double dt = 0.1;
};

} // namespace asn
#endif // R2D2_STEERCONTROL_HPP