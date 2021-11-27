#include <iostream>
using namespace std;

/**
 * @brief 
 * when program starts, fetch node_map and edge_map -> publish to isdn4000g/stms/nodes and isdn4000g/stms/edges
 * allow user to input command, 3 tasks
 * 1. initialize the vehicles by inputting their starting position
 * 2. toggle between manual mode and automatic mode
 * 3. input new destination
 * 
 * 
 */

// read node_map, edge_map

int number;
int vehicle_id;
int start_node_index;
int target_node_index;

void input_destination(){
    while (true){
        cout <<"Commands:"<<endl;
        cout <<"1. initialize vehicle: init <VEHICLE_ID> <START_NODE_INDEX>"<<endl;
        cout <<"2. set vehicle destination: set <VEHICLE_ID> <TARGET_NODE_INDEX>"<<endl;
        cout <<"3. toggle vehicle mode: toggle <VEHICLE_ID>"<<endl;
        cout <<"Please enter corresponding number: "<<endl;
        cin >> number;

        if (number == 1){
            cout <<"Please enter <VEHICLE_ID> and <START_NODE_INDEX>. "<<endl;
            cin >> vehicle_id;
            cin >> start_node_index;
            if ( vehicle_id == 1){
                self.index = start_node_index;
                
                //has its corresponding x,y

            }

            if ( vehicle_id == 2){
                self.index = start_node_index;
                //has its corresponding x,y
                
            }

            if ( vehicle_id == 3){
                self.index = start_node_index;
                //has its corresponding x,y
                
            }
            
        }

        if (number == 2){
            cout <<"Please enter <VEHICLE_ID> and <TARGET_NODE_INDEX>. "<<endl;
            cin >> vehicle_id;
            cin >> target_node_index;
            
        }

        if (number == 3){
            cout <<"Please enter <VEHICLE_ID>. "<<endl;
            cin >> vehicle_id;
            
        }

        if (!(number == 1)||!(number == 2)||!(number == 3)){
            cerr<<"Error: Invalid number of parameters. Please try again"<<endl;
            break;
        }



    }
}