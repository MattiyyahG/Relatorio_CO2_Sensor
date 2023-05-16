# Relatório

## 1. Introdução.

        Neste relatório, será estabelecido a conexão, pinagem, e citação de materiais necessários para realizar a conexão MQTT (Message Queue Telemetry Transport) entre um ESP8266. Não obstante, o envio dos dados de um sensor de gás carbonico (CO2) - MQ135, via broker também será realizada.

        Iniciamos da seguinte forma, com os materiais necessários para implementação do mesmo, tal qual são:

* ESP8266.
* Jumper.
*  Cabo USB (Tipo-B) para alimentar o ESP8266.
* Sensor MQ-135.
* Protoboard (Opcional caso não haja pinagem soldada no ESP8266).

## 2. Pinout Modelo Esquemático.

        Abaixo, está o modelo esquemático da pinagem para implementação do ESP8266:

![GitHub/Assets/ESP8266_PinOut.png](PinOut)

        Na imagem, foi utilizado o sensor MQ-7, mas o mesmo esquemático se aplica ao MQ-135, pois a pinagem do MQ-135 se dá por:

![https://www.circuits-diy.com/wp-content/uploads/2021/10/MQ135-pinout-air-quality-sensor.jpg](MQ-135 Pinagem)

        As entradas se dá por, respectivamente:
* A0 - Saída do valor analógico registrado/medido pelo sensor.
* D0 - Saída digital, o valor limite de tolerância pode ser definido usando o potenciômetro.
* GND - Terra/GND.
* VCC - Entrada de energia (3.3v). [Cuidado, se colocar em 5v pode ocorrer acidentes!].


