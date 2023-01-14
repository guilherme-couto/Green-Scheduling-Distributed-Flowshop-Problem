//
// Created by patri on 12/01/2023.
//

#ifndef UTIL_H
#define UTIL_H

#include "defines.hpp"

class Util {

public:
    static vector<vector<Solution*>> fastNonDominatedSort(vector<Solution*> population);
    static float DMetric(vector<Solution*> &PF, vector<Solution*> &A);
    static float SMetric(vector<Solution*> &PF, vector<Solution*> &A);
    string generateCSV(Factory* factory);

};

#endif //UTIL_H
