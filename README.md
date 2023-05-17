# Relatório

## 1. Introdução.

Neste relatório, será estabelecido a conexão, pinagem, e citação de materiais necessários para realizar a conexão MQTT (Message Queue Telemetry Transport) entre um ESP8266. Não obstante, o envio dos dados de um sensor de gás carbonico (CO2) - MQ135, via broker também será realizada.

Iniciamos da seguinte forma, com os materiais necessários para implementação do mesmo, tal qual são:

* ESP8266.
* Jumper.
* Cabo USB (Tipo-B) para alimentar o ESP8266.
* Sensor MQ-135.
* Protoboard (Opcional caso não haja pinagem soldada no ESP8266).

## 2. Pinout Modelo Esquemático.

Abaixo, está o modelo esquemático da pinagem para implementação do ESP8266:

![PinOut](Code/Assets/ESP8266_PinOut.png)

#### Imagem obtida no site (https://how2electronics.com/gas-level-monitor-esp8266-gas-sensor/)

Na imagem, foi utilizado o sensor MQ-7, mas o mesmo esquemático se aplica ao MQ-135, pois a pinagem do MQ-135 se dá por:

![MQ-135 Pinagem](https://www.circuits-diy.com/wp-content/uploads/2021/10/MQ135-pinout-air-quality-sensor.jpg)

#### Imagem obtida no site (https://how2electronics.com/gas-level-monitor-esp8266-gas-sensor/)


As entradas se dá por, respectivamente:
* A0 - Saída do valor analógico registrado/medido pelo sensor.
* D0 - Saída digital, o valor limite de tolerância pode ser definido usando o potenciômetro.
* GND - Terra/GND.
* VCC - Entrada de energia (3.3v). [Cuidado, se colocar em 5v pode ocorrer acidentes!].

## 3. Código.

Incluindo as bibliotecas 

```
#include <ESP8266WiFi.h>                          

#include <PubSubClient.h>   
```

Para encontrar a biblioteca do ESP8266, é necessário colocar esse URL (http://arduino.esp8266.com/stable/package_esp8266com_index.json) no diretório: Files -> Preferences -> 'Additional Boards Manager URLs:'. 

Após isso, vá em: Tools -> Board -> Boards Manager... e pesquise por 'ESP8266' e adicione.

Não obstante, logo abaixo está a definição da variável que vai receber o valor analógico do sensor, no pino A0.

```
#define sensor A0
```

Logo abaixo, existe o código predefinido para que o ESP8266 entenda qual é a Rede que você está conectado (Ou qualquer uma que esteja em sua área de sinal). A respectiva senha do mesmo, e o broker que vai ser utilizado.

Após, a declaração de um objeto de classe WiFiClient que permite a conexão com um especifico IP e porta definida, e o PubSubClient recebe uma entrada de um construtor previamente definido pelo WiFiClient.

Definição do buffer, e da tolerância da taxa de gás carbonico medida pelo sensor (MQ-135).

```
const char* ssid = "(Substitua pelo seu WiFi)"; 

const char* password = "(Coloque a senha do mesmo)"; 

const char* mqtt_server = "test.mosquitto.org";


WiFiClient espClient;

PubSubClient client(espClient);


unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)

char msg[MSG_BUFFER_SIZE];                   

int tolerancia = 100000;
```

Abaixo, configurações antes e depois da conexão ter sido feita, vai ocorrer um print no serial do arduino enquanto a conexão com o Wi-Fi não for concluida, e respectivamente, printar um "." enquanto o status do wifi for diferente de conectado, e quando conectar, printar uma mensagem de confirmação e o respectivo endereço IP.

```
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
```

Logo abaixo, uma funçao callback foi criada, cujo objetivo principal é, após uma mensagem ter sido de fato enviada e recebida sem nenhum problema, envie uma confirmação de mensagem no serial, especificando em qual tópico a mensagem foi enviada e recebida, e continue enquanto a mensagem for enviada.

```
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Mensagem Recebida no Topico [");

  Serial.print(topic);

  Serial.print("] ");

  for (int i = 0; i < length; i++) {

  Serial.print((char)payload[i]);
  
  }
  Serial.println();

}
```

Não obstante, a função de reconexão foi criada, que diz, enquanto o status do cliente for diferente de conectado, realize um print que está tentando reconectar a conexão com o client definido, agora, se o cliente estiver conectado, mande uma mensagem confirmando que está, e publique no tópico criado no broker a seguinte mensagem de "olá mundo" e se inscreva no tópico. Agora, caso nenhuma das alternativas foram concluidas com sucesso, envie uma confirmação de falha e printe o status no serial.

```
void reconnect() {
  
  while (!client.connected()) {

  Serial.print("Tentando reconectar...");

  String clientId = "CO2-ESP8266-Client-";

  clientId += String(random(0xffff), HEX);

  if (client.connect(clientId.c_str())) {

    Serial.println("Conectado!");

    client.publish("labnet/CO2", "hello world");

    client.subscribe("labnet/CO2");

  } else {

    Serial.print("Falha ao conectar, rc=");

    Serial.print(client.state());

    delay(5000);

  }

  }

}
```