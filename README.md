# P5bó_AlexandrePascual_MartiVila
Participants: Alexandre Pascual i Martí Vila

# Busos de comunicació (I2C)


## Introducció

En aquesta pràctica treballarem amb el bus **I2C** a la nostra placa 'ESP32-S3'.
Primer mostrarem com hem aprés a escanejar dispositus **I2C** connectats a la placa i en el segon apartat mostrarem la temperatura d'un sensor a una panatalla OLED i un servidor web el qual ha sigut aplicat desde el codi base de la pràctica 3.

## Escàner

### Objectiu

Fer un escaneig i que es pugui detectar els dispositius **I2C** connectats a la nostra placa.

#### *Codi*
```c++
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Wire.begin(18, 17); // Configurar SDA en pin 18 i SCL en pin 17
  Serial.begin(115200);
  while (!Serial); // Esperar a que el monitor serie estigui llest
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

  delay(5000); // Esperar 5 segons entre escaneig i escaneig
}
```

### **Explicació**

Configurem els pins **SDA** i **SCL** en **18** i **17** respectivament per connectar els dispositius, tot seguit es realitza un 'escaneig de direccions'.
Si un dispositiu es connecta, a la pantalla podrem veure la seva direcció.
Aquest escaneig s'anirà repetint cada 5 segons.

### **Sortida esperada**
```
Scanning...
I2C device found at address 0x3C !
I2C device found at address 0x38 !
done
```


## Sensor de temperatura i pantalla OLED

### Objectiu

Utilitzem un sensor **AHTX0** que mesura la temperatura i ho mostrem en una pantalla OLED i una pagina web indicada a la placa.

#### *Codi*
```c++
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>  // Llibreria per el sensor AHTX0

// Configuració del display OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configuració del sensor AHTX0
Adafruit_AHTX0 aht;

// Credencials de la red WiFi
const char* ssid = "Redmi"; 
const char* password = "Marti12345"; 

// Creació del servidor en el port 80
WebServer server(80);

// Variable per guardar la temperatura
float temperatura = 0.0;

// Funció per controlar la página web
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

// Configuració inicial del ESP32
void setup() {
    Serial.begin(115200);
    Wire.begin(18, 17);  // Pines SDA y SCL

    // Inicialització del display OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("No se encontró la pantalla OLED");
        while (true);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Inicialització del sensor AHTX0
    if (!aht.begin()) {
        Serial.println("No se encontró el sensor AHTX0");
        while (true);
    }

    // Connexió WiFi
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
    // Llegir temperatura del AHTX0
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp); // Obtenir temperatura i humitat

    temperatura = temp.temperature; // Extreure el valor de temperatura

    // Mostrar en la pantalla OLED
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Temperatura:");
    display.setTextSize(2);
    display.setCursor(10, 30);
    display.print(temperatura);
    display.print(" C");
    display.display();

    // Controlar les peticions de la web
    server.handleClient();

    // Esperar una mica abans d'actualizar la medició
    delay(2000);
}
```

### **Explicació**

Activa la pantalla i el sensor **AHTX0** i la ESP32-S3 crea un servidor web al port 80.
La temperatura es va actualitzant cada 2 segons i s'imprimeix tant per la pantalla com a la pagina web.

### **Sortida esperada**
```
WiFi conectado con éxito.
Dirección IP: 192.168.1.100
Servidor HTTP iniciado.
```

## Resum

Hem aprés a identificar dispositius I2C connectats a la ESP32-S3 i tot seguit ho hem aplicat mitjançant la connexió d'una pantalla OLED i un sensor de temperatura **AHTX0**.
Hem creat una pagina web per el qual hem mostrat la temperatura del sensor i alhora es mostraba per la pantalla OLED. L'I2C genera una comunicació eficaç entre diversos dispositius com hem pogut veure en aquesta pràctica.
