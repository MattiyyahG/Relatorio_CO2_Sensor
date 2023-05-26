#include <ESP8266WiFi.h>

#include <PubSubClient.h>

#include <HCSR04.h>

#include <NTPClient.h>

#include <WiFiUdp.h>

//#define sensor A0

const byte triggerPin = D6;

const byte echoPin = D5;

const char* ssid = "wPESC-Visitante";

const char* password = ""; 

const char* mqtt_server = "test.mosquitto.org"; 

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org");

WiFiClient espClient;

PubSubClient client(espClient);

UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)

char msg[MSG_BUFFER_SIZE];

int tolerancia = 1000;

void setup_wifi() {

  delay(500);

  Serial.println();

  Serial.print("Conectando ao Wifi: ");

  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

  delay(500);

  Serial.print(".");

  }

  randomSeed(micros());

  Serial.println("");

  Serial.println("Conectado!");

  Serial.println("IP: ");

  Serial.println(WiFi.localIP());

}


void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Mensagem Recebida no Topico [");

  Serial.print(topic);

  Serial.print("] ");

  for (int i = 0; i < length; i++) {

  Serial.print((char)payload[i]);
  
  }
  Serial.println();

}

void reconnect() {

  while (!client.connected()) {

  Serial.print("Tentando reconectar...");

  String clientId = "CO2-ESP8266-Client-";

  clientId += String(random(0xffff), HEX);

  if (client.connect(clientId.c_str())) {

    Serial.println("Conectado!");

    client.publish("lens/CO2", "hello world");

    client.subscribe("lens/CO2");

  } else {

    Serial.print("Falha ao conectar, rc=");

    Serial.print(client.state());

    delay(5000);

    }

  }

}


void setup() {

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);

  client.setCallback(callback);
  
  timeClient.begin();

  timeClient.setTimeOffset(-10800);

}

void loop() {

  if (!client.connected()) {

  reconnect();

  }

  client.loop();

  timeClient.update();
  
  unsigned long now = millis();

  int distance = distanceSensor.measureDistanceCm();

//  int valor_dist = analogRead(sensor); 

  if(distance > tolerancia){         
    
  Serial.print("Muito Proximo");
    
  Serial.println();

  client.publish("lens/CO2", "MUITO PROXIMO");

  String formattedTime = timeClient.getFormattedTime();
  
  Serial.println(formattedTime);
  
  client.publish("lens/CO2", formattedTime.c_str());

  delay(2000); 
  
  }
 
  else {
    
  if (now - lastMsg > 2000) {

  lastMsg = now;
  
  Serial.println();

  snprintf (msg, MSG_BUFFER_SIZE, "Proximidade: %ld cm", distance);

  Serial.println(msg);
  
  Serial.println();

  client.publish("lens/CO2", msg);

  String formattedTime = timeClient.getFormattedTime();
  
  Serial.println(formattedTime);
  
  client.publish("lens/CO2", formattedTime.c_str());
  
  delay(2000);

    }
 
  }
 
}