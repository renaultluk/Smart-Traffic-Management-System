#include <WiFiNINA.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "graphStructs.h"

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
    client.publish(weightTopic, weightStr);
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
        int value = root[i]["value"]
        edge_map[key].weight += value;
    }
}

void parseNodeMap(String payload) {
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
        int node_index = root[i]["index"];
        int node_x = root[i]["x"];
        int node_y = root[i]["y"];
        bool dest = root[i]["dest"];
        node_map[i] = newNode(node_index, node_x, node_y);
        
        if (dest) {
          destinations[dest_length] = &node_map[i];
          dest_length++;
        }
    }
}

void parseEdgeMap(String payload) {
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
        int edge_index = root[i]["index"];
        int edge_start_index = root[i]["start"];
        int edge_end_index = root[i]["end"];
        int edge_weight = root[i]["weight"];
        Node* start_node = node_map[edge_start_index];
        Node* end_node = node_map[edge_end_index];
        edge_map[i] = newEdge(edge_index, start_node, end_node, edge_weight);
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

  switch (topic) {
    case weightTopic:
      parseWeightJson(payload);
      break;
    case nodeTopic:
      parseNodeMap(payload);
      break;
    case edgeTopic:
      parseEdgemap(payload);
      break;
    default:
      Serial.println("Invalid topic");
  }
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
      client.subscribe(nodeTopic);
      client.subscribe(edgeTopic);
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