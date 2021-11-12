#include "../graphStructs.h"

// *** Planning *** //
void planPath(Node start, Node target, Node path[]);

// *** Sensors and Actuation *** //
void motorDrive(int power, int in1, int in2);

void brake();

bool ultRead();

bool linePosition();