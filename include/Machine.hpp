#ifndef MACHINE_HPP
#define MACHINE_HPP

#include "defines.hpp"

class Machine
{
private:
    int id;           // Machine index
    int factory_id;   // Factory index
    int SP;           // Energy consumption per unit time at stand-by mode
    vector<float> PP; // Energy consumption per unit time running on each processing speed v
    vector<float> t;  // Standard processing time of operation of each job i
    float SEC;        // Total energy consumption running at stand-by mode
    float PEC;        // Total energy consumption running at processing mode

public:
    Machine(int id, int f_id, int s); // Constructor (s = number of processing speeds)
    ~Machine();                       // Destructor

    int getID();
    int getFactoryID();
    int getSP();
    vector<float> getPP();
    float getPP_v(int v);
    vector<float> getT();
    float getT_i(int i);
    float getSEC();
    float getPEC();
};

#endif // MACHINE_HPP