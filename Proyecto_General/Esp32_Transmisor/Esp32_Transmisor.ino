#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal.h>
#include <DHTesp.h>
#include <TinyGPSPlus.h>
#define DHT_PIN 32
#define SS 5
#define RST 14
#define DIO0 26
#define RXD2 16
#define TXD2 17

#define GPS_BAUD 9600
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;
DHTesp dht;
const int NODE_ID = 1;
void setup() {
  Serial.begin(115200);
  delay(1000);

  SPI.begin(18, 19, 23, SS);
  LoRa.setPins(SS, RST, DIO0);
  //Serial.println("Iniciando LoRa...");

  if (!LoRa.begin(433E6)) {
    Serial.println("FALLO");
    while (1)
      ;
  }
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  Serial.println("LoRa OK");
  dht.setup(DHT_PIN, DHTesp::DHT22);
  delay(3000);
  Serial.println("Iniciado");
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial 2 started at 9600 baud rate");
}



void loop() {

  TempAndHumidity data = dht.getTempAndHumidity();

  float temperatura = data.temperature;
  float humedad = data.humidity;
  double lon;
  double lat;
  // Verificar lectura válida
  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Error leyendo DHT22");
    delay(2000);
    return;
  }
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();

    gps.encode(c);

    if (gps.location.isUpdated()) {

      Serial.print("Latitud: ");
      lat = gps.location.lat();
      Serial.println(lat, 6);
       


      Serial.print("Longitud: ");
      lon = gps.location.lng();
      Serial.println(lon, 6);
       
    }
  }
  String mensaje =
    String(NODE_ID) + "," + String(temperatura, 1) + "," + String(humedad, 1) + "," + String(lat, 6) + "," + String(lon, 6);

  LoRa.beginPacket();
  LoRa.print(mensaje);
  LoRa.endPacket();


  //Serial.println("Mensaje enviado:1");
  Serial.print("Enviado: ");
  Serial.println(mensaje);

  delay(2000);
}
