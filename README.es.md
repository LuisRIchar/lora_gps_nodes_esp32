| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

# Nodos LoRa GPS con geocerca

[English](README.md) | [Español](README.es.md)

Firmware ESP-IDF para nodos de rastreo alimentados por batería, basados en una Seeed XIAO ESP32-S3, un radio LoRa SX1278 a 433 MHz y un módulo GPS por UART. Cada nodo reporta su posición por LoRa en un paquete cifrado con AES-128-CTR, recibe una coordenada objetivo desde el lado web del sistema y activa un buzzer cuando se aleja más de 20 m de ese punto.

<img src="docs/IMG_5771.png" width="560" alt="Dos nodos prototipo sobre un escritorio: cada uno con una XIAO ESP32-S3, un módulo SX1278 con antena de resorte, un buzzer y un módulo GPS con antena cerámica de parche">

## Funcionamiento

Cada nodo corre cuatro tareas de FreeRTOS, todas con stack estático excepto la del driver del radio:

| Tarea | Prioridad | Función |
| ----- | --------- | ------- |
| `lora_driver` | 10 | Se despierta con la interrupción de DIO0, llama a `sx127x_handle_interrupt()` y mete los paquetes recibidos en una cola |
| `lora_task` | 5 | Espera un paquete durante 4–6 s (aleatorio); si no llega nada, envía su propia posición |
| `geofence_measure_task` | 5 | Cada 2 s compara la posición actual con el objetivo y controla la alarma |
| `gps_task` | 4 | Lee NMEA de UART1, parsea `$GPRMC`/`$GNRMC` y guarda el último fix válido protegido por un mutex |

La ventana de recepción aleatoria evita que dos nodos transmitan sincronizados en el mismo canal.

### Geocerca

El objetivo llega por LoRa desde el lado web como un mensaje cifrado en alguno de estos dos formatos:

```
Web Latitude: 19.432608, Longitude: -99.133209
Web:19.432608,-99.133209
```

`receive_helper.c` lo parsea, lo mete en la cola de la geocerca y lo confirma con el LED y una melodía del buzzer. A partir de ahí, `geofence_measure_task` calcula cada 2 s la distancia entre el fix actual y el objetivo. Si es mayor que `GEOFENCE_THRESHOLD_METERS` (20 m), el buzzer suena tres veces y el nodo reporta `On Target: 0`; dentro del radio reporta `On Target: 1`. Un nuevo mensaje de objetivo reemplaza al anterior.

La distancia se calcula con una aproximación equirectangular en lugar de Haversine:

```
x = Δλ · cos((φ1 + φ2) / 2)
y = Δφ
d = R · √(x² + y²)        R = 6 371 000 m
```

Latitud y longitud son ángulos, así que la diferencia este–oeste se escala por el coseno de la latitud media antes de aplicar Pitágoras. Para perímetros menores a 1 km la curvatura de la Tierra es despreciable y el error queda por debajo del 0.1 %, y se ahorran las llamadas a `sin`/`asin`/`atan2` que Haversine necesita en cada iteración.

### Formato del paquete

Cada paquete empieza con un contador de 4 bytes en big-endian seguido del texto cifrado. El contador ocupa los últimos 4 bytes del nonce de CTR (los primeros 12 son fijos y compartidos con el gateway en la Raspberry Pi Zero), así que el receptor descifra con la misma llamada a `encrypt_data()`.

```
| counter (4 B, big-endian) | AES-128-CTR(payload) |
```

Payload de uplink:

```
Node: 1, Lat: 19.432608, Lon: -99.133209, On Target: 1
```

No se envía nada hasta que el GPS tiene un fix válido.

## Uso

### Hardware requerido

- Seeed Studio XIAO ESP32-S3
- Módulo LoRa SX1278 (433 MHz)
- Módulo GPS con salida NMEA a 9600 baud
- Buzzer pasivo
- Batería LiPo en los pads de batería de la XIAO (opcional)

<img src="docs/IMG_5772.png" width="560" alt="Vista superior de ambos nodos mostrando el cableado en protoboard entre la XIAO ESP32-S3, los módulos SX1278, los buzzers y los dos módulos GPS">

| Señal | GPIO ESP32-S3 |
| ----- | ------------- |
| SX1278 SCK | 7 |
| SX1278 MISO | 8 |
| SX1278 MOSI | 9 |
| SX1278 NSS | 4 |
| SX1278 RESET | 3 |
| SX1278 DIO0 | 2 |
| GPS RX (TX del ESP) | 43 |
| GPS TX (RX del ESP) | 44 |
| Buzzer (LEDC, PWM) | 6 |
| LED de estado | 21 |
| Selección de ID de nodo | 1 |

El número de nodo sale del nivel de GPIO1 en tiempo de ejecución: conéctalo a GND para el nodo 1 y a 3V3 para el nodo 2. El pin no tiene pull interno configurado, así que no lo dejes flotando.

### Parámetros del radio

Definidos en `src/drivers/lora_spi.c`. El gateway debe coincidir en todos:

| Parámetro | Valor |
| --------- | ----- |
| Frecuencia | 433 MHz |
| Ancho de banda | 125 kHz |
| Spreading factor | 12 |
| Coding rate | 4/5, header explícito, CRC activo |
| Sync word | `0x12` |
| Potencia TX | 10 dBm en PA_BOOST |

### Configuración

El proyecto se compiló con ESP-IDF v6.0.0 (según `dependencies.lock`). El único componente externo es [`dernasherbrezon/sx127x`](https://components.espressif.com/components/dernasherbrezon/sx127x) ^5.0.1, que el Component Manager descarga en el primer build.

```sh
idf.py set-target esp32s3
```

El `sdkconfig` del repo ya apunta al ESP32-S3 con 4 MB de flash; no hace falta `idf.py menuconfig` para un build por defecto.

### Compilación y flasheo

```sh
idf.py build
idf.py -p PORT flash monitor
```

## Estructura del proyecto

| Directorio | Contenido |
| ---------- | --------- |
| `src/main` | `app_main()`: arranca las tareas de LoRa y de geocerca |
| `src/app` | Ciclo de envío/recepción, parseo de mensajes, lógica de geocerca |
| `src/drivers` | SX1278 por SPI, GPS por UART, LED, buzzer, mapa de pines |
| `src/util` | Wrapper de AES-128-CTR sobre el AES por hardware del ESP32 |
| `docs` | Fotos del prototipo |

## Limitaciones

- La llave y el nonce de AES están fijos en `src/util/crypto.c` y son públicos en este repositorio.
- El contador de TX arranca en 0 en cada boot, así que después de un reset el nodo reutiliza keystreams de CTR. El receptor no valida el contador, por lo que no hay protección contra replay.
- Los paquetes no llevan tag de autenticación; un texto cifrado modificado se descifra como basura en lugar de rechazarse.
- Solo hay un objetivo activo a la vez y el radio es una constante de compilación.
- `test_tasks()` en `send_receive.c` es una prueba de eco en texto plano que se conserva para bring-up; no se llama desde `app_main()`.

## Licencia

MIT. Ver [LICENSE](LICENSE).
