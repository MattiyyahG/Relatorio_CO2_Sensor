//Abaixo, foi definido o código, mas o sensor utilizado foi o de distância Ultrassônico HC-SR04
//Por motivos de acessibilidade, mas a implementação é a mesma da mencionada no READ.ME, mas apenas a biblioteca e as variaveis necessarias para o funcionamento do mesmo que foram alteradas.

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

WiFiClient espClient;

NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org");

PubSubClient client(espClient);

UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE (100)

char msg[MSG_BUFFER_SIZE];

int tolerancia = 10;

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

//Definindo o tempo e a data

  time_t epochTime = timeClient.getEpochTime();

  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int diaMes = ptm-> tm_mday;

  int atualMes = ptm-> tm_mon + 1;

  int atualAno = ptm-> tm_year + 1900;

  String atualData = String(atualAno) + "-" + String(atualMes) + "-" + String(diaMes);

  if(distance < tolerancia){         
    
  Serial.print("Muito Proximo");
    
  Serial.println();

  String formattedTime = timeClient.getFormattedTime();
  
  snprintf (msg, MSG_BUFFER_SIZE, "MUITO PROXIMO - [Horário: %s] - [Data: %s]", formattedTime, atualData);

  client.publish("lens/CO2", msg);

  delay(2000); 
  
  }
 
  else {
    
  if (now - lastMsg > 2000) {

  lastMsg = now;

  String formattedTime = timeClient.getFormattedTime();
  
  Serial.println();

  snprintf (msg, MSG_BUFFER_SIZE, "Proximidade: %ld cm - [Horário: %s] - [Data: %s]", distance, formattedTime, atualData);

  Serial.println(msg);
  
  Serial.println();

  client.publish("lens/CO2", msg);
  
//  Serial.println(formattedTime);
  
  delay(2000);

    }
 
  }
 
}
