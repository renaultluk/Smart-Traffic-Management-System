#include <WiFiNINA.h> 
#include <PubSubClient.h>
#include "graphStructs.h"
#include "credentials.h"
#include "client.h"

#define CONTROL_FREQ    20

#define NODE_BOUNDS      10

typedef enum {
    STATE_INIT,
    STATE_CONNECTING,
    // STATE_CONNECTED,
    // STATE_DISCONNECTED,
    // STATE_ERROR,
    STATE_PLANNING,
    STATE_FOLLOWING,
    STATE_BRAKING,
    STATE_ARRIVED,
} State;

enum { MANUAL, AUTO } destMode;

Node* start;
Node* target;
Node* path[NUM_NODES];
char direction_queue[NUM_NODES];
int path_length = 0;
int direction_length = 0;
// Node* destinations[NUM_NODES];
// int dest_length = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledState = 0;

void serialInputHandler() {
    char command = Serial.read();
    switch (command) {
        case 'd':
            if (Serial.parseInt() == VEHICLE_ID) {
                target->index = Serial.parseInt();
            }
            break;
    
        case 't':
            if (Serial.parseInt() == VEHICLE_ID) {
                destMode = destMode == AUTO ? MANUAL : AUTO;
            }
            break;
        
        default:
            break;
    }
}

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
        destinations[i] = nullptr;
    }
}

State planFunc() {
    planPath(start, target, path, direction_queue);
    return STATE_FOLLOWING;
}

State followFunc() {
    float cur_time = micros();
    float prev_time = cur_time;
    while (true) {
        if (ultRead()) {
            return STATE_BRAKING;
        }
        cur_time = micros();
        if ((cur_time - prev_time)/1.0e6 > 1.0/CONTROL_FREQ) {
            bool split = false;
            bool haveLine = linePosition(direction_queue[0], split);
            if (split) {
                Edge* tmpEdge = connectEdge(path[0],path[1]);
                releaseEdge(tmpEdge);
                publishWeightChanges(weightsJson);

                dequeue(path, path_length);
                dequeue(direction_queue, direction_length);
                
            }
            if (!haveLine) {
                return STATE_ARRIVED;
            }
            prev_time = cur_time;
        }
    }
}

State brakeFunc() {
    while (ultRead()) {
        brake();
    }
    return STATE_FOLLOWING;
}

State arrivedFunc() {
    reset();
    switch (destMode)
    {
    case MANUAL:
        brake();
        while (!Serial.available()) {
            delay(100);
        };
        serialInputHandler();
        return STATE_PLANNING;
        break;
    
    case AUTO:
        target = destinations[random(NUM_DEST)];
        break;

    default:
        break;
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqttServer, 1883);
    client.setCallback(callback);
}

void loop() {
    State vehicleState = arrivedFunc();

    while (true) {
        if (Serial.available()) {
            serialInputHandler();
        }

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

        default:
            break;
        }
    }
}
