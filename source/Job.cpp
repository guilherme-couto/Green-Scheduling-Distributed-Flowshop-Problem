#include "defines.hpp"

float Job::getTotalP(){
    float total = 0.0;

    for(int m=0; m< this->t.size(); m++){
        total += getP(m);
    }

    return total;
}
