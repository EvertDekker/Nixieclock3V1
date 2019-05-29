void init_mqtt(){
   mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setCredentials(MQTT_LOGIN, MQTT_PASSW);
  mqttClient.setServer(MQTT_IP, 1883);
}

void onMqttConnect(bool sessionPresent) {
  DEBUG_PRINTLN("Connected to MQTT.");
  DEBUG_PRINT("Session present: ");
  DEBUG_PRINTLN(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe(MQTT_ROOT_TOPIC "status", 2);
  packetIdSub = mqttClient.subscribe(MQTT_ROOT_TOPIC "displaytest", 2);
  packetIdSub = mqttClient.subscribe(MQTT_ROOT_TOPIC "reset", 2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  DEBUG_PRINTLN("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  DEBUG_PRINTLN("Subscribe acknowledged.");
  DEBUG_PRINT("  packetId: ");
  DEBUG_PRINTLN(packetId);
  DEBUG_PRINT("  qos: ");
  DEBUG_PRINTLN(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  DEBUG_PRINTLN("Unsubscribe acknowledged.");
  DEBUG_PRINT("  packetId: ");
  DEBUG_PRINTLN(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  DEBUG_PRINTLN("Publish received.");
  DEBUG_PRINT("  topic: ");
  DEBUG_PRINTLN(topic);
  DEBUG_PRINT("  qos: ");
  DEBUG_PRINTLN(properties.qos);
  DEBUG_PRINT("  dup: ");
  DEBUG_PRINTLN(properties.dup);
  DEBUG_PRINT("  retain: ");
  DEBUG_PRINTLN(properties.retain);
  DEBUG_PRINT("  len: ");
  DEBUG_PRINTLN(len);
  DEBUG_PRINT("  index: ");
  DEBUG_PRINTLN(index);
  DEBUG_PRINT("  total: ");
  DEBUG_PRINTLN(total);

  if (String(topic) == MQTT_ROOT_TOPIC "reset") {
    if (String(payload) == "RESET") {
      DEBUG_PRINTLN("Going for reset...");
      resetsystem("By Mqqt request");
    }
  }
}

void onMqttPublish(uint16_t packetId) {
  DEBUG_PRINTLN("Publish acknowledged.");
  DEBUG_PRINT("  packetId: ");
  DEBUG_PRINTLN(packetId);
}


void publishdata( void * parameter )
{

//  while (1) {
//
//    vTaskDelay( PUBLISHTIME / portTICK_PERIOD_MS );
//  }
//  vTaskDelete( NULL );

}
