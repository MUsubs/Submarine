#ifndef R2D2_DEBUG_MACROS_HPP
#define R2D2_DEBUG_MACROS_HPP

#ifdef R2D2_DEBUG_ENABLE
#define R2D2_DEBUG_LOG( format, ... ) Serial.printf( "==DEBUG== " format "\n", ##__VA_ARGS__ )
#else
#define R2D2_DEBUG_LOG( format, ... )
#endif  // R2D2_DEBUG_ENABLE

#endif  // R2D2_DEBUG_MACROS_HPP