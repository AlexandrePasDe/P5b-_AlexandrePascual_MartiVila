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
