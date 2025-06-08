# Práctica 5: Buses de comunicación (I2C)
Participants: Alexandre Pascual / Martí Vila

## Introducción
En esta práctica, exploraremos el **bus I2C** aplicada a la placa _ESP32-S3_.

El laboratorio se dividirà en dos partes:
1. **Escaneo de dispositivos I2C** conectados a la placa.
2. **Visualización de la temperatura de un sensor** en una pantalla OLED y un servidor web (con el codigo aplicado de la practica 3).


## Parte 1: Escaneo de dispositivos

### **Objetivo**
Detectar dispositivos I2C conectados al ESP32-S3 a partir de un escaneo.

#### **Código**
```c++
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Wire.begin(18, 17); // Configurar SDA en pin 18 y SCL en pin 17
  Serial.begin(115200);
  while (!Serial); // Esperar a que el monitor serie esté listo
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    } 
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found\n");
  else Serial.println("done\n");

  delay(5000); // Esperar 5 segundos antes del próximo escaneo
}
```

### **Explicación del código**
- Se configuran los pines **SDA (18)** y **SCL (17)** para conectar los dispositivos en el _I2C_.
- Se realiza un **escaneo de direcciones**.
- Si un dispositivo responde, se muestra por pantalla su dirección.
- Se repite el escaneo cada **5 segundos**.

### **Salida esperada por pantalla**
```
Scanning...
I2C device found at address 0x3C !
I2C device found at address 0x38 !
done
```


## Parte 2: Sensor de temperatura y pantalla OLED

### **Objetivo**
Se utiliza un **sensor AHTX0** para medir la temperatura y mostrarla en:
- Una **pantalla OLED**.
- Una **página web iniciada en la ESP32-S3**.

#### **Código**
```c++
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>  // Librería para el sensor AHTX0

// Configuración del display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuración del sensor AHTX0
Adafruit_AHTX0 aht;

// Credenciales de la red WiFi
const char* ssid = "Redmi"; 
const char* password = "Marti12345"; 

// Creación del servidor en el puerto 80
WebServer server(80);

// Variable para almacenar la temperatura
float temperatura = 0.0;

// Función para manejar la página web
void handle_root() {
  String HTML = "<!DOCTYPE html>\
  <html>\
  <head>\
      <meta charset='UTF-8'>\
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>\
      <title>Temperatura ESP32</title>\
      <style>\
          body { font-family: Arial, sans-serif; text-align: center; margin: 50px; }\
          h1 { color: #4CAF50; }\
          p { font-size: 20px; }\
      </style>\
  </head>\
  <body>\
      <h1>Medición de Temperatura 🌡️</h1>\
      <p>Temperatura actual: " + String(temperatura) + " °C</p>\
      <p>Dirección IP: " + WiFi.localIP().toString() + "</p>\
  </body>\
  </html>";

  server.send(200, "text/html", HTML);
}

// Configuración inicial del ESP32
void setup() {
    Serial.begin(115200);
    Wire.begin(18, 17);  // Pines SDA y SCL

    // Inicialización del display OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("No se encontró la pantalla OLED");
        while (true);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Inicialización del sensor AHTX0
    if (!aht.begin()) {
        Serial.println("No se encontró el sensor AHTX0");
        while (true);
    }

    // Conexión WiFi
    Serial.print("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado con éxito.");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Configuración del servidor
    server.on("/", handle_root);
    server.begin();
    Serial.println("Servidor HTTP iniciado.");
}

// Bucle principal
void loop() {
    // Leer temperatura del AHTX0
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp); // Obtener temperatura y humedad

    temperatura = temp.temperature; // Extraer el valor de temperatura

    // Mostrar en la pantalla OLED
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Temperatura:");
    display.setTextSize(2);
    display.setCursor(10, 30);
    display.print(temperatura);
    display.print(" C");
    display.display();

    // Manejar las peticiones de la web
    server.handleClient();

    // Esperar un poco antes de actualizar la medición
    delay(2000);
}
```

### **Explicación del código**
- Se activa la pantalla OLED junto con el sensor AHTX0.
- La ESP32-S3 se enlazó a una conexión WiFi y inició un servidor web en el puerto 80.
- La temperatura se renueva cada 2 segundos, presentándose tanto en la pantalla como en la página web.

### **Salida esperada por pantalla**
```
WiFi conectado con éxito.
Dirección IP: 192.168.1.100
Servidor HTTP iniciado.
```

## Conclusiones
- Se consiguió identificar dispositivos I2C conectados al ESP32-S3.
- Se realizó la medición de la temperatura utilizando el sensor AHTX0, que se visualizó en una pantalla OLED.
- Se creó un servidor web para que la temperatura se pudiera observar desde el teléfono.
- La utilización de I2C facilitó una comunicación **eficaz** entre varios dispositivos.

