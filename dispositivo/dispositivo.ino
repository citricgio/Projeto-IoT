//Incluir bibliotecas
#include <DHTesp.h>
#include <EspMQTTClient.h>

//Para saber mais sobre esta biblioteca, acessar https://github.com/plapointe6/EspMQTTClient

//Definicoes e constantes
char SSIDName[] = "Wokwi-GUEST"; //nome da rede WiFi
char SSIDPass[] = ""; //senha da rede WiFI

const int DHT_PIN = 15; //terminal do sensor de temperatura e umidade

char BrokerURL[] = "broker.hivemq.com"; //URL do broker MQTT
char BrokerUserName[] = ""; //nome do usuario para autenticar no broker MQTT
char BrokerPassword[] = ""; //senha para autenticar no broker MQTT
char MQTTClientName[] = "mqtt-mack-pub"; //nome do cliente MQTT
int BrokerPort = 1883; //porta do broker MQTT

String TopicoPrefixo = "TESTMACK1165744"; //prefixo do topico
String Topico_01 = TopicoPrefixo+"/Temperatura"; //nome do topico 01
String Topico_02 = TopicoPrefixo+"/Umidade"; //nome do topico 02

//Variaveis globais e objetos
DHTesp dhtSensor; //instancia o objeto dhtSensor a partir da classa DHTesp

EspMQTTClient clienteMQTT(SSIDName, SSIDPass, BrokerURL, BrokerUserName, BrokerPassword, MQTTClientName, BrokerPort); //inicializa o cliente MQTT

//Este prototipo de funcao deve estar sempre presente
void onConnectionEstablished() {
}

void enviarDados() {
  TempAndHumidity temp_umid = dhtSensor.getTempAndHumidity(); //instancia o objeto temp_umid a partir da classe TempAndHumidity

  //Serial.println("Temperatura: " + String(temp_umid.temperature, 2) + "°C");
  //Serial.println("Umidade: " + String(temp_umid.humidity, 1) + "%");
  //Serial.println("---");

  clienteMQTT.publish(Topico_01, String(temp_umid.temperature, 2) + "°C");
  clienteMQTT.publish(Topico_02, String(temp_umid.humidity, 1) + "%");
}

//Setup
void setup() {
  Serial.begin(9600);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22); //inicializa o sensor de temperatura e umidade

  clienteMQTT.enableDebuggingMessages(); //habilita mensagens de debug no monitor serial
  //clienteMQTT.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  //clienteMQTT.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  //clienteMQTT.enableLastWillMessage("TestClient/lastwill", "Vou ficar offline");
}

//Loop
void loop() {
  //clienteMQTT.enableMQTTPersistence(); //estabelece uma conexao persistente
  clienteMQTT.loop(); //funcao necessaria para manter a conexao com o broker MQTT ativa e coletar as mensagens recebidas
  enviarDados(); //funcao para publicar os dados no broker MQTT

  if (clienteMQTT.isWifiConnected() == 1) {
    Serial.println("Conectado ao WiFi!");
  } else {
    Serial.println("Nao conectado ao WiFi!");
  }

  if (clienteMQTT.isMqttConnected() == 1) {
    Serial.println("Conectado ao broker MQTT!");
  } else {
    Serial.println("Nao conectado ao broker MQTT!");
  }

  Serial.println("Nome do cliente: " + String(clienteMQTT.getMqttClientName())
    + " / Broker MQTT: " + String(clienteMQTT.getMqttServerIp())
    + " / Porta: " + String(clienteMQTT.getMqttServerPort())
  );

  delay(5000);
}
