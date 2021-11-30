#include <WiFiNINA.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "graphStructs.h"

const int capacity = JSON_ARRAY_SIZE(NUM_EDGES) + NUM_EDGES*JSON_OBJECT_SIZE(2);

void createNewJSON() {
  Serial.print("createNewJSON \t capacity= ");
  Serial.println(capacity);
  DynamicJsonDocument doc(capacity);
  weightsJson = doc.to<JsonArray>();
}

void addToJSON(JsonArray& root, int key, int value) {
    Serial.print("addToJSON \t key: ");
    Serial.print(key);
    Serial.print("\t value: ");
    Serial.println(value);
    JsonObject record = root.createNestedObject();
    record["key"] = key;
    record["value"] = value;
}

void publishWeightChanges(JsonArray& weightChanges) {
    Serial.println("publishWeightChanges");
    String weightStr;
    serializeJson(weightChanges, weightStr);
    byte arrSize = weightStr.length() + 1;
    char msg[arrSize];
    weightStr.toCharArray(msg, arrSize);
    client.publish(weightTopic, msg);
    createNewJSON();
}

void parseWeightJson(String payload) {
    StaticJsonDocument<capacity> doc;
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
