#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "graphStructs.h"
#include "credentials.h"
#include "client.h"

#define CONTROL_FREQ    20

#define NODE_BOUNDS      10

typedef enum {
  STATE_INIT,
  //    STATE_CONNECTING,
  // STATE_CONNECTED,
  STATE_DISCONNECTED,
  // STATE_ERROR,
  STATE_PLANNING,
  STATE_FOLLOWING,
  STATE_BRAKING,
  STATE_ARRIVED,
} State;

State prevState = STATE_INIT;

Node* start = &node_map[15];
Node* target = start;
Node* path[NUM_NODES];
char direction_queue[NUM_NODES];
int path_length = 0;
int direction_length = 0;
int target_index = 15;
// Node* destinations[NUM_NODES];
// int dest_length = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledState = 0;

// void serialInputHandler() {
//     char command = Serial.read();
//     switch (command) {
//         case 'd':
//             if (Serial.parseInt() == VEHICLE_ID) {
//                 target->index = Serial.parseInt();
//             }
//             break;

//         case 't':
//             if (Serial.parseInt() == VEHICLE_ID) {
//                 destMode = destMode == AUTO ? MANUAL : AUTO;
//             }
//             break;

//         default:
//             break;
//     }
// }

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

void reset() {
  start = target;
  target = nullptr;
  for (int i = 0; i < NUM_NODES; i++) {
    path[i] = nullptr;
    direction_queue[i] = '\0';

    node_map[i].visited = false;
    node_map[i].distance = __INT_MAX__;
    node_map[i].prev = nullptr;
  }
}

State planFunc() {
  Serial.println("Entering STATE_PLANNING");
  Serial.print("Planning from node ");
  Serial.print(start->index);
  Serial.print(" to node ");
  Serial.println(target->index);
  if (!client.connected()) {
    prevState = STATE_PLANNING;
    return STATE_DISCONNECTED;
  }
  planPath(start, target, path, direction_queue);
  publishWeightChanges(weightsJson);      // JSON pending
  return STATE_FOLLOWING;
}

State followFunc() {
  Serial.println("Entering STATE_FOLLOWING");
  float cur_time = micros();
  float prev_time = cur_time;
  while (true) {
    if (!client.connected()) {
      prevState = STATE_FOLLOWING;
      return STATE_DISCONNECTED;
    }
    if (ultRead()) {
//      return STATE_BRAKING;
    }
    cur_time = micros();
    if ((cur_time - prev_time) / 1.0e6 > 1.0 / CONTROL_FREQ) {
      bool split = false;
      bool haveLine = linePosition(direction_queue[0], split);
      Serial.println("Finished line reading");
      if (split) {
        Edge* tmpEdge = connectEdge(path[0], path[1]);
//        releaseEdge(tmpEdge);
//        publishWeightChanges(weightsJson);

        Serial.println("split detected");
        dequeue(path, path_length);
        dequeue(direction_queue, direction_length);
        split = false;
      }
      if (!haveLine) {
//        return STATE_ARRIVED;
      }
      prev_time = cur_time;
    }
  }
}

State brakeFunc() {
  Serial.println("Entering STATE_BRAKING");
  while (ultRead()) {
    if (!client.connected()) {
      prevState = STATE_BRAKING;
      return STATE_DISCONNECTED;
    }
    brake();
  }
  return STATE_FOLLOWING;
}

State arrivedFunc() {
  Serial.println("Entering STATE_ARRIVED");
  reset();
  if (!client.connected()) {
    prevState = STATE_ARRIVED;
    return STATE_DISCONNECTED;
  }
  int new_target_index = target_index;
  while (new_target_index == target_index) {
    Serial.println("Generating new destination");
    new_target_index = random(NUM_DEST);
  }
  target = destinations[new_target_index];
  target_index = new_target_index;
  return STATE_PLANNING;
}

State disconnectedFunc() {
  reconnect();
  return prevState;
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  while (!Serial);
  createNewJSON();
  initMap();
//  start = &node_map[17];
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {
  State vehicleState = arrivedFunc();

  while (true) {
    //        if (Serial.available()) {
    //            serialInputHandler();
    //        }

    switch (vehicleState)
    {
      case STATE_PLANNING:
        vehicleState = planFunc();
        break;

      case STATE_FOLLOWING:
        vehicleState = followFunc();
        break;

      case STATE_BRAKING:
        vehicleState = brakeFunc();
        break;

      case STATE_ARRIVED:
        vehicleState = arrivedFunc();
        break;

      case STATE_DISCONNECTED:
        vehicleState = disconnectedFunc();
        break;

      default:
        break;
    }
  }
}
