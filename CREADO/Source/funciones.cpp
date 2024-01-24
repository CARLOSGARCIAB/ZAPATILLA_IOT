
#include "funciones.h"
#include <cstdint>
//#include "rtc_api.h"
//#include <ctime>
//TODO LO RELACIONADO CON LA HORA Y LA FECHA

time_t getCurrentTime() {
    // Obtener la hora y la fecha actuales
    return time(NULL);
}

void Encender_Sensor(LSM6DSLSensor &sensor){
    // Inicializar el sensor
    if (sensor.init(NULL) != 0) {
        printf("Error initializing sensor\n");
        return;
    }
    printf("Sensor Iniciado\n");
    sensor.enable_x();
    sensor.enable_pedometer();
}


//Ejemplo de uso:
//struct tm t = createCustomTime(2022, 9, 30, 12, 0, 0, -1);

struct tm PonerHora(int year, int month, int day, int hour, int minute, int second, int isdst) {
    struct tm t;
    t.tm_year = year - 1900;  // Los años se cuentan desde 1900
    t.tm_mon = month - 1;     // Los meses se cuentan desde 0
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;
    t.tm_isdst = isdst;       // -1 significa que no se aplica el horario de verano

    return t;
}
//// Convertir la estructura tm a un valor time_t
//time_t currentTime = mktime(&t);

// Establecer la hora del RTC
//set_time(currentTime);

// Calcular la diferencia en segundos
//double secondsDifference = difftime(currentTime, otherTime);




