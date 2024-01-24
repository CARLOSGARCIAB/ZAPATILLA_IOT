
#pragma once
#include "mbed.h"
#include "LSM6DSL/LSM6DSLSensor.h" // libreria del sensor


    DevI2C devI2c(PB_11, PB_10); // Crea la instancia de la comunicacion Serie con el modulo BT
    LSM6DSLSensor sensor(&devI2c); // Crea la instancia del sensor
    DigitalOut LEDBT (LED3); // Ponemos el led AZUL a funcionar
    Ticker ticker; // Interrupcion para el parpadeo del led
     