
#include "mbed.h"
#include "instancias.h"
#include "funciones.h"
#include "ble_process.h"
#include "SERVIDOR_GATT_PROPIO.h"
// Tarea de inicialización
void inicializacionTask() {
    // Colocar aquí el código de inicialización
    
    Encender_Sensor(sensor);
    struct tm t = PonerHora(2023, 1, 22, 15, 37, 30, -1);
    time_t HoraActual = mktime(&t);
    set_time(HoraActual);

    while (true) {
        
    ThisThread::flags_wait_any(0); // Pongo la tarea en bloqueo
       
    }   
}

// Tarea del servidor GATT
void servidor_GATTTask() {
    
    events::EventQueue ColaEv; // Cola para manejar eventos
    ble_error_t error;
    BLE& ble = BLE::Instance();
    error = ble.init();
        if (error != 0) {
            //printf("Error inicio BLE");
        }
    ServicioMonitoreo Monitor(ble);
    ServicioActividad Activi(ble);
    Persona Carlos(ble);
    Historial historial(ble);
    EventHandler HDL(ble, ColaEv, Monitor, Activi, Carlos, historial);
    BLEProcess GAP(ColaEv, ble);
    GAP.start();
    
    ColaEv.dispatch_forever(); // Ejemplo: esperar 1 segundo

    while (true) {
        
       
       
    }
}