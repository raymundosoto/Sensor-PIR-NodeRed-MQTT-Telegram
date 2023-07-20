#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Configuración de la red Wi-Fi
const char* ssid = "Totalplay-319F_2.4Gnormal";
const char* password = "319FEF5D88K2kS97";

// Configuración de MQTT
const char* mqtt_server = "192.168.100.137";  // Dirección IP del broker MQTT local
const int mqtt_port = 1883;                  // Puerto del broker MQTT
const char* mqtt_topic = "Sensores/PIR";     // Tópico MQTT para publicar los datos del sensor PIR

// Configuración del sensor PIR
const int pirPin = 2;  // Pin al que está conectado el sensor PIR

// Objeto WiFi
WiFiClient wifiClient;

// Objeto MQTT
PubSubClient mqttClient(wifiClient);

void setup() {
  // Inicializar el puerto serie
  Serial.begin(115200);

  // Conexión a la red Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red Wi-Fi...");
  }
  Serial.println("Conectado a la red Wi-Fi");

  // Conexión al broker MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  while (!mqttClient.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Conectado al broker MQTT");
    } else {
      Serial.print("Error de conexión al broker MQTT. Código de error: ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }

  // Configurar el pin del sensor PIR como entrada
  pinMode(pirPin, INPUT);

  // Suscribirse al tópico MQTT (opcional)
  // mqttClient.subscribe(mqtt_topic);
}

void loop() {
  // Leer el estado del sensor PIR
  int pirValue = digitalRead(pirPin);

  // Crear un objeto DynamicJsonDocument
  DynamicJsonDocument jsonDoc(128);

  // Establecer los valores en el objeto JSON
  jsonDoc["id"] = "PIR";
  jsonDoc["Presencia"] = (pirValue == HIGH);

  // Serializar el objeto JSON a una cadena
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Publicar el estado del sensor PIR en el tópico MQTT
  mqttClient.publish(mqtt_topic, jsonString.c_str());

  // Imprimir la cadena JSON en el monitor serie
  Serial.println(jsonString);

  if (pirValue == HIGH) {
    delay(5000);
  } else {
    delay(1000);
  }

  

  // Esperar un segundo antes de la siguiente lectura
//  delay(1000);

  // Manejar las comunicaciones MQTT
  mqttClient.loop();
}
