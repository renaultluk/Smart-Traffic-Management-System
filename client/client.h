#ifndef CLIENT_H
#define CLIENT_H

#include <ArduinoJson.h>
#include "graphStructs.h"

#include "DW1000Ranging.h"

#define VEHICLE_ID       1
int dest_length = 0;

struct lamppost {
    DW1000Device* anchor;
    float x;
    float y;
};

lamppost lamppost_arr[3] = {};

DW1000Device* tag;

// *** Utils *** //
Node dequeue(Node queue[], int &queue_size);

char dequeue(char queue[], int &queue_size);

// *** Planning *** //
void planPath(Node start, Node target, Node path[], char direction_queue[]);

void releaseEdge(Edge *edge);

// *** Sensors and Actuation *** //
void motorDrive(int power, int in1, int in2);

void brake();

bool ultRead();

bool linePosition(char direction);

void initUWB();

float nodeDistance(DW1000Device *device, Node *node);


// *** MQTT *** //
JsonArray weightsJson;

void setup_lampposts();

void setup_wifi();

void addToJSON(JsonObject& root, const char key, const char value);

void publishWeightChanges(JsonArray& weightChanges);

#endif
