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

```
const char* ssid = "(Substitua pelo seu WiFi)"; 

const char* password = "(Coloque a senha do mesmo)"; 

const char* mqtt_server = "test.mosquitto.org";


WiFiClient espClient;

PubSubClient client(espClient);
```




