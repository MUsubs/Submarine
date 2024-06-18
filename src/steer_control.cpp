#include "steer_control.hpp"

namespace asn {

SteerControl::SteerControl(Mpu6050 &mpu) : mpu(mpu)
{
}

void SteerControl::setSetpoint(float s){
    setpoint = s;
}

float SteerControl::PID() {
    float gyro_z = mpu.getCurrent_z();
    float current_z = highPassFilter(gyro_z, previous_z);

    error = setpoint - current_z;
    error_sum += error * dt;
    error_div = (error - error_prev) / dt;
    steer_action = (kp * error + ki * error_sum + kd * error_div);
    error_prev = error;

    pos_prev = steer_action;
    previous_z = current_z;

    return steer_action;
}

void SteerControl::sendMove() {
    if ( round( gyro.getCurrent_z() ) < steer_action ) {
        motorControl.move( motorControl.direction_t::LEFT );
        delay( 50 );
    } else if ( round( gyro.getCurrent_z() ) > steer_action ) {
        motorControl.move( motorControl.direction_t::RIGHT );
        delay( 50 );
    }
}

} // namespace asn
