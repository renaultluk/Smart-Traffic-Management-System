#include "../graphStructs.h"
#include "DW1000Ranging.h"

#define VEHICLE_ID       1
int dest_length;

// *** Utils *** //
Node dequeue(Node queue[], int &queue_size);

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

void setup_wifi();

void addToJSON(JsonObject& root, const char key, const char value);

void publishWeightChanges(JsonArray& weightChanges);