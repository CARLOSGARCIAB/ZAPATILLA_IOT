
#include "mbed.h"
#include "rtc_api.h"
#include <cstdint>
#include <ctime>
#include <cstdlib> // Para std::rand

#include "LSM6DSL/LSM6DSLSensor.h" // libreria del sensor




// SENSOR PEDOMETRO
void Encender_Sensor(LSM6DSLSensor &sensor);




time_t getCurrentTime(); //// Obtener la hora y la fecha actuales

struct tm PonerHora(int year, int month, int day, int hour, int minute, int second, int isdst);

extern uint8_t calculateCalories(LSM6DSLSensor &sensor);

extern uint16_t CalcularPasos(LSM6DSLSensor &sensor);







