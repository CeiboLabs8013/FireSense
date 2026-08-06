// A. Inclusión de librerías y definición de pines
#include <TinyGPSPlus.h>

// Pines físicos GPIO 16 y GPIO 17 del ESP32 para la comunicación UART secundaria.
#define RXD2 16
#define TXD2 17

// Velocidad de transmisión estándar (baud rate) del módulo NEO-6M: 9600 baudios.
#define GPS_BAUD 9600

// Instancia de comunicación serial por hardware usando el UART2 del ESP32.
HardwareSerial gpsSerial(2);

// Objeto principal de TinyGPSPlus, encargado de decodificar las tramas NMEA.
TinyGPSPlus gps;

// B. Configuración inicial
void setup() {
  // Inicializa el monitor serie a 115200 baudios.
  Serial.begin(115200);
  // Inicializa el UART2 a 9600 baudios, formato 8N1, RX en pin 16 y TX en pin 17.
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
}

// C. Bucle principal
void loop() {
  // Las variables double son más precisas: reservan 64 bits, en vez de los 32 bits de un float.
  double lat;
  double lon;

  // Revisa si hay bytes disponibles en el buffer del gpsSerial y los alimenta al objeto gps.
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);

    // Evalúa si el GPS calculó una nueva posición válida.
    if (gps.location.isUpdated()) {
      Serial.print("Latitud: ");
      lat = gps.location.lat();
      Serial.println(lat, 6);

      Serial.print("Longitud: ");
      lon = gps.location.lng();
      Serial.println(lon, 6);
    }
  }

  // Se arma un mensaje "lat,lon" para poder enviarlo luego por HTTP o LoRa.
  String mensaje = String(lat, 6) + "," + String(lon, 6);

  Serial.print("Enviado: ");
  Serial.println(mensaje);

  delay(2000);
}