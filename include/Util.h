//
// Created by patri on 12/01/2023.
//

#ifndef UTIL_H
#define UTIL_H

#include "defines.hpp"

class Util {

private:
    static vector<Solution*> allocatedSolutions;

public:
    static vector<vector<Solution*>> fastNonDominatedSort(vector<Solution*> population);
    static float DMetric(vector<Solution*> &PF, vector<Solution*> &A);
    static float SMetric(vector<Solution*> &PF, vector<Solution*> &A);
    static float GDMetric(vector<Solution*> &PF, vector<Solution*> &A);
    static float IGDMetric(vector<Solution*> &PF, vector<Solution*> &A);
    static string generateCSV(Factory* factory);
    static void allocate(Solution* sol);
    static void deallocate();


};

#endif //UTIL_H
