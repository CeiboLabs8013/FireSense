#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal.h>
#define SS 5
#define RST 14
#define DIO0 26
const int rs = 32, en = 33, d4 = 15, d5 = 4, d6 = 16, d7 = 17;
LiquidCrystal lcd(25, 33, 27, 21, 16, 17);
void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(13, OUTPUT);
  SPI.begin(18, 19, 23, SS);
  delay(1000);
  lcd.begin(16, 1);

  lcd.setCursor(0, 0);
  lcd.print("Linea 1");

  lcd.setCursor(0, 1);
  lcd.print("Linea 2");
  LoRa.setPins(SS, RST, DIO0);

  Serial.println("Iniciando LoRa...");

  if (!LoRa.begin(433E6)) {
    Serial.println("FALLO");
    while (1)
      ;
  }
    LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("LoRa OK");
}

void loop() {

  int packetSize = LoRa.parsePacket();

  if (packetSize) {

    String incoming = "";
    String LED = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }
    LED = incoming;
    if (LED == "1") {
      digitalWrite(13, HIGH);
      delay(1000);
      LED = "0";
    }
    if (LED == "0") {
      digitalWrite(13, LOW);
    }
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("RSSI:");
    lcd.print(LoRa.packetRssi());
    lcd.setCursor(9, 1);
    lcd.print("SNR:");
    lcd.print(LoRa.packetSnr());
  }
}