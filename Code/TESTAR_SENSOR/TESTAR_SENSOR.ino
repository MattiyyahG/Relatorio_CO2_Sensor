#include <MQUnifiedsensor.h>

#define PIN_SENSOR A0 // Pino analógico conectado ao sensor

MQUnifiedsensor MQ2(PIN_SENSOR);

void setup() {
  Serial.begin(9600); // Inicia a comunicação serial
  MQ2.setRegressionMethod(1); // Escolhe o método de regressão linear
  MQ2.setA(262.81); // Configura o valor da "a" na equação de calibração
  MQ2.setB(-0.7421); // Configura o valor da "b" na equação de calibração
}

void loop() {
  float gasValue = MQ2.readSensor(); // Lê o valor do sensor
  Serial.print("Valor de Gás: ");
  Serial.println(gasValue); // Imprime o valor do gás na porta serial
  
  delay(1000); // Aguarda 1 segundo antes de ler novamente
}