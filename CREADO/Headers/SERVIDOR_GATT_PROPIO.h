

// Aquí incluiremos las clases de los servicios y características que vamos a utilizar

#include "GattCharacteristic.h"
#include "blecommon.h"
#include "mbed.h"
#include "ble/BLE.h"
#include "events/EventQueue.h"
#include "funciones.h"
#include "instancias.h"
#include "ChainableGapEventHandler.h"
#include "pretty_printer.h"
#include <chrono>
#include <cstdint>
#include <errno.h>
#include <iostream> 
#include <iterator>
#include "ble/GattServer.h" 
#include <vector>








using namespace std::chrono;
using namespace ble;
   /**
     * @brief  Constructs a new GattCharacteristic.
     *
     * @param[in] uuid The UUID of this characteristic.
     * @param[in] valuePtr Memory buffer holding the initial value. The value is
     * copied into the Bluetooth subsytem when the enclosing service is added.
     * Thereafter, the stack maintains it internally.
     * @param[in] len The length in bytes of this characteristic's value.
     * @param[in] maxLen The capacity in bytes of the characteristic value
     * buffer.
     * @param[in] props An 8-bit field that contains the characteristic's
     * properties.
     * @param[in] descriptors A pointer to an array of descriptors to be included
     * within this characteristic. The caller owns the memory for the descriptor
     * array, which must remain valid at least until the enclosing service is
     * added to the GATT table.
     * @param[in] numDescriptors The number of descriptors presents in @p
     * descriptors array.
     * @param[in] hasVariableLen Flag that indicates if the attribute's value
     * length can change throughout time.
     *
     * @note If valuePtr is nullptr, length is equal to 0 and the characteristic
     * is readable, then that particular characteristic may be considered
     * optional and dropped while instantiating the service with the underlying
     * BLE stack.
     *
     * @note A Client Characteristic Configuration Descriptor (CCCD) should not
     * be allocated if either the notify or indicate flag in the @p props bit
     * field; the underlying BLE stack handles it.
     *
     * @attention GattCharacteristic registered in a GattServer must remain
     * valid for the lifetime of the GattServer.
     */

//      SERVICIO MONITOREO // SERVICIO ACTIVIDAD // SERVICIO HISTORIAL // SERVICIO DATOS PERSONA

struct Histo {     // ESTRUCTURA PARA MANEJAR DATOS DEL HISTORICO --> USO DE ARRAY
    //time_t Start=getCurrentTime(); // Hora de inicio -->Sería un buen trabajo futuro implementarla 
   // time_t End=getCurrentTime(); // Hora de final -->Sería un buen trabajo futuro implementarla 
    bool Mode=false; // true si es monitoreo, false si es actividad
    uint16_t Cal_Total=0; // calorias consumidas en ese periodo
 };
// LAS CLASES QUE FORMAN CADA UNO DE LOS SERVICIOS BLE

Histo Array_Histo[100];

class ServicioMonitoreo : public GattServer::EventHandler {
    public:
        ServicioMonitoreo(BLE &bleInstance) : 
            ble(bleInstance),
            servicioUUID(0x0001),
            caracteristica1UUID(0x0003),
            Caracteristica1(nullptr),
            Servicio(nullptr)
            
        {
            
            uint8_t Valor_Inicial[] = {0};
            // Crear la característica de lectura con un valor inicial
            Caracteristica1 = new GattCharacteristic(
                caracteristica1UUID,
                Valor_Inicial,
                sizeof(Valor_Inicial),
                sizeof(Valor_Inicial),
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
            );
               
            GattCharacteristic *caracteristicas[] = {Caracteristica1};

            Servicio = new GattService(
                servicioUUID,
                caracteristicas,
                sizeof(caracteristicas) / sizeof(GattCharacteristic*)
            );

            // Añadir el servicio al servidor GATT 
            if (ble.gattServer().addService(*Servicio) == 0) {
                
                
            }
        }
        
    public:
             GattCharacteristic *Caracteristica1;
    private:
        BLE &ble;
        UUID servicioUUID;
        UUID caracteristica1UUID;
        GattService *Servicio;
   
    };

class Historial : public GattServer::EventHandler {
    public:
        Historial(BLE &bleInstance) :
            ble(bleInstance),
            servicioUUID(0x0040),
            caracteristica1UUID(0x0041),
            caracteristica2UUID(0x0042),
            Histor(nullptr),
            Objetivo(nullptr),
            Servicio(nullptr) {
      
            uint8_t VALOR_INICIAL_OBJETIVO []= {100};
            // Crear la característica de lectura con un valor inicial
            Histor = new GattCharacteristic( // AL SOLO ACEPTAR DATOS UINT8, LA CONVERSION DE TIME_T ES COMPLICADA --> TRABAJO FUTURO
                caracteristica1UUID,
                reinterpret_cast<uint8_t*>(&Array_Histo),
                sizeof(Histo),
                sizeof(Array_Histo), // Tamaño suficiente para el array que almacena los datos del hisotrial
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            );

            Objetivo = new GattCharacteristic(
                caracteristica2UUID,
                VALOR_INICIAL_OBJETIVO,
                sizeof(VALOR_INICIAL_OBJETIVO),
                sizeof(VALOR_INICIAL_OBJETIVO),
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            );

            // Crear un array de GattCharacteristic
            GattCharacteristic* caracteristicas[] = {Histor,Objetivo};

            // Crear la instancia de GattService
            Servicio = new GattService(
                servicioUUID,
                caracteristicas,
                sizeof(caracteristicas) / sizeof(GattCharacteristic*)
            );

            // Añadir el servicio al servidor GATT después de haberse creado
            if (ble.gattServer().addService(*Servicio) == 0) {
                //printf("Servicio1 Añadido\n");
            }
    }

    public:
        GattCharacteristic *Histor;
        GattCharacteristic *Objetivo;

    private:
        BLE &ble;
        UUID servicioUUID;
        UUID caracteristica1UUID;
        UUID caracteristica2UUID;
        GattService *Servicio;
    };

class ServicioActividad :public GattServer::EventHandler {
        
        public:
            ServicioActividad(BLE& bleInstance) : 
                ble(bleInstance),
                servicioUUID(0x0020),
                caracteristica1UUID(0x0021),
                caracteristica2UUID(0x0022),
                caracteristica3UUID(0x0023),
                Intensidad(nullptr),
                Tiempo_Adquisicion(nullptr),
                Umbral_Intensidad(nullptr),
                Servicio(nullptr)
            {
                
                Intensidad = new GattCharacteristic( // INTENSIDAD
                    caracteristica1UUID,
                    Valor_Inicial,
                    sizeof(Valor_Inicial),
                    4,
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
                );

                
                Tiempo_Adquisicion = new GattCharacteristic( // TIEMPO ADQUISISCION
                    Valor_Inicial,
                    Valor_Inicial,
                    sizeof(Valor_Inicial),
                    2,
                    GattCharacteristic:: BLE_GATT_CHAR_PROPERTIES_WRITE
                );
                Umbral_Intensidad = new GattCharacteristic( // UMBRAL INTENSIDAD
                    caracteristica3UUID,
                    Valor_Inicial,
                    sizeof(Valor_Inicial),
                    1,
                    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE 
                );
             
                GattCharacteristic* caracteristicas[] = {Intensidad, Tiempo_Adquisicion, Umbral_Intensidad};
            
                Servicio = new GattService(
                    servicioUUID,
                    caracteristicas,
                    sizeof(caracteristicas) / sizeof(GattCharacteristic*)
                );

                // Añadir el servicio al servidor GATT 
                if (ble.gattServer().addService(*Servicio) == 0) {
                   // printf("Servicio 2 añadido\n");
                }
                
            }
        
        public:
            GattCharacteristic* Intensidad;
            GattCharacteristic* Tiempo_Adquisicion;
            GattCharacteristic* Umbral_Intensidad;
        private:
            BLE& ble;

            // Definir UUIDs para el servicio y las características
            const UUID servicioUUID;
            const UUID caracteristica1UUID;
            const UUID caracteristica2UUID;
            const UUID caracteristica3UUID;
            GattService* Servicio;
            uint8_t Valor_Inicial[1] = {0};       
            
        };

// CLASE CON LOS PARAMETROS DE LA PERSONA 
class Persona : public GattServer::EventHandler {
    public:
        Persona(BLE& bleInstance) :
            ble(bleInstance),
            servicioUUID(0x0010),
            alturaUUID(0x0011),
            pesoUUID(0x0012),
            edadUUID(0x0013),
            sexoUUID(0x0014),
            umbralUUID(0x0015),

            Altura(nullptr),
            Peso(nullptr),
            Edad(nullptr),
            Sexo(nullptr),
            Umbral(nullptr),
            Servicio(nullptr)
        {

            Altura = new GattCharacteristic(
                alturaUUID,
                ValorInicial,
                sizeof(ValorInicial),
                3,
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | 
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE 
            );


            Peso = new GattCharacteristic(
                pesoUUID,
                ValorInicial,
                sizeof(ValorInicial),
                3,
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | 
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE 
            );
            Edad = new GattCharacteristic(
                edadUUID,
                ValorInicial,
                sizeof(ValorInicial),
                2,
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | 
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE 
            );


            Sexo = new GattCharacteristic(
                sexoUUID,
                ValorInicial,
                sizeof(ValorInicial),
                1,
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | 
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE 
            );
            Umbral = new GattCharacteristic(
                umbralUUID,
                ValorInicial,
                sizeof(ValorInicial),
                6,
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | 
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE 
            );

            // Crear un array de GattCharacteristic
            GattCharacteristic* caracteristicas[] = { Altura, Peso, Edad, Sexo, Umbral };

            // Crear la instancia de GattService
            Servicio = new GattService(
                servicioUUID,
                caracteristicas,
                sizeof(caracteristicas) / sizeof(GattCharacteristic*)
            );

            // Añadir el servicio al servidor GATT después de haberse creado
            if (ble.gattServer().addService(*Servicio) == 0) {
                //printf("Servicio persona añadido\n");
            }

        }

    public:
        GattCharacteristic* Altura;
        GattCharacteristic* Peso;
        GattCharacteristic* Edad;
        GattCharacteristic* Sexo;
        GattCharacteristic* Umbral;
       
        // ESTRUCTURA PERSONA , DEDICADA  A MANEJAR EL HISTORIAL SIN RECURIR A PAETICIONES DE LECTURA DEL SERVIDOR-->HANDLER PERSONALIZADO
        struct DatosPersona {  
                uint16_t Altura=180;
                uint8_t Edad=21;
                uint8_t Sexo='m';  // Datos iniciales de la persona, antes de introducir datos , a modo de tener unos parametrso de referencia si el usuario no los escribe desde el servicio.
                uint8_t Peso=60;
                uint16_t Umbral_Cal;
                uint8_t TiempoAdq=5;
                uint8_t Umbral_Int=1;
            };
        DatosPersona Datos;    
    private:
       
        BLE& ble;
        const UUID servicioUUID;
        const UUID alturaUUID;
        const UUID pesoUUID;
        const UUID edadUUID;
        const UUID sexoUUID;
        const UUID umbralUUID;

        uint8_t ValorInicial[1] = { 0 };
        GattService* Servicio;
    };

// CLASE MANEJADORA DE EVENTOS BLE
class EventHandler : public GattServer::EventHandler {
    
    public:
        EventHandler(BLE& bleInstance, EventQueue& queue, ServicioMonitoreo &servicio,  ServicioActividad &servicio2, Persona &ServicioP, Historial &historia ):
            ble(bleInstance),
            eventQueue(queue), 
            Monitoreo(servicio),
            Actividad(servicio2),
            Histo(historia),
            Per(ServicioP)
            
        {
            ble.gattServer().setEventHandler(this); // HACEMOS DE ESTA CLASE LA MANEJADORA DE EVENTOS DEL SERVIDOR, GESTIONAMOS CON CALLBACKS DE EVENTOS
            
        }
             // Las siguientes funciones son callbacks asincronos, al recibir una peticion, tanto WR como Updates on/off -->Servirá para comunicarnos con el servidor desde el micro.
            // La funcion Write actualiza el valor de una caracteristica. -->Existe su analoga Read, pero no la usaremos en este proyecto.
        
            virtual void onDataRead(const GattReadCallbackParams &params) override {
                
                if (params.handle == Histo.Histor->getValueHandle()) {

                    /*for (uint8_t i = 0; i < indice; ++i) { // ESTE SERIA EL BLOQUE DE DATOS QUE DEBERIA LEER EL HISTORICO DE LAS ULTIMAS X HORAS 
                        printf("Entrada numero : %d\n", i + 1);
                        printf("Start: %s", ctime(&Array_Histo[i].Start));
                        printf("End: %s", ctime(&Array_Histo[i].End));
                        printf("Mode: %s\n", (Array_Histo[i].Mode ? "Monitoreo" : "Actividad"));
                        printf("Cal_Total: %d\n", Array_Histo[i].Cal_Total);
                        printf("------------------------\n");
                        
                    }   if (indice =23){ // si llenamos todos los espacios de memoria del array.... 
                            indice=0;
                        }*/
                    //Vamos a crear un interprete entre el array de la estructura y la funcion write , aunque desconfio de que vaya a funcionar correctamente

                        for (uint8_t i = 0; i < indice; ++i) {
                        
                         uint8_t *Struct_a_Bytes = reinterpret_cast<uint8_t*>(&Array_Histo[i]); // Faltaria implementar la reestructuracion de datos, desde cliente.

                        // Escribir en la característica BLE
                         error = ble.gattServer().write(
                                                    Histo.Histor->getValueHandle(),
                                                    Struct_a_Bytes,
                                                    sizeof(Histo),
                                                    false
                                                    );
                        }                                                         
               
               } else if (params.handle == Histo.Objetivo->getValueHandle()) {
                   
                   uint8_t objetivo=Calorias_Diarias(); // Devuelve 1 si lo ha cumplido
                   error = ble.gattServer().write(Histo.Objetivo->getValueHandle(),
                                                    &objetivo,
                                                    sizeof(objetivo));


               }
            };


            virtual void onDataWritten(const GattWriteCallbackParams &params) override {
            
                std::vector<uint8_t> buffer(params.data, params.data + params.len);

                // RELACIONADO CON SERVICIO PERSONA
                if (params.handle == Per.Edad->getValueHandle()) {
                    int8_t valorNumerico = 0;
                    for (size_t i = 0; i < buffer.size(); ++i) {
                        if (buffer[i] >= '0' && buffer[i] <= '9') {
                            valorNumerico = valorNumerico * 10 + (buffer[i] - '0');
                        } else {
                            std::cerr << "Error: Se recibió un caracter no numérico\n";
                            return;
                        }
                    }
                    Per.Datos.Edad = valorNumerico;

                } else if (params.handle  == Per.Peso->getValueHandle()) {
                    int8_t valorNumerico = 0;
                    for (size_t i = 0; i < buffer.size(); ++i) {
                        if (buffer[i] >= '0' && buffer[i] <= '9') {
                            valorNumerico = valorNumerico * 10 + (buffer[i] - '0');
                        } else {
                            //std::cerr << "Error: Se recibió un caracter no numérico\n";
                            return;
                        }
                    }
                    Per.Datos.Peso = valorNumerico;
                    error = ble.gattServer().write(Per.Peso->getValueHandle(),
                                                    &Per.Datos.Peso,
                                                    sizeof(Per.Datos.Peso));

                } else if (params.handle  == Per.Altura->getValueHandle()) {
                    uint16_t valorNumerico = 0;

                    for (size_t i = 0; i < buffer.size(); ++i) {
                        if (buffer[i] >= '0' && buffer[i] <= '9') {
                            valorNumerico = valorNumerico * 10 + (buffer[i] - '0');
                        } else {
                            //std::cerr << "Error: Se recibió un caracter no numérico\n";
                            return;
                        }
                    }
                    Per.Datos.Altura = valorNumerico;
                    error = ble.gattServer().write(Per.Umbral->getValueHandle(),
                                    reinterpret_cast<const uint8_t*>(&Per.Datos.Altura),
                                    sizeof(Per.Datos.Altura),
                                    false);

                } else if (params.handle  == Per.Sexo->getValueHandle()) { // Se introduce M o F
                    if (buffer.size() == 1) {
                        Per.Datos.Sexo = buffer[0];
                        error = ble.gattServer().write(Per.Sexo->getValueHandle(),
                                    &Per.Datos.Sexo,
                                    sizeof(Per.Datos.Sexo),
                                    false);
              
                    };
                } else if ( params.handle == Per.Umbral->getValueHandle()){
                                    
                    uint16_t valorNumerico = 0;
                    for (size_t i = 0; i < buffer.size(); ++i) {
                        if (buffer[i] >= '0' && buffer[i] <= '9') {
                            valorNumerico = valorNumerico * 10 + (buffer[i] - '0');
                        } else {
                            //std::cerr << "Error: Se recibió un caracter no numérico\n";
                            return;
                        }
                    }

                    Per.Datos.Umbral_Cal = valorNumerico;
                    error = ble.gattServer().write(Per.Umbral->getValueHandle(),
                                    reinterpret_cast<const uint8_t*>(&Per.Datos.Umbral_Cal),
                                    sizeof(Per.Datos.Umbral_Cal),
                                    false);
            
                // RELACIONADO CON SERVICIO ACTIVIDAD
                } else if ( params.handle == Actividad.Tiempo_Adquisicion->getValueHandle() ){
                            
                    uint8_t valorNumerico = 0;
                    for (size_t i = 0; i < buffer.size(); ++i) {
                        if (buffer[i] >= '0' && buffer[i] <= '9') {
                            valorNumerico = valorNumerico * 10 + (buffer[i] - '0');
                        } else {
                            std::cerr << "Error: Se recibió un caracter no numérico\n";
                            return;
                        }
                    }
                    Per.Datos.TiempoAdq = valorNumerico;
                    error = ble.gattServer().write(Actividad.Tiempo_Adquisicion->getValueHandle(),
                                    reinterpret_cast<const uint8_t*>(&Per.Datos.TiempoAdq),
                                    sizeof(Per.Datos.TiempoAdq),
                                    false);
                                    
                } else if (params.handle == Actividad.Umbral_Intensidad->getValueHandle()) {
                    printf("Umbral de intensidad mandado en asci\n");
                    int8_t valorNumerico = 0;
                    for (size_t i = 0; i < buffer.size(); ++i) {
                        if (buffer[i] >= '0' && buffer[i] <= '9') {
                            valorNumerico = valorNumerico * 10 + (buffer[i] - '0');
                        }    
                    }

                    Per.Datos.Umbral_Int = valorNumerico;
                    error = ble.gattServer().write(
                        Actividad.Umbral_Intensidad->getValueHandle(),
                        &Per.Datos.Umbral_Int,
                        sizeof(Per.Datos.Umbral_Int)
                    );
                }

            }

            virtual void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params) override {
                
                // SI ACTIVAS EL MODO MONITOREO...
                if (params.charHandle == Monitoreo.Caracteristica1->getValueHandle()) {             
                    
                    start_time = getCurrentTime(); // Esta funcion devuelve la hora del RTC
                    eventQueue.call_every(std::chrono::seconds(6), callback(this, &EventHandler::Cal_60s)); // Envio continuo cada 60s, mientras notificacioes activadas
                            // Se utiliza para un envio periodico, cada 60s
                    
                } else if (params.charHandle == Actividad.Intensidad->getValueHandle()) {
                    
                    start_time = getCurrentTime();
                    std::chrono::seconds duracion(Per.Datos.TiempoAdq); // Convertimos er.Datos.TiempoAdq a minutos
                    eventQueue.call_in(duracion, callback(this, &EventHandler::Calc_Intensidad)); //Enviaremos la intensidad durante ese perido de adquisición del ejercicio, una vez transcurra el tiempo establecido
                            // Se utiliza para un envio puntual, al finalizar el ejercicio
                }
            }
            
            virtual void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params) override{
           
                if (params.charHandle == Monitoreo.Caracteristica1->getValueHandle()) {
                    
                    end_time = getCurrentTime(); // Registrar la hora de finalización
                    /*printf("Inicio de notificación:%s",ctime(&start_time) );
                    printf("Fin de notificación: %s", ctime(&end_time) );
                    printf("Calorías totales quemadas: %d\n", Calorias_Acumuladas_Monitoreo);*/
                    
                    //Almacenamos en el historico
                    //Array_Histo[indice].Start= start_time; -->Sería un buen trabajo futuro implementarla 
                    //Array_Histo[indice].End= end_time; -->Sería un buen trabajo futuro implementarla 
                    Array_Histo[indice].Mode= true;
                    Array_Histo[indice].Cal_Total=Calorias_Acumuladas_Monitoreo ;
                    indice++;
                    

                    Calorias_Totales += Calorias_Acumuladas_Monitoreo; //ALMACENO LAS CALORIAS TOTALES 
                    Calorias_Acumuladas_Monitoreo = 0 ;
                
                } else if (params.charHandle == Actividad.Intensidad->getValueHandle()) {
                    
                    end_time = getCurrentTime(); // Registrar la hora de finalización
                    /*printf("Inicio de notificación:%s",ctime(&start_time) );
                    printf("Fin de notificación: %s", ctime(&end_time) );
                    printf("Calorías totales quemadas: %d\n", Calorias_Acumuladas_Actividad);*/
                
                    //Almacenamos en el historico
                    //Array_Histo[indice].Start= start_time; -->Sería un buen trabajo futuro implementarla 
                    //Array_Histo[indice].End= end_time; -->Sería un buen trabajo futuro implementarla 
                    Array_Histo[indice].Mode= false;
                    Array_Histo[indice].Cal_Total=Calorias_Acumuladas_Actividad ;
                    indice++;
                    

                    Calorias_Totales +=Calorias_Acumuladas_Actividad;
                    Calorias_Acumuladas_Actividad = 0 ;
                };                      
            }

        //FUNCIONES PARA EL CORRECTO USO DE LOS SERVICIOS
       
        void Calc_Intensidad() { // Se encargara de enviar que intensidad se ha llevado a cabo durante la actividad.
            
            // Se trabaja con doubles , ya que hay fracciones y tenemos espacio en memoria , sino , buscariamos otras altenativas menos costosas en recursos
            uint16_t pasos;
            sensor.get_step_counter(&pasos);
            Total_Pasos += pasos;
            uint8_t tiempoDuracion = Per.Datos.TiempoAdq;  // Proporciona el tiempo de duración 
            const double Zancada = Per.Datos.Altura * 0.5;  // Distancia constante en metros por zancada, la mitad de la altura
            uint8_t Intensidad=0;
            // Parámetros personalizables pero sirven para la demostracion del funcionamiento
            double Intensidad_Establecida = Per.Datos.Umbral_Int;
            double umbralMuyBajaIntensidad = Intensidad_Establecida * 0.45;
            double umbralBajaIntensidad = Intensidad_Establecida * 0.55;
            double umbralMediaIntensidad = Intensidad_Establecida * 0.75;
            double umbralAltaIntensidad = Intensidad_Establecida * 0.95;
          
            
            
            // Calcular el ritmo
            auto ritmo = static_cast<double>(pasos) / tiempoDuracion;  // Zancadas por segundo
            // Calcular las calorías consumidas
            double caloriasConsumidas = CaloriasxRitmo(ritmo);
            Calorias_Acumuladas_Actividad += caloriasConsumidas;

                                                                 // Calcular la velocidad promedio
            double distanciaTotal = pasos * Zancada;            // Este calculo no lo he necestiado, pero podría usarse en un futuro.
            double tiempoTotal = tiempoDuracion * 60;          // Este calculo no lo he necestiado, pero podría usarse en un futuro.
            double velocidad = distanciaTotal / tiempoTotal;  // Este calculo no lo he necestiado, pero podría usarse en un futuro.
            

            // Determinar la intensidad del ejercicio
            if (ritmo > umbralAltaIntensidad ) {
                Intensidad = 3;
                //cout << "La intensidad del ejercicio es muy alta." << endl;
            } else if (ritmo > umbralMediaIntensidad ) {
                Intensidad = 2;
                //cout << "La intensidad del ejercicio es alta." << endl;
            } else if (ritmo > umbralBajaIntensidad ) {
                Intensidad = 1;
                //cout << "La intensidad del ejercicio es media." << endl;
            } else {
                Intensidad = 0;
                //cout << "La intensidad del ejercicio es baja." << endl;
            }

            error = ble.gattServer().write(Actividad.Intensidad->getValueHandle(),
                                    &Intensidad,
                                    sizeof(Intensidad),
                                    false);
            sensor.reset_step_counter();

        }

        void  Cal_60s() { // Se encargará de enviar las calorias consumidas en 60s del modo Monitorizacion
                
                bool updatesEnabled = false;
                if (ble.gattServer().areUpdatesEnabled(*Monitoreo.Caracteristica1, &updatesEnabled) == BLE_ERROR_NONE ){
                    if (updatesEnabled==true) {
                        
                        uint16_t pasos;
                        sensor.get_step_counter(&pasos);
                        Total_Pasos +=pasos; 
                        // Calcular las calorías usando la función CaloriasxRitmo
                        uint8_t calorias = CaloriasxRitmo(pasos);
                        Calorias_Acumuladas_Monitoreo += calorias;
                        // Enviar las calorías a través de BLE
                        error = ble.gattServer().write(Monitoreo.Caracteristica1->getValueHandle(),
                                                    &calorias,
                                                    sizeof(calorias),
                                                    false);
                        sensor.reset_step_counter();
                    }
                }      
        }

        uint8_t CaloriasxRitmo(double ritmo) { // Es la relacion entre las calorias y el ritmo
           
            const double factorCalorias = 0.75;  // APROXIMACIÓN.
            double resultado = ritmo * factorCalorias;
            uint8_t res= static_cast<uint8_t>(resultado);
            return res;
        }

        uint8_t Calorias_Diarias() { // Comprobacion de si se ha cumplido el objetivo de calorias diarias
            
            // Verifica si la suma total es mayor que el umbral establecido por el usuario de calorias diarias
            if (Calorias_Totales >= Per.Datos.Umbral_Cal) {
                return 1;
            }
            else return 0; 
        }
    
    private:
 
        uint8_t dataToSend; // BYTE QUE SE ENVIA AL SERVIDOR
        uint16_t Calorias_Acumuladas_Monitoreo=0; // PARA CREAR EL HISTORIAL...
        uint16_t Calorias_Acumuladas_Actividad=0;
        uint16_t Calorias_Totales=0; // SERVIRÁ PARA EL AVISO DE CALORIAS DIARIAS
        uint16_t Total_Pasos = 0;   // SE PODRÍA IMPLEMENTAR EN UN FUTURO
        time_t end_time;    // REGISTROS DE TIEMPO DE CADA SERVICIO -->Sería un buen trabajo futuro implementarla 
        time_t start_time; //-->Sería un buen trabajo futuro implementarla 
        uint8_t indice=0; // indice del historial

        BLE& ble;
        ble_error_t error;
        EventQueue& eventQueue;
        std::vector<uint8_t> buffer;
        ServicioMonitoreo &Monitoreo;
        ServicioActividad &Actividad;
        Persona &Per;
        Historial &Histo; 

    };


