#include "steer_control.hpp"

#define R2D2_DEBUG_ENABLE
#include "r2d2_debug_macros.hpp"

namespace asn {

SteerControl::SteerControl( Mpu6050 &mpu, MotorControl &motor_control, Kalman &kalman_filter ) :
    mpu( mpu ), motor_control( motor_control ), kalman_filter( kalman_filter ), stop( true ) {
}

void SteerControl::setSetpoint( float s ) {
    setpoint = s;
}

float SteerControl::highPassFilter( float current_value, float previous_value ) {
    return alpha * ( previous_value + current_value - alpha * previous_value );
}

void SteerControl::PID() {
    float gyro_z = mpu.getCurrent_z();
    float current_z = highPassFilter( gyro_z, previous_z );
    kalman();

    error = setpoint - current_z;
    error_sum += error * dt;
    error_div = ( error - error_prev ) / dt;
    steer_action = ( kp * error + ki * error_sum + kd * error_div );
    error_prev = error;

    pos_prev = steer_action;
    previous_z = current_z;
    // Serial.printf( "Steer action: %f\n", steer_action );

    if ( round( mpu.getCurrent_z() + 5 ) < steer_action ) {
        // Serial.println( "LEFT" );
        motor_control.move( motor_control.direction_t::LEFT );
        vTaskDelay( wait_time );
    } else if ( round( mpu.getCurrent_z() - 5 ) > steer_action ) {
        // Serial.println( "RIGHT" );
        motor_control.move( motor_control.direction_t::RIGHT );
        vTaskDelay( wait_time );
    } else {
        // Serial.println( "FORWARD" );
        motor_control.move( motor_control.direction_t::FORWARD );
        vTaskDelay( wait_time );
    }
    motor_control.move( motor_control.direction_t::STOP );
}

void SteerControl::kalman() {
    current_time = millis();

    steer_action =
        kalman_filter.getAngle( mpu.getCurrent_z(), mpu.getAcc_z(), ( current_time - prev_time ) / 1000 );

    prev_time = current_time;
}

void SteerControl::setUpSteerControl() {
    mpu.setUpGyro();
    kalman_filter.setAngle( 0.0f );
    kalman_filter.setQangle( 0.001f );
    kalman_filter.setQbias( 0.0067f );
    kalman_filter.setRmeasure( 0.075f );
    prev_time = millis();
}

void SteerControl::main() {
    // Serial.println( "start steer" );
    for ( ;; ) {
        if ( !stop ) {
            PID();
        } else {
            vTaskDelay( 2 );
        }
    }
}

void SteerControl::disable() {
    R2D2_DEBUG_LOG( "disable steer" );
    stop = true;
}

void SteerControl::enable() {
    R2D2_DEBUG_LOG( "enable steer" );
    stop = false;
}

}  // namespace asn
