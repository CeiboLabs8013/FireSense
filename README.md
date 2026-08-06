# CeiboLabs 🌿

**Tecnología abierta para impacto ambiental real.**

---

## ¿Qué es CeiboLabs?

CeiboLabs es un laboratorio tecnológico **abierto, educativo y sin fines de lucro**, nacido dentro de las prácticas profesionales del **Colegio San José de Rosario**.

Somos **cuatro estudiantes** de último año que decidimos usar nuestras prácticas profesionales para algo más que una tarea escolar: construir tecnología accesible que resuelva problemas ambientales y sociales reales.

Todo lo que desarrollamos es:

- 🔓 **Open Source** — el código y el conocimiento se comparten.
- 💸 **Sin fines de lucro** — no buscamos vender, buscamos ayudar.
- 🌎 **Con impacto real** — pensado para comunidades y ecosistemas concretos, no como ejercicio teórico.

La idea de fondo es simple: *la innovación no tiene sentido si no mejora la vida de alguien.*

---

## ¿Qué es FireSense?

**FireSense** es el primer proyecto de CeiboLabs: un sistema de **detección temprana de incendios** pensado especialmente para los **humedales de Rosario y Entre Ríos**, una zona históricamente afectada por quemas e incendios que son muy difíciles de detectar a tiempo.

La idea es combinar **sensores de bajo costo, comunicación de largo alcance e inteligencia artificial** dentro de una columna autoportante y autónoma, para poder avisar antes de que el fuego se propague y el daño sea irreversible.

Trabajamos junto a ONGs y organizaciones ambientales para que la herramienta responda a una necesidad real, no a una idea de laboratorio.

### Arquitectura técnica (resumen)

La "columna FireSense" es un dispositivo modular basado en **ESP32**, pensado para desplegarse en el campo de forma autónoma. Sus componentes principales son:

| Módulo | Función |
|---|---|
| **ESP32 (maestro)** | Cerebro del dispositivo, gestiona sensores y comunicación. |
| **Comunicación LoRa** | Alcance mayor a 5 km, ideal para zonas rurales/forestales sin cobertura. Permite interconectar múltiples columnas. |
| **ESP32-CAM + Visión Artificial** | Analiza el entorno con modelos de IA livianos (procesados localmente) para detectar humo y llamas en tiempo real, sin depender de conexión externa. |
| **Sensores ambientales** | Viento (velocidad/dirección), temperatura y humedad (ambiente y suelo), para estimar riesgo y propagación. |
| **GPS** | Georreferencia el foco de incendio con precisión, alimentando mapas de riesgo. |
| **Panel solar + baterías** | Energía autónoma, con respaldo para operar de noche o en días nublados. |

Además, el sitio incluye una **visualización experimental de focos de incendio en Argentina**, usando datos satelitales de **NASA FIRMS** (satélite **VIIRS**), actualizados periódicamente.

## Avances del proyecto:
Dentro de este repositorio se encontraran todos los avances realizados en estos meses de trabajo:
-Comunicación LoRa
-Localización GPS
-Estructura MVP1
-Datos satelitales
-APP comunicación dashword ambientales poste 1.


## ¿Como ayudar?

Si sos una persona interesada en el proyecto, ya sea un pasante, o alguien externo al colegio, estas son algunas áreas donde se necesita ayuda:
- **Antenas** No logramos una optima comunicación entre antenas (la mejor prueba fue de 600mts), por lo cual se deberia continuar incursionando sobre el tema e intentar resolver este problema.
- **Firmware embebido** (ESP32 / ESP32-CAM, sensores, LoRa, GPS).
- **Visión artificial / IA** para detección de humo y llamas en dispositivos de bajos recursos.
- **Frontend web** (el sitio institucional y el mapa de incendios).
- **Integración de datos satelitales** (NASA FIRMS / VIIRS).
- **Diseño de hardware** (energía solar, carcasa, montaje de la columna).
- **Contenido / bitácora** (documentar avances del proyecto).

## Contacto
- Página web: www.ceibolabs.com.ar
- 📧 Email: [ceibolabs8013@gmail.com](mailto:ceibolabs8013@gmail.com)
- 💬 WhatsApp: [+54 341 5427141](https://wa.me/543415427141)
- 📷 Instagram: [@ceibo.labs](https://instagram.com/ceibo.labs)

