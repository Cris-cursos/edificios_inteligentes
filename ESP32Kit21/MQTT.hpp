//***** CONFIGURACION MQTT *******//
const char* MQTT_BROKER_ADRESS = "192.168.7.66";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_SERVER_NAME = "iotserver";
const char* MQTT_CLIENT_NAME = "esp21";
const char* MQTT_USERNAME = "iotserver";
const char* MQTT_PASSWORD = "CSC#Jesuitas";
void InitMqtt();
void ConnectMqtt();
void HandleMqtt();
void SubscribeMqtt();
void PublisMqtt(String, String);
void OnMqttReceived(char*, byte*, unsigned int);

void InitMqtt() {
  mqttClient.setServer(MQTT_BROKER_ADRESS, MQTT_PORT);
  SubscribeMqtt();
  mqttClient.setCallback(OnMqttReceived);
}

void ConnectMqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Starting MQTT connection...");
    InitMqtt();
    mqttClient.setCallback(OnMqttReceived);
    if (mqttClient.connect(MQTT_CLIENT_NAME, MQTT_USERNAME, MQTT_PASSWORD)) {
      SubscribeMqtt();
      Serial.println("Connected");
    } else {
      Serial.print("Failed MQTT connection, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    if (WiFi.status() != WL_CONNECTED) {break;}
  }
}

void HandleMqtt() {
  if (!mqttClient.connected()) {
    ConnectMqtt();
  }
  mqttClient.loop();
}

void SubscribeMqtt() {
  for (int i = 0; i < NUMITEMS(TOPICS_SUBSCRIBED); i++) {
    mqttClient.subscribe(TOPICS_SUBSCRIBED[i].c_str());
  }
}

void PublisMqtt(String newTopic, String data) {
  mqttClient.publish(newTopic.c_str(), (char*)data.c_str());
}

void OnMqttReceived(char* topic, byte* payload, unsigned int length) {
  content = "";
  Serial.print("Received on ");
  Serial.print(topic);
  Serial.print(": ");
  for (size_t i = 0; i < length; i++) {
    content.concat((char)payload[i]);
  }
  Serial.print(content);
  accessGranted = content.toInt();
  Serial.println();
}