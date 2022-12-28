#include "defines.hpp"

Job::Job(int id)
{
    this->id = id;
}

int Job::getId()
{ 
    return this->id;
}

float Job::getTotalP(){
    float total = 0.0;

    for(int m=0; m< this->t.size(); m++){
        total += getP(m);
    }

    return total;
}

float Job::getP(int j){ return this->t[j]/this->v[j];}

float Job::getV(int j) {
    return this->v[j];
}

void Job::setVForMachine(int j, float v) {
    this->v[j] = v;
}
