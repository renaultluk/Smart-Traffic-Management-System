#ifndef CLIENT_H
#define CLIENT_H

#include <ArduinoJson.h>
#include "graphStructs.h"

#define VEHICLE_ID       1
int dest_length = 0;

// *** Utils *** //
void dequeue(Node* queue[], int &queue_size);

char dequeue(char queue[], int &queue_size);

// *** Planning *** //
void planPath(Node* start, Node* target, Node* path[], char direction_queue[]);

void releaseEdge(Edge *edge);

// *** Sensors and Actuation *** //
void motorDrive(int power, int in1, int in2);

void brake();

bool ultRead();

bool linePosition(char direction, bool &split);


// *** MQTT *** //
JsonArray weightsJson;

void setup_lampposts();

void setup_wifi();

void reconnect();

void createNewJSON();

void addToJSON(JsonObject& root, const char key, const char value);

void publishWeightChanges(JsonArray& weightChanges);

void callback(char* topic, byte* payload, unsigned int length);

#endif
