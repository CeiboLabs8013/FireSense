# Comunicación LoRa punto a punto con ESP32 y módulos Ra-02

Proyecto básico de comunicación inalámbrica **LoRa** entre dos placas **ESP32**, cada una conectada a un módulo **LoRa Ra-02 (SX1278)**. El objetivo es demostrar el enlace más simple posible:

- El **transmisor** envía el carácter `"1"` cada 2 segundos.
- El **receptor** recibe el paquete, enciende un LED por 1 segundo y muestra en una pantalla **LCD 16x2** la potencia de la señal recibida (**RSSI**) y la relación señal/ruido (**SNR**).

Sirve como punto de partida para proyectos más grandes (telemetría, sensores remotos, etc.), ya que valida que el enlace físico LoRa funciona antes de sumar lógica adicional.

---

## Índice

1. [¿Qué es LoRa y el módulo Ra-02?](#qué-es-lora-y-el-módulo-ra-02)
2. [Materiales necesarios](#materiales-necesarios)
3. [Instalación del entorno (Arduino IDE)](#instalación-del-entorno-arduino-ide)
4. [Conexionado](#conexionado)
5. [Explicación del código - Transmisor](#explicación-del-código---transmisor)
6. [Explicación del código - Receptor](#explicación-del-código---receptor)
7. [Cómo probarlo](#cómo-probarlo)
8. [Problemas conocidos / a mejorar](#problemas-conocidos--a-mejorar)
9. [Posibles mejoras futuras](#posibles-mejoras-futuras)

---

## ¿Qué es LoRa y el módulo Ra-02?

**LoRa** (*Long Range*) es una técnica de modulación de radio de baja potencia y largo alcance, pensada para redes IoT. Permite enviar paquetes pequeños de datos a varios kilómetros de distancia con muy bajo consumo, a costa de un ancho de banda bajo (no sirve para transmitir audio, video, ni grandes volúmenes de datos).

El **Ra-02** es un módulo económico basado en el chip **SX1278 (Semtech)**, que trabaja en la banda de **433 MHz** (también existen versiones de 868/915 MHz) y se comunica con el microcontrolador mediante el bus **SPI**. En este proyecto ambos ESP32 usan la frecuencia `433E6` (433 MHz), por lo que **los dos módulos Ra-02 deben ser de la misma banda de frecuencia**.

Parámetros de radio que se configuran en el código (deben coincidir en ambos extremos):

| Parámetro | Valor usado | Función |
|---|---|---|
| Frecuencia | 433 MHz | Frecuencia portadora |
| Spreading Factor (SF) | 12 | A mayor SF, mayor alcance pero menor velocidad |
| Ancho de banda (BW) | 125 kHz | Ancho del canal |
| Coding Rate | 4/8 | Corrección de errores hacia adelante (FEC) |
| Potencia TX | 20 dBm | Solo configurada en el transmisor |

> ⚠️ Si la frecuencia, el SF, el BW o el Coding Rate no coinciden entre el transmisor y el receptor, **no va a haber comunicación**.

---

## Materiales necesarios

- 2x **ESP32** (cualquier placa de desarrollo genérica, ej. ESP32 DevKit V1)
- 2x módulo **LoRa Ra-02 (SX1278)** de 433 MHz, con antena
- 1x **LED** + resistencia de 220-330 Ω (para el receptor)
- 1x **pantalla LCD 16x2** (con o sin módulo I2C — el código usa conexión paralela directa) (para el receptor)
- 1x **potenciómetro de 10kΩ** (para ajustar el contraste del LCD)
- Protoboard y cables dupont
- Fuente de alimentación / cable USB para cada ESP32

---

## Instalación del entorno (Arduino IDE)

1. Instalar el **Arduino IDE** (versión 1.8.x o 2.x).
2. Agregar el soporte de placas ESP32:
   - `Archivo > Preferencias > Gestor de URLs Adicionales de Tarjetas`, agregar:
     `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Luego en `Herramientas > Placa > Gestor de tarjetas`, buscar "esp32" e instalar el paquete de **Espressif Systems**.
3. Instalar las librerías necesarias desde `Herramientas > Administrar Bibliotecas`:
   - **LoRa** de *Sandeep Mistry* (la que provee `LoRa.h`)
   - **LiquidCrystal** (suele venir incluida con el IDE)
   - **DHT sensor library for ESPx** (`DHTesp.h`) — solo necesaria si se usa el sensor DHT (ver [Problemas conocidos](#problemas-conocidos--a-mejorar))
4. Seleccionar la placa correcta en `Herramientas > Placa`, por ejemplo *ESP32 Dev Module*, y el puerto COM correspondiente.

---

## Conexionado

### 1. Módulo LoRa Ra-02 ↔ ESP32 (igual en transmisor y receptor)

El módulo se conecta por SPI. Estos son los pines definidos en ambos programas:

| Pin del Ra-02 | Pin del ESP32 | Definido en el código como |
|---|---|---|
| VCC | **3.3V** (nunca 5V) | — |
| GND | GND | — |
| SCK | GPIO 18 | `SPI.begin(18, ...)` |
| MISO | GPIO 19 | `SPI.begin(..., 19, ...)` |
| MOSI | GPIO 23 | `SPI.begin(..., ..., 23, ...)` |
| NSS / CS | GPIO 5 | `SS` |
| RST | GPIO 14 | `RST` |
| DIO0 | GPIO 26 | `DIO0` |

> ⚠️ El Ra-02 se alimenta con **3.3V**, no con 5V. La mayoría de los ESP32 ya proveen 3.3V regulados en su pin `3V3`.

### 2. LED (solo en el receptor)

| LED | Pin ESP32 |
|---|---|
| Ánodo (+) → resistencia 220-330 Ω → | GPIO 13 |
| Cátodo (-) | GND |

### 3. LCD 16x2 (solo en el receptor)

El código inicializa el LCD así:
```cpp
LiquidCrystal lcd(25, 33, 27, 21, 16, 17); // RS, EN, D4, D5, D6, D7
```

| Pin del LCD | Pin del ESP32 / componente |
|---|---|
| VSS | GND |
| VDD | 5V |
| V0 (contraste) | Cursor del potenciómetro de 10kΩ (entre 5V y GND) |
| RS | GPIO 25 |
| RW | GND |
| EN | GPIO 33 |
| D4 | GPIO 27 |
| D5 | GPIO 21 |
| D6 | GPIO 16 |
| D7 | GPIO 17 |
| A (ánodo backlight) | 5V (a través de una resistencia si el módulo no la trae) |
| K (cátodo backlight) | GND |

> ⚠️ En el código hay una segunda declaración de pines (`rs, en, d4, d5, d6, d7` en la línea 7 del receptor) que **no se usa realmente** — el objeto `lcd` se crea con los valores de la línea 8. Ver la sección de [problemas conocidos](#problemas-conocidos--a-mejorar).

---

## Explicación del código - Transmisor

Archivo: `Esp32_Transmisor.ino`

**`setup()`**
1. Inicia el puerto serie a 115200 baudios (solo para debug).
2. Inicializa el bus SPI en los pines `18, 19, 23` (SCK, MISO, MOSI) usando `SS` como pin de chip select.
3. Define los pines de control del módulo LoRa con `LoRa.setPins(SS, RST, DIO0)`.
4. Inicia el módulo LoRa en 433 MHz con `LoRa.begin(433E6)`. Si falla, imprime `"FALLO"` y queda trabado en un bucle infinito (esto normalmente indica mal cableado o módulo dañado).
5. Configura los parámetros de radio: potencia de transmisión (20 dBm), spreading factor (12), ancho de banda (125 kHz) y coding rate (4/8).
6. Inicializa el sensor DHT22 (actualmente declarado pero no utilizado en el `loop()`, ver más abajo).

**`loop()`**
1. Abre un paquete LoRa con `LoRa.beginPacket()`.
2. Escribe el carácter `"1"` en el paquete.
3. Cierra y envía el paquete con `LoRa.endPacket()`.
4. Espera 2 segundos antes de repetir.

Es decir: cada 2 segundos el transmisor manda un paquete con el contenido `"1"`, sin ningún dato de sensores todavía.

---

## Explicación del código - Receptor

Archivo: `Esp_32_Receptor.ino`

**`setup()`**
1. Inicia el puerto serie a 115200 baudios.
2. Configura el pin 13 como salida digital (para el LED).
3. Inicializa el bus SPI igual que el transmisor.
4. Inicializa el LCD y muestra un mensaje de prueba ("Linea 1" / "Linea 2").
5. Configura los pines del módulo LoRa e inicia el radio en 433 MHz con los mismos parámetros que el transmisor (spreading factor 12, BW 125 kHz, coding rate 4/8). **Deben coincidir siempre con el transmisor.**
6. Limpia el LCD y muestra "LoRa OK" si todo salió bien.

**`loop()`**
1. Consulta si llegó un paquete nuevo con `LoRa.parsePacket()`.
2. Si hay un paquete disponible:
   - Lee byte por byte el contenido recibido y lo arma en el string `incoming`.
   - Si el mensaje recibido es `"1"`, enciende el LED (`digitalWrite(13, HIGH)`), lo mantiene prendido 1 segundo, y lo apaga.
   - Actualiza el LCD mostrando:
     - `RSSI:` → potencia de la señal recibida, en dBm (`LoRa.packetRssi()`). Cuanto más cercano a 0, mejor señal (valores típicos van de -30 a -120 dBm aprox.).
     - `SNR:` → relación señal/ruido en dB (`LoRa.packetSnr()`). Valores positivos indican buena calidad de señal.

---

## Cómo probarlo

1. Cablear ambos ESP32 con su módulo Ra-02 según la tabla de conexionado. Conectar además el LED y el LCD solo en la placa que va a usarse como **receptor**.
2. Cargar `Esp32_Transmisor.ino` en un ESP32 y `Esp_32_Receptor.ino` en el otro, cada uno desde el Arduino IDE con la placa y el puerto correctos.
3. Alimentar ambas placas (por USB, por ejemplo).
4. Abrir el **Monitor Serie** (115200 baudios) de cada placa para verificar que ambas iniciaron el módulo LoRa correctamente (deberían imprimir `"Iniciado"` en el transmisor; si aparece `"FALLO"`, revisar el cableado SPI).
5. En el receptor, el LCD debería mostrar `"LoRa OK"` tras la inicialización.
6. Cada 2 segundos debería verse en el receptor:
   - El LED encendiéndose brevemente.
   - El LCD actualizando los valores de `RSSI` y `SNR`.
7. Para probar el alcance, alejar gradualmente los dos ESP32 y observar cómo cambian el RSSI y el SNR (a mayor distancia, RSSI más negativo y SNR más bajo, hasta perder la conexión).

---

## Problemas conocidos / a mejorar

Cosas detectadas en el código actual que conviene tener en cuenta antes de subir el proyecto, o corregir si se quiere dejar "prolijo" en el repositorio:

- **DHT22 sin declarar en el transmisor**: se incluye `DHTesp.h` y se llama a `dht.setup(...)`, pero nunca se declara el objeto `DHTesp dht;`. Esto **no debería compilar** tal cual está. Si no se va a usar el sensor de temperatura/humedad todavía, se puede quitar esa parte del código; si se va a usar, falta declarar el objeto y agregar la lectura del sensor dentro del paquete enviado.
- **Pines de LCD declarados pero no usados (receptor)**: la línea `const int rs = 32, en = 33, d4 = 15, d5 = 4, d6 = 16, d7 = 17;` no se utiliza — el objeto `lcd` se crea con otros valores (`lcd(25, 33, 27, 21, 16, 17)`). Conviene eliminar esa línea para evitar confusiones al conectar el hardware.
- **`lcd.begin(16, 1)`**: para una pantalla LCD de 16x2 (2 filas), debería ser `lcd.begin(16, 2)`. Con `1` fila, escribir en `lcd.setCursor(0, 1)` (segunda fila) puede no comportarse como se espera en todas las librerías/pantallas.
- El mensaje enviado por el transmisor es siempre el texto fijo `"1"`. No hay verificación de contenido ni checksum más allá de lo que ya maneja la librería LoRa internamente.

---

## Posibles mejoras futuras

- Enviar datos reales del sensor **DHT22** (temperatura y humedad) en lugar del `"1"` fijo.
- Usar un formato de mensaje más estructurado (JSON simple o separado por comas) para poder escalar a más variables.
- Agregar control de errores / reintentos si no llega un paquete dentro de un tiempo esperado.
- Migrar el LCD a interfaz **I2C** para reducir la cantidad de cables usados.
