#ifndef R2D2_MPU6050_HPP
#define R2D2_MPU6050_HPP

#include <Arduino.h>
#include <MPU6050_light.h>

#include "Wire.h"

namespace asn {
/**
 * @class Class Mpu6050 mpu6050.hpp
 * @brief A class that controls the mpu6050 gyroscope
 */

class Mpu6050 {
public:
    Mpu6050( MPU6050 &mpu );
    /**
     * @brief Starts the setup sequence of the mpu6050
     * @details Starts the mpu6050 and applies a delay of 1 second so the mpu6050 can calibrate itself.
     */
    void setUpGyro();
    /**
     * @brief Returns the current z angle of the mpu6050
     * @return The Z-angle as calculated by the mpu6050
     */
    float getCurrent_z();
    /**
     * @brief Returns the current z acceleration of the mpu6050
     * @return The Z-acceleration as calculated by the mpu6050
     */
    float getAcc_z();

private:
    MPU6050 &mpu;
};

}  // namespace asn
#endif  // R2D2_MPU6050_HPP
