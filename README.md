# Smart Traffic Management System

### Introduction
We are planning to use UWB radio network as well as MQTT to create communication between vehicles to prevent traffic jams and traffic accidents.

---
### TODOs
- Implement UWB radio network and check if the vehicle has passed the node through getting its coordinates on the map.
- Implement MQTT communication between the vehicle and the server.
- Implement server code to add weight to the edge map and send commands to vehicles.
---
### Usage
1. Start the server.
2. Initialize each of the vehicles by typing in the following syntax in the server serial monitor:
```
i <VEHICLE_ID> <STARTING_NODE_INDEX>
```
3. The vehicles are set by default to receive destinations instructions manually. To enter the destination manually, type in the following syntax in the server serial monitor:
```
d <VEHICLE_ID> <DESTINATION_NODE_INDEX>
```
4. To toggle whether the vehicles receive destinations manually or generate new destinations automatically, type in the following syntax in the server serial monitor:
```
t <VEHICLE_ID>
```
---
### Setup
The map can be seen as a weighted graph, where points of significance (e.g. intersections, destinations) are the nodes, while the roads are the edges. The edges are weighted by their physical lengths and distance between the nodes. Additional weight will be added to the edges if a vehicle plans to pass through it.

Three “smart lamp posts” will be placed around the map, so that vehicles can calculate their physical location through getting the distance between themselves and the lamp posts using the UWB radio network, and doing triangulation.

---
### Operation Flow
##### 1. Pre-journey planning
The vehicle will plan out its journey before it starts driving. It first searches paths possible using Dijkstra’s Algorithm, by tracing each path along the nodes, and returning the shortest distance from the initial node to all of the other nodes. It ultimately reconstructs the path  through tracing the target's immediate predecessor, and the immediate predecessor of that, and so on; and saves the steering directions in a queue.

After it finishes planning, its path will be appended on a list on the server through MQTT, where additional weights will be added onto each edge that the vehicle will pass through, to deter other vehicles from occupying the same road.

##### 2. Driving
The vehicle drives along the line on the road, where the infrared line sensor will guide the vehicle to follow closely with the PID controller, whereby the vehicle will calculate a weighted sum of the data from each IR sensor inside the line sensor, and offset the PWM of the motors driving the wheels accordingly.

##### 3. Passing a node
When the vehicle passes a node, the following operation will be carried out:
1. Steering: When facing a crossroads, the line sensor will either scan from left to right (if going left) or from right to left (if going right) and only read the first line it encounters. By carrying out PID with only the relevant line, the vehicle will steer without affecting the overall speed of the vehicle.
2. Updating the direction queue: The first element of the direction queue (the operation it just carried out) will be dequeued, so the vehicle will check for the next element in the queue next time.
3. Reporting to the server: the vehicle will report to the server, with its vehicle ID and physical location based on UWB signals, so as to better do traffic analysis.
4. Updating the map: The edge that the vehicle just passed through will be removed from the list on the server, and the additional weight on said edge will be subtracted, freeing up the path for other vehicles.

---
### Project Contributors
- Renault Luk
- Kelly Lai
