#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal.h>
#include <DHTesp.h>

#define DHT_PIN 8
#define SS 5
#define RST 14
#define DIO0 26

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
  //Serial.println("LoRa OK");
  dht.setup(DHT_PIN, DHTesp::DHT22);
  delay(3000);
  Serial.println("Iniciado");
}



void loop() {

  LoRa.beginPacket();
  LoRa.print("1");
  LoRa.endPacket();

  //Serial.println("Mensaje enviado:1");

  delay(2000);
}
