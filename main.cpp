
#include "PinNames.h"
#include "ThisThread.h"
#include "arch.h"
#include "mbed.h"
#include "tareas.h"



int main() {
    
      //No necestiamos más de 2 tareas, la de inicio, y la que gestiona el BLE.
     // No necesitamos exclusión mutua ni colas entre ambas.
    // LA tarea  : servidorGATTTThread utiliza una cola de eventos asincronos para la correcta comunicacion.

    
    Thread inicializacionThread(osPriorityHigh, OS_STACK_SIZE, nullptr, "inicializacionTask");
    inicializacionThread.start(inicializacionTask);

    Thread servidorGATTTThread(osPriorityNormal, OS_STACK_SIZE, nullptr, "servidorGATTTask");
    servidorGATTTThread.start(servidor_GATTTask);


    while (true) { // No debería llegar nunca aqui
        ThisThread::sleep_for(1s);
    }
    return 0;
}

