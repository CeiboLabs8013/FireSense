#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

#define SS 5
#define RST 14
#define DIO0 26

// WiFi
const char* ssid = "AlbertEinstein";
const char* password = "Faraday80132024";
WebServer server(80);

String webNodeId = "--";
String webTemp = "--";
String webHum = "--";
String webLat = "--";
String webLon = "--";
String webRisk = "--";
String webRSSI = "--";
String webSnR = "--";
// API
const char* serverUrl = "http://TU_SERVIDOR/api/data";
void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta http-equiv="refresh" content="2">
<title>FireSense</title>

<style>
body{
font-family:Arial;
background:#111;
color:white;
text-align:center;
padding-top:30px;
}

.card{
background:#222;
width:300px;
margin:auto;
padding:20px;
border-radius:15px;
}

h1{
color:#ff4d4d;
}
</style>

</head>

<body>

<h1>FireSense</h1>

<div class="card">

<h2>Nodo )rawliteral";

  html += webNodeId;

  html += R"rawliteral(</h2>

<p>🌡 Temperatura: )rawliteral";

  html += webTemp;

  html += R"rawliteral( °C</p>

<p>💧 Humedad: )rawliteral";

  html += webHum;

  html += R"rawliteral( %</p>

  <p>📍 Ubicacion: )rawliteral";

  html += webLat + " " + webLon;

  html += R"rawliteral(</p>

<p>📶 RSSI: )rawliteral";

  html += webRSSI;

  html += R"rawliteral( dBm</p>
  
  <p>📶 SNR: )rawliteral";

  html += webSnR;

  html += R"rawliteral( dBm</p>

<p>🔥 Riesgo: )rawliteral";

  html += webRisk;

  html += R"rawliteral( %</p>

</div>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}
void setup() {
  Serial.begin(115200);

  // LoRa
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("Error iniciando LoRa");
    while (true)
      ;
  }
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);

  Serial.println("LoRa iniciado");

  // WiFi
  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  server.on("/", handleRoot);
  server.begin();

  Serial.println("Servidor Web iniciado");

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  int packetSize = LoRa.parsePacket();

  if (packetSize) {

    String packet = "";

    while (LoRa.available()) {
      packet += (char)LoRa.read();
    }

    Serial.println("Paquete recibido:");
    Serial.println(packet);

    // Formato:
    // ID,TEMP,HUM,WIND,DIR,LAT,LON

    int index1 = packet.indexOf(',');
    int index2 = packet.indexOf(',', index1 + 1);
    int index3 = packet.indexOf(',', index2 + 1);
    int index4 = packet.indexOf(',', index3 + 1);
    //int index5 = packet.indexOf(',', index4 + 1);
    //int index6 = packet.indexOf(',', index5 + 1);

    int nodeId = packet.substring(0, index1).toInt();

    float temp = packet.substring(index1 + 1, index2).toFloat();
    float hum = packet.substring(index2 + 1, index3).toFloat();
    float lat = packet.substring(index3 + 1, index4).toFloat();
    float lon = packet.substring(index4 + 1).toFloat();
    //float lat = packet.substring(index5 + 1, index6).toFloat();
    //float lon = packet.substring(index6 + 1).toFloat();

    // RSSI recibido
    int rssi = LoRa.packetRssi();
    int SnR = LoRa.packetSnr();

    // Riesgo FireSense V1
    int risk = 0;

    if (temp > 35) risk += 20;
    if (temp > 40) risk += 20;

    if (hum < 40) risk += 20;
    if (hum < 20) risk += 20;



    if (risk > 100)
      risk = 100;
    webNodeId = String(nodeId);
    webTemp = String(temp, 1);
    webHum = String(hum, 1);
    webLat = String(lat, 6);
    webLon = String(lon, 6);
    webRSSI = String(rssi);
    webSnR = String(SnR);
    webRisk = String(risk);
    Serial.println("====== DATOS ======");
    Serial.print("Nodo: ");
    Serial.println(nodeId);

    Serial.print("Temp: ");
    Serial.println(temp);

    Serial.print("Humedad: ");
    Serial.println(hum);

    Serial.print("Ubicacion: ");
    Serial.print(lat);
     Serial.println(" ");
    Serial.println(lon);

    Serial.print("RSSI: ");
    Serial.println(rssi);

    Serial.print("SnR: ");
    Serial.println(SnR);

    Serial.print("Riesgo: ");
    Serial.print(risk);
    Serial.println("%");

    // Enviar a servidor
    if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;

      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String json =
        "{"
        "\"nodeId\":"
        + String(nodeId) + "," + "\"temperature\":" + String(temp, 2) + "," + "\"humidity\":" + String(hum, 2) + "," + "\"latitud\":" + String(lat, 7) + "," + "\"longitud\":" + String(lon, 7) + "," + "\"rssi\":" + String(rssi) + ","+ "\"SnR\":" + String(SnR) + "," + "\"risk\":" + String(risk) + "}";

      int responseCode = http.POST(json);

      Serial.print("HTTP Response: ");
      Serial.println(responseCode);

      http.end();
    }

    Serial.println("==================");
    Serial.println();
  }
}