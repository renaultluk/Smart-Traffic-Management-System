#include "../graphStructs.h"
#include "DW1000Ranging.h"

// *** Planning *** //
void planPath(Node start, Node target, Node path[], char direction_queue[]);

// *** Sensors and Actuation *** //
void motorDrive(int power, int in1, int in2);

void brake();

bool ultRead();

bool linePosition(char direction);

void initUWB();

float nodeDistance(DW1000Device *device, Node *node);

// *** MQTT *** //
void setup_wifi();