# FireSense - Nodo remoto con DHT22 + GPS (LoRa) y servidor web

Esta es una evolución del [proyecto básico de comunicación LoRa](../lora-basico/README.md) *(ajustá el enlace según la carpeta real dentro del repo)*. Se mantiene el mismo esquema transmisor/receptor por LoRa, pero ahora:

- El **transmisor** ya no manda un `"1"` fijo: lee un sensor de **temperatura y humedad (DHT22)** y un **módulo GPS**, y arma con esos datos un mensaje con formato de texto separado por comas.
- El **receptor** ya no usa LED ni LCD: recibe el paquete LoRa, calcula un **índice de riesgo** simple, y expone toda la información en una **página web** (servida desde el propio ESP32 por WiFi), además de reenviar los datos por HTTP a un servidor externo (API).

> ℹ️ El GPS y su librería (`TinyGPSPlus`) ya están explicados en detalle en [`Tutorial uso del GPS con esp32.txt`](./Tutorial%20uso%20del%20GPS%20con%20esp32.txt) *(ajustá el nombre/ruta según cómo lo subas al repo)*, así que acá no se repite esa parte — solo se indica cómo se conecta.

---

## Índice

1. [Qué cambia respecto al proyecto básico](#qué-cambia-respecto-al-proyecto-básico)
2. [Materiales adicionales](#materiales-adicionales)
3. [Librerías adicionales](#librerías-adicionales)
4. [Conexionado](#conexionado)
5. [Explicación del código - Transmisor](#explicación-del-código---transmisor)
6. [Explicación del código - Receptor / Servidor web](#explicación-del-código---receptor--servidor-web)
7. [Cómo probarlo](#cómo-probarlo)
8. [Problemas conocidos / a mejorar](#problemas-conocidos--a-mejorar)

---

## Qué cambia respecto al proyecto básico

| | Proyecto básico | Este proyecto (FireSense) |
|---|---|---|
| Transmisor envía | `"1"` fijo | `nodeId,temperatura,humedad,lat,lon` (datos reales) |
| Sensor | — | DHT22 + GPS |
| Receptor muestra | LED + LCD | Página web (WiFi) + reenvío HTTP a un servidor |
| Conectividad extra | — | WiFi (`WiFi.h`) + servidor HTTP embebido (`WebServer.h`) + cliente HTTP (`HTTPClient.h`) |

El conexionado del módulo **LoRa Ra-02 (SPI)** es el mismo que en el proyecto básico (pines `SCK 18 / MISO 19 / MOSI 23 / SS 5 / RST 14 / DIO0 26`, alimentado a 3.3V) — ver el README del proyecto base para el detalle. Acá se documenta solo lo nuevo.

---

## Materiales adicionales

- 1x sensor **DHT22** (para el transmisor)
- 1x módulo **GPS NEO-6M** compatible con UART (para el transmisor) — su conexionado y funcionamiento detallado están en [`Tutorial uso del GPS con esp32.txt`](./Tutorial%20uso%20del%20GPS%20con%20esp32.txt)
- Red WiFi disponible (2.4 GHz) para que el receptor pueda conectarse y levantar el servidor web

En este caso el receptor **ya no lleva LED ni LCD**.

---

## Librerías adicionales

Además de `SPI` y `LoRa` (ya usadas en el proyecto básico), instalar desde el Gestor de Bibliotecas del Arduino IDE:

- **DHT sensor library for ESPx** (`DHTesp.h`)
- **TinyGPSPlus-ESP32**, de *Mikal Hart* (`TinyGPSPlus.h`) — instalar desde `Herramientas > Administrador de librerías`, buscando "TinyGPSPlus"
- `WiFi.h`, `HTTPClient.h` y `WebServer.h` ya vienen incluidas con el core de ESP32, no requieren instalación aparte.

---

## Conexionado

### Transmisor

| Componente | Pin del componente | Pin del ESP32 |
|---|---|---|
| LoRa Ra-02 | VCC / GND / SCK / MISO / MOSI / NSS / RST / DIO0 | Igual que en el proyecto básico (3.3V, GND, 18, 19, 23, 5, 14, 26) |
| DHT22 | Data | GPIO 32 |
| DHT22 | VCC | 3.3V |
| DHT22 | GND | GND |
| GPS NEO-6M | VCC | VIN / 5V (o 3.3V, según el módulo) |
| GPS NEO-6M | GND | GND |
| GPS NEO-6M | TX | GPIO 16 (`RXD2`, entrada del ESP32) |
| GPS NEO-6M | RX | GPIO 17 (`TXD2`, salida del ESP32) |

> ⚠️ Las conexiones de datos van **cruzadas**: el TX del GPS va al RX del ESP32 y viceversa. El GPS se conecta al **UART2** del ESP32 (`HardwareSerial gpsSerial(2)`) a 9600 baudios. El detalle de funcionamiento del módulo y de la librería `TinyGPSPlus` está en [`Tutorial uso del GPS con esp32.txt`](./Tutorial%20uso%20del%20GPS%20con%20esp32.txt).

### Receptor

| Componente | Conexión |
|---|---|
| LoRa Ra-02 | Igual que en el proyecto básico (3.3V, GND, SCK 18, MISO 19, MOSI 23, NSS 5, RST 14, DIO0 26) |
| LED / LCD | **No se usan en esta versión** |
| Conectividad | Se conecta por **WiFi** a la red configurada en el código — no requiere cableado adicional |

---

## Explicación del código - Transmisor

Archivo: `Esp32_Transmisor.ino`

**`setup()`**
- Inicializa SPI y el módulo LoRa igual que en el proyecto básico (433 MHz, SF 12, BW 125 kHz, CR 4/8, potencia 20 dBm).
- Inicializa el sensor DHT22 en el pin `GPIO 32`.
- Inicializa el puerto serie del GPS (`gpsSerial`) en el `UART2` del ESP32, a 9600 baudios, usando `GPIO 16` como RX y `GPIO 17` como TX.

**`loop()`**
1. Lee temperatura y humedad del DHT22 con `dht.getTempAndHumidity()`. Si la lectura falla (`NaN`), imprime un error por serie y reintenta en el próximo ciclo (no envía el paquete ese ciclo).
2. Lee los datos disponibles del GPS byte a byte (`gpsSerial.available()` / `gps.encode(c)`), y cuando hay una ubicación actualizada (`gps.location.isUpdated()`), obtiene latitud y longitud.
3. Arma un mensaje de texto separado por comas con el formato:

   ```
   nodeId,temperatura,humedad,latitud,longitud
   ```

   Ejemplo: `1,28.4,55.0,-32.950000,-60.663333`

4. Envía el mensaje por LoRa con `LoRa.beginPacket()` / `LoRa.print(mensaje)` / `LoRa.endPacket()`.
5. Espera 2 segundos y repite.

`NODE_ID` es una constante (actualmente `1`) que identifica a este transmisor — útil si en el futuro hay varios nodos transmitiendo al mismo receptor.

---

## Explicación del código - Receptor / Servidor web

Archivo: `1785867290084_Esp32_Receptor_App.ino`

**`setup()`**
- Inicializa el módulo LoRa (mismos parámetros de radio que el transmisor).
- Se conecta a la red WiFi definida en `ssid` / `password`, esperando en un bucle hasta lograr la conexión (`WiFi.status() == WL_CONNECTED`).
- Levanta un servidor web propio en el puerto 80 (`WebServer server(80)`), asociando la ruta raíz `"/"` a la función `handleRoot()`.
- Imprime por el puerto serie la IP asignada por WiFi — **esa es la dirección que hay que usar en el navegador** para ver la web.

**`loop()`**
1. Atiende las solicitudes web entrantes con `server.handleClient()` (necesario en cada vuelta del loop para que el servidor responda).
2. Revisa si llegó un paquete LoRa nuevo con `LoRa.parsePacket()`.
3. Si llegó un paquete:
   - Lo arma como string y lo separa por comas para extraer `nodeId`, `temp`, `hum`, `lat` y `lon` (mismo orden en que los envía el transmisor).
   - Obtiene `RSSI` y `SNR` del paquete recibido (`LoRa.packetRssi()` / `LoRa.packetSnr()`).
   - Calcula un **índice de riesgo** simple (0-100), sumando puntos según umbrales:
     - Temperatura > 35°C → +20 puntos, y otros +20 si supera 40°C.
     - Humedad < 40% → +20 puntos, y otros +20 si baja de 20%.
     - El resultado se limita (clamp) a un máximo de 100.
   - Guarda todos los valores en variables globales (`webTemp`, `webHum`, `webRisk`, etc.) que son las que se muestran en la página web.
   - Reenvía los datos por HTTP como JSON a `serverUrl` mediante `HTTPClient` (`http.POST(json)`), útil para centralizar los datos en un servidor/backend externo.
   - Imprime todo por el puerto serie a modo de log.

**`handleRoot()`** — genera el HTML de la página web (con auto-refresh cada 2 segundos vía `<meta http-equiv="refresh" content="2">`) mostrando nodo, temperatura, humedad, ubicación, RSSI, SNR y riesgo, con un estilo simple oscuro tipo tarjeta.

---

## Cómo probarlo

1. Cablear el transmisor con LoRa + DHT22 + GPS según la tabla de conexionado. Cablear el receptor solo con el módulo LoRa (no lleva LED ni LCD).
2. Antes de subir el código del receptor, editar:
   - `ssid` y `password` con los datos de tu red WiFi.
   - `serverUrl` con la URL real de tu API (si no tenés un servidor propio para recibir los datos, podés comentar/quitar el bloque del `HTTPClient` y usar solo la página web).
3. Cargar `Esp32_Transmisor.ino` en el ESP32 transmisor y el `.ino` del receptor en el otro ESP32.
4. Abrir el Monitor Serie del receptor (115200 baudios) y esperar a que se conecte al WiFi — va a imprimir la **dirección IP** asignada.
5. Desde un celular o PC conectado a la **misma red WiFi**, abrir esa IP en el navegador (ej. `http://192.168.1.50/`). Debería verse la tarjeta "FireSense" actualizándose automáticamente cada vez que llega un paquete nuevo.
6. Revisar en el Monitor Serie del transmisor que las lecturas de temperatura, humedad y GPS sean válidas antes de asumir que el problema está del lado del receptor.

---

## Problemas conocidos / a mejorar

- **Credenciales de WiFi hardcodeadas en el código** (`ssid`, `password`): si el repositorio es público, conviene no subir la contraseña real — reemplazarla por un placeholder en el código que se publica, o mover las credenciales a un archivo separado no versionado.
- **`serverUrl` es un placeholder** (`http://TU_SERVIDOR/api/data`): hay que reemplazarlo por la URL real del backend, o el `http.POST()` va a fallar silenciosamente (solo se ve el código de error en el Monitor Serie).
- **`int SnR = LoRa.packetSnr();`**: `packetSnr()` devuelve un valor `float`; al guardarlo en un `int` se pierde la parte decimal. Si se quiere precisión, conviene declarar `SnR` como `float`.
- **Comentario de formato desactualizado**: el comentario `// Formato: ID,TEMP,HUM,WIND,DIR,LAT,LON` no coincide con lo que realmente se parsea (`nodeId,temp,hum,lat,lon`, sin viento ni dirección). Conviene actualizar el comentario para que no confunda a quien lea el código.
- **Sin control de paquetes incompletos**: si por ruido o error de transmisión llega un paquete con menos comas de las esperadas, `packet.substring()` puede devolver valores vacíos o `0`, y no hay ninguna validación antes de usarlos.
- **Latitud/Longitud sin inicializar en el transmisor si el GPS no tiene fix**: `lat` y `lon` se declaran sin valor inicial (`double lat; double lon;`) y solo se les asigna algo si `gps.location.isUpdated()` es verdadero ese ciclo. Si el GPS todavía no tiene señal (por ejemplo recién arrancado o en interiores), el mensaje puede enviarse con coordenadas basura. Conviene inicializarlas en `0` y/o no enviar el paquete hasta tener un primer fix válido.
