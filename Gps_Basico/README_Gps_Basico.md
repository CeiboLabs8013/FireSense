# Uso del GPS NEO-6M con ESP32

Tutorial para leer datos de un módulo GPS NEO-6M-V2 desde una placa ESP32 utilizando la librería **TinyGPSPlus**, procesando las sentencias NMEA y mostrando latitud/longitud por el monitor serie.

## Requisitos

- Placa ESP32
- Módulo GPS NEO-6M-V2
- Arduino IDE

## Instalación de la librería

Este proyecto depende de la librería:

- **Nombre:** TinyGPSPlus-ESP32
- **Autor:** Mikal Hart

Pasos para instalarla:

1. Abre el IDE de Arduino.
2. Ve a **Herramientas > Administrador de librerías**.
3. En la barra de búsqueda escribe `TinyGPSPlus`.
4. Busca la librería desarrollada por Mikal Hart y haz clic en **Instalar**.

## Conexiones físicas

| Módulo GPS NEO-6M | Placa ESP32 |
|---|---|
| VCC | VIN o 5V (o 3.3V según tu módulo) |
| GND | GND (Tierra) |
| TX | GPIO 16 (RX2 del ESP32) |
| RX | GPIO 17 (TX2 del ESP32) |

> **Nota importante:** las conexiones de datos van cruzadas (TX del GPS al RX del ESP32).

## Descripción del funcionamiento

El programa lee los datos crudos (sentencias NMEA) que emite el GPS por un puerto serial de hardware, los procesa con la librería TinyGPSPlus y muestra la latitud y longitud por el monitor serie con 6 decimales de precisión.

- Se usa el **UART2** del ESP32 (pines GPIO 16 y GPIO 17) para comunicarse con el módulo GPS.
- El módulo NEO-6M transmite por defecto a **9600 baudios**.
- El monitor serie del ESP32 se inicializa a **115200 baudios**.
- Las coordenadas se guardan como `double` (64 bits) para mayor precisión que un `float` (32 bits).
- Cada vez que hay una posición nueva y válida (`gps.location.isUpdated()`), se imprime la latitud y longitud.
- Las coordenadas se concatenan en un `String` (`"lat,lon"`) pensado para enviarse luego por HTTP o LoRa.

## Explicación del código (`.ino`)

### A. Inclusión de librerías y definición de pines

- `#include <TinyGPSPlus.h>` incorpora la librería que decodifica las sentencias NMEA.
- `RXD2` (GPIO 16) y `TXD2` (GPIO 17) son los pines físicos usados para la comunicación UART secundaria del ESP32.
- `GPS_BAUD` se fija en 9600, la velocidad estándar del módulo NEO-6M.
- `HardwareSerial gpsSerial(2)` crea una instancia de comunicación serial por hardware usando el **UART2** del ESP32.
- `TinyGPSPlus gps` es el objeto principal de la librería, encargado de decodificar las tramas NMEA que envía el GPS.

### B. Configuración inicial (`setup()`)

- `Serial.begin(115200)` inicializa el monitor serie (comunicación con la PC).
- `gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2)` inicializa el UART2 a 9600 baudios, con formato **8N1** (8 bits de datos, sin paridad, 1 bit de parada), asignando RX al pin 16 y TX al pin 17.

### C. Bucle principal (`loop()`)

- Se declaran `lat` y `lon` como `double`, ya que reservan 64 bits de memoria y son más precisas que un `float` (32 bits).
- Mientras haya bytes disponibles en `gpsSerial`, se leen carácter por carácter (`gpsSerial.read()`) y se alimentan al objeto `gps` con `gps.encode(c)`. La librería va armando las sentencias NMEA hasta completarlas.
- `gps.location.isUpdated()` evalúa si el GPS adquirió suficientes satélites y calculó una nueva posición válida. Si es así:
  - `gps.location.lat()` y `gps.location.lng()` obtienen latitud y longitud en doble precisión.
  - `Serial.println(lat, 6)` / `Serial.println(lon, 6)` las imprimen forzando 6 decimales de resolución.
- Al final del `loop()`, se arma un `String mensaje` uniendo latitud y longitud separadas por una coma, pensado para enviarse más adelante por HTTP o LoRa.
- `delay(2000)` espera 2 segundos antes de repetir el ciclo.

> **Nota:** en el archivo original había una llave `}` sobrante al final del `loop()`, que impedía que el código compilara. Si tu `.ino` no compila, revisá que no tenga esa llave de más.

## Posibles mejoras futuras

- Enviar el `mensaje` por HTTP a un servidor.
- Enviar el `mensaje` por LoRa a otro nodo.
- Validar `lat`/`lon` antes de enviarlos, ya que al inicio (antes del primer fix del GPS) pueden estar sin inicializar.
