# Relatório sobre a implementação do sensor de dióxido de carbono e envio de dados via MQTT.

## 1. Resumo 

Neste trabalho será apresentado o desenvolvimento de um sensor de CO2 com transmissão de dados utilizando o módulo ESP8266 e o protocolo MQTT. Além disso, foi implementada uma funcionalidade de time stamp para registrar a data e hora das leituras enviadas para o broker. Essa abordagem permite uma análise mais precisa e um melhor entendimento dos níveis de CO2 no ambiente monitorado, contribuindo para a tomada de decisões eficientes em relação à qualidade do ar.

## 2. Introdução.

Iniciamos da seguinte forma, com os materiais necessários para implementação do mesmo, tal qual:

* ESP8266.
* Jumper.
* Cabo USB (Tipo-B) para alimentar o ESP8266.
* Sensor MQ-135.
* Protoboard (Opcional caso não haja pinagem soldada no ESP8266).

## 3. Metodologia

Para atingir o objetivo proposto, foram utilizados os seguintes componentes e tecnologias:

## 3.1 Módulo ESP8266 

O módulo ESP8266 foi selecionado como plataforma de desenvolvimento devido à sua capacidade de se conectar a redes Wi-Fi e sua compatibilidade com o protocolo MQTT. Esse módulo permite a integração do sensor de CO2 e a transmissão dos dados coletados para um broker MQTT.

![ESP8266](Code/Assets/esp8266.jpg)

## 3.2 Protocolo MQTT

O protocolo MQTT (Message Queuing Telemetry Transport) é um protocolo leve e de baixo consumo de energia, o que o torna ideal para o envio de dados de sensores em redes de dispositivos conectados.

## 3.3 Sensor de CO2

Foi utilizado um sensor de CO2 de alta precisão para realizar as medições dos níveis de dióxido de carbono no ambiente monitorado. tal sensor fornece leituras confiáveis e estáveis, garantindo a qualidade dos dados coletados.

![MQ135](Code/Assets/MQ-135.webp)

## 3.4 Time Stamp

Tambem foi implementado uma funcionalidade de time stamp para registrar a data e hora das leituras enviadas para o broker MQTT. Essa informação é fundamental para análises posteriores e permite um melhor entendimento das variações nos níveis de CO2 ao longo do tempo.

## 4. Pinout Modelo Esquemático.

Abaixo (1), está o modelo esquemático da pinagem para implementação do ESP8266:

![PinOut](Code/Assets/ESP8266_PinOut.png)


(Na imagem acima (2), foi utilizado o sensor MQ-7, mas o mesmo esquemático se aplica ao MQ-135, pois a pinagem do MQ-135 se dá por:).

![MQ-135 Pinagem](https://www.circuits-diy.com/wp-content/uploads/2021/10/MQ135-pinout-air-quality-sensor.jpg)

* A0 - Saída do valor analógico registrado/medido pelo sensor.
* D0 - Saída digital, o valor limite de tolerância pode ser definido usando o potenciômetro.
* GND - Terra/GND.
* VCC - Entrada de energia (3.3v). [Cuidado, se colocar em 5v a probabilidade de ocorrer acidentes é alta!].

## 5. Código.

Incluindo as bibliotecas:

```
#include <ESP8266WiFi.h>          

#include <PubSubClient.h>   

#include <NTPClient.h>

#include <WiFiUdp.h>
```

(No código acima, não foi incluido, mas para o funcionamento do mesmo, é necessário incluir e instalar a bilioteca respectiva ao sensor que será utilizado, tal qual: "MQ-2, MQ-3, MQ-135" e afins, sem a mesma, não haverá calibração e respectivamente não funcionará o mesmo, no arquivo 'Codigo.ino' a biblioteca.)

Para encontrar a biblioteca do ESP8266, é necessário colocar esse URL (http://arduino.esp8266.com/stable/package_esp8266com_index.json) no diretório: Files -> Preferences -> 'Additional Boards Manager URLs:'. 

Após isso, vá em: Tools -> Board -> Boards Manager... e pesquise por 'ESP8266' e adicione.

Depois, Sketch -> Include Library -> Manage Libraries e pesquise por 'PubSubClient' e adicione.

Não obstante, logo abaixo está a definição da variável que vai receber o valor analógico do sensor, no pino A0.

NTPClient.h biblioteca desenvolvida para sincronizar com o servidor NTP, essa biblioteca foi adicionada para realizar uma sincronização do sensor com o valor do tempo/horário atual, para criar uma timestamp, com o objetivo de entender em qual horário que foi valor foi medido.

WiFiUdp.h Biblioteca que lida com as tarefas do protocolo UDP, enviando e recebendo pacotes UDP enquanto estiver online.

```
#define sensor A0
```

Abaixo está o código predefinido para configurar o ESP8266 com informações sobre a rede Wi-Fi à qual você deseja se conectar (ou qualquer rede disponível na área), a respectiva senha e o broker que será utilizado. Existem diversos brokers gratuitos, o que foi utilizado nesse trabalho foi um de testes feito pelo mosquitto.

Em seguida, é feita a declaração de um objeto da classe WiFiClient, que permite a conexão com um IP e porta específicos definidos. O objeto PubSubClient é inicializado com o construtor que recebe o WiFiClient como parâmetro. Em seguida, o servidor responsável por fornecer o horário mundial específico da América do Sul é definido. Outros servidores possíveis são:

* pool.ntp.org
* asia.pool.ntp.org
* europe.pool.ntp.org
* north-america.pool.ntp.org
* oceania.pool.ntp.org

Também é definido o tamanho do buffer, que representa a quantidade total de caracteres da mensagem a ser enviada.

```
const char* ssid = "(Substitua pelo seu WiFi)"; 

const char* password = "(Coloque a senha do mesmo)"; 

const char* mqtt_server = "test.mosquitto.org";

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "south-america.pool.ntp.org");

WiFiClient espClient;

PubSubClient client(espClient);

unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)

char msg[MSG_BUFFER_SIZE];                   

```

Abaixo estão as configurações antes e depois da conexão ser estabelecida. Durante esse processo, o Arduino irá imprimir no serial até que a conexão Wi-Fi seja concluída. Enquanto o status do Wi-Fi for diferente de "conectado", será impresso um ".". Quando a conexão for estabelecida, será impressa uma mensagem de confirmação juntamente com o endereço IP correspondente.

Para definir a placa ESP8266 no modo estação, utilize o comando "WiFi.mode(WIFI_STA)". Esse modo permite que a placa se conecte a outras redes, funcionando como um roteador. Após a conexão, a placa ESP8266 receberá um endereço IP único, possibilitando a comunicação com outros dispositivos conectados à mesma rede. Nesse caso, a conexão será feita usando o endereço IP definido para o próprio ESP8266.

Para iniciar a conexão com o Wi-Fi, utilize o comando "WiFi.begin()" informando o nome da rede (SSID) e a senha correspondente. Em seguida, uma função será executada para aguardar a estabilização da conexão, exibindo um "." enquanto o status do Wi-Fi estiver diferente de "conectado".

O comando "randomSeed(micros())" é utilizado para inicializar o gerador de números pseudoaleatórios com uma semente baseada no tempo atual. Isso garante que diferentes execuções do programa gerem sequências distintas de números pseudoaleatórios, atualizando o valor a cada recebimento.

Resumidamente, nesse trabalho, o roteador foi configurado como ponto de acesso principal, e as estações (ESP8266 e outros dispositivos conectados ao broker) se conectaram a ele. Também é possível configurar a placa ESP8266 como um ponto de acesso semelhante ao roteador, mas isso não foi necessário nesse projeto.

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

Logo abaixo, é criada uma função de retorno de chamada (callback) cujo objetivo principal é enviar uma confirmação para o serial após uma mensagem ter sido enviada e recebida sem problemas. A confirmação inclui informações sobre o tópico no qual a mensagem foi enviada e recebida. A função continuará executando enquanto as mensagens estiverem sendo enviadas.

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

Além disso, foi criada uma função de reconexão, que verifica o status do cliente. Se o cliente estiver desconectado, será exibida uma mensagem informando que está tentando reconectar à conexão com o cliente definido. Se o cliente estiver conectado, será enviada uma mensagem de confirmação e será publicada a mensagem "olá mundo" no tópico criado no broker, além de se inscrever nesse tópico. No entanto, se nenhuma das opções for concluída com sucesso, será enviada uma confirmação de falha e o status será impresso no serial.

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

Na função setup, é feita a chamada da função "setup_wifi" para configurar a conexão Wi-Fi, e é definida a taxa de leitura serial em 115200 Baund Rate. O broker é configurado na porta 1883 e a função de retorno de chamada de mensagem é ativada.

A função "client.setServer(mqtt_server, 1883)" conecta o dispositivo ao servidor no endereço especificado. Em seguida, é iniciado o cliente NTP com a função timeClient.begin(). O fuso horário da América do Sul, que é GMT -3, é configurado em segundos usando o cálculo: (GMT da sua região: -1, -2, -3...) * 60 * 60. O resultado desse cálculo é utilizado para calibrar o horário com a função timeClient.setTimeOffset().

```
void setup() {

  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);

  client.setCallback(callback);

  timeClient.begin();

  timeClient.setTimeOffset(-10800);

}
```

Na função loop (um laço de repetição), o valor do sensor é medido e enviado repetidamente para o broker.

A primeira condição verifica se o cliente não está conectado e, nesse caso, realiza uma reconexão.

É definida a variável "value_gas" para receber o valor analógico inteiro do sensor no pino A0, em seguida, aguarda-se 1 segundo. Caso contrário, apenas o valor da taxa de gás medida é impresso no tópico do broker.

Além disso, é configurado um formato padrão para exibir o horário, no formato de Horas:Minutos:Segundos. A função "timeClient.getFormattedTime()" retorna os valores do horário nesse formato.

Para complementar com os valores da data atual, é utilizado novamente o servidor de salvamento de dados do dia atual. É utilizado a função "timeClient.getEpochTime()" para obter o valor da data atual em forma de época. Em seguida, é criada uma estrutura para armazenar esse valor, convertendo-o em um inteiro para o dia, mês e ano atuais. Essas variáveis são utilizadas para criar uma string chamada "atualData", que contém todos os valores coletados em um único formato.

Como a variável "tm_mday" inicia com o valor 0, é necessário adicionar o valor inteiro 1 ao mês para que "janeiro" seja representado pelo valor 1, fevereiro pelo valor 2 e assim por diante.

Logo abaixo, é criada a variável "atualMes" para armazenar o valor inteiro do mês atual. E em seguida, a variável "atualAno" é configurada para corresponder ao ano atual. É importante observar que essa biblioteca somente permite salvar valores de ano a partir de 1900, então é necessário adicionar esse valor inteiro para obter o ano corrente.


```
void loop() {

  if (!client.connected()) {

  reconnect();

  }

  client.loop();

  timeClient.update();

  unsigned long now = millis();

  int value_gas = analogRead(sensor); 

  time_t epochTime = timeClient.getEpochTime();

  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int diaMes = ptm-> tm_mday;

  int atualMes = ptm-> tm_mon + 1;

  int atualAno = ptm-> tm_year + 1900;

  String atualData = String(atualAno) + "-" + String(atualMes) + "-" + String(diaMes);

  String formattedTime = timeClient.getFormattedTime();
    
  if (now - lastMsg > 2000) {

  lastMsg = now;

  snprintf (msg, MSG_BUFFER_SIZE, "Taxa de Gás: %ld ppm - [Horário: %s]- [Data: %s]", value_gas, formattedTime, atualData);

  client.publish("labnet/CO2", msg);

  delay(2000);

  }
 
}
```

## 6. Resultados e Discussão

O sensor de CO2 com transmissão de dados utilizando o módulo ESP8266 e o protocolo MQTT foi implementado com sucesso. As leituras de CO2, juntamente com as informações de time stamp, foram enviadas para o broker MQTT de forma eficiente e essa abordagem permitiu uma análise mais precisa e um melhor entendimento dos níveis de CO2 no ambiente monitorado. A disponibilidade dos dados em tempo real possibilita a identificação de padrões e variações nos níveis de CO2 ao longo do tempo, auxiliando na tomada de decisões eficientes em relação à qualidade do ar.

## 7. Referências e Material de Leitura.

* 1) (https://how2electronics.com/gas-level-monitor-esp8266-gas-sensor/), Autor: How to Electronics.

* 2) (https://www.circuits-diy.com/mq135-air-quality-smoke-gas-sensor/), Autor: Farwah Nawazi.

* 3) (https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/), Autor: Random Nerd Tutorials.

* 4) (https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/), Autor: Last Minute Engineers.

* 5) (https://linuxhint.com/esp32-ntp-client-server-date-time/), Autor: Kashif.

* 6) (https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/), Autor: Random Nerd Tutorials.

* 7) (https://github.com/Martinsos/arduino-lib-hc-sr04), Autor: Martinsos.

* 8) (https://www.instructables.com/Getting-Time-From-Internet-Using-ESP8266-NTP-Clock/), Autor: Utsource.

* 9) (https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/), Autor: Techtutorialsx.

* 10) (https://www.instructables.com/NodeMCU-MQTT-Basic-Example/), Autor: Osoyooproduct.