#include <WiFiNINA.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "../graphStructs.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledState = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void createNewJSON(String topic, String payload) {
  StaticJsonDocument<200> doc;
}

void addToJSON(JsonObject& root, int key, int value) {
    JsonObject record = root.createNestedObject();
    record["key"] = key;
    record["value"] = value;
}

void parseWeightJson(String payload) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    JsonObject root = doc.as<JsonObject>();
    for (JsonPair kv : root) {
        Serial.print(kv.key());
        Serial.print(": ");
        Serial.println(kv.value());

        edge_map[kv.key()].weight += kv.value();
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void fetchMap() {

}

void publishMap(int path_length) {
    string mapStr = "";
    for (int i = 0; i < MAP_SIZE; i++) {
        mapStr += String(map[i]);
        mapStr += ",";
    }
    client.publish("/map", "map");
}

void publishWeightChanges(JsonObject& weightChanges) {
    string weightStr;
    serializeJson(weightChanges, weightStr);
    client.publish("/weights", weightStr);
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
      client.subscribe(subTopic);
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

void setup() 
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  }
}