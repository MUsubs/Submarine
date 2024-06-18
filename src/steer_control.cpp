#include "steer_control.hpp"

namespace asn {

SteerControl::SteerControl(Mpu6050 &mpu) : mpu(mpu)
{
}

float SteerControl::PID() {
    float gyro_z = mpu.getCurrent_z();
    float current_z = highPassFilter(gyro_z, previous_z);

    error = setpoint - current_z;
    error_sum += error * dt;
    error_div = (error - error_prev) / dt;
    servo_pos = (kp * error + ki * error_sum + kd * error_div);
    error_prev = error;

    pos_prev = servo_pos;
    previous_z = current_z;

    return servo_pos;
}

} // namespace asn