#include "mpu6050.hpp"

namespace asn {

Mpu6050::Mpu6050( MPU6050 &mpu ) : mpu( mpu ) {
}

void Mpu6050::setUpGyro() {
    Wire.begin();
    byte status = mpu.begin();
    delay( 1000 );
    mpu.calcOffsets();
}

float Mpu6050::getCurrent_z() {
    mpu.update();
    return mpu.getAngleZ();
}

float Mpu6050::getAcc_z() {
    mpu.update();
    return mpu.getAccZ();
}

}  // namespace asn