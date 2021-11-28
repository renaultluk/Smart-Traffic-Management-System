#include <WiFiNINA.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "graphStructs.h"


void splitCommand(String payload, String commandList[]) {
  int str_length = payload.length();
  int list_index = 0;
  
  for (int i = 0; i < str_length; i++) {
    if (payload[i] == ' ') {
      list_index++;
    } else {
      commandList[list_index] += payload[i];
    }
  }
}

void createNewJSON(String topic) {
  StaticJsonDocument<200> doc;
  weightsJson = doc.to<JsonArray>();
}

void addToJSON(JsonArray& root, int key, int value) {
    JsonObject record = root.createNestedObject();
    record["key"] = key;
    record["value"] = value;
}

void publishWeightChanges(JsonArray& weightChanges) {
    String weightStr;
    serializeJson(weightChanges, weightStr);
    byte arrSize = weightStr.length() + 1;
    char msg[arrSize];
    weightStr.toCharArray(msg, arrSize);
    client.publish(weightTopic, msg);
    createNewJSON(weightTopic);
}

void parseWeightJson(String payload) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    JsonArray root = doc.as<JsonArray>();
    int root_size = root.size();
    for (int i = 0; i < root_size; i++) {
        int key = root[i]["key"];
        int value = root[i]["value"];
        edge_map[key].weight += value;
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String payloadStr;
  for (int i = 0; i < length; i++) 
  {
    payloadStr += (char)payload[i];
  }
  Serial.println();

  parseWeightJson(payloadStr);
}


void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ArduinoClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttUsername, mqttPassword)) 
    {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(weightTopic);
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//void loop() {
//  if (!client.connected()) {
//    reconnect();
//  }
//  client.loop();
//  long now = millis();
//  if (now - lastMsg > 5000) {
//    lastMsg = now;
//  }
//}
