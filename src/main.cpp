#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "data_transceiver.hpp"
#include "message_interpreter.hpp"
#include "message_printer.hpp"
#include "packet_enums.hpp"

#define R2D2_DEBUG_ENABLE
#include "r2d2_debug_macros.hpp"

bool is_sub = false;

sen::MessageInterpreter message_interpreter{ 20, 1 };
sen::DataTransceiver data_transceiver{ 10, 9, 2, is_sub, message_interpreter, 1 };
sen::MessagePrinter message_printer{};

void setup() {
    Serial.begin( 115200 );

    while ( !Serial );
    vTaskDelay( 2000 );

    message_interpreter.setListener( &message_printer );

    message_interpreter.activate();
    data_transceiver.activate();
}

std::vector<uint8_t> data;
uint8_t header;

void loop() {
    // // update 0 0 0
    // header = data_transceiver.generateUpdateHeader(sen::data_t::CURR, 3);
    // data = {header, 0, 0, 0};
    // data_transceiver.sendBytes(data);
    // Serial.println("Data:");
    // for (auto b : data){
    //     Serial.printf("%d\n", b);
    // }
    // Serial.println();
    // vTaskDelay(500);

    // new pos 1 0 1
    int time = 200;

    header = data_transceiver.generateInstructionHeader( sen::inst_t::NEW_POS, 3 );
    data = { header, 255, 0, 255 };
    data_transceiver.sendBytes( data );
    R2D2_DEBUG_LOG( "sending NEW_POS 255, 0, 255" );
    vTaskDelay( time );

    header = data_transceiver.generateInstructionHeader( sen::inst_t::ACK, 0 );
    data = { header };
    data_transceiver.sendBytes( data );
    R2D2_DEBUG_LOG( "sending ACK" );
    vTaskDelay( time );

    header = data_transceiver.generateUpdateHeader( sen::data_t::CURR, 3 );
    data = { header, 127, 255, 127 };
    data_transceiver.sendBytes( data );
    R2D2_DEBUG_LOG( "sending UPDATE 127, 255, 127" );
    vTaskDelay( time );

    header = data_transceiver.generateSensorHeader( sen::sens_t::TEMP, 2 );
    data = { header, 30, 50 };
    data_transceiver.sendBytes( data );
    R2D2_DEBUG_LOG( "sending SENS 30.5" );
    vTaskDelay( time );
    taskYIELD();
}
