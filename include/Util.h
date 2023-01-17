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
    static Solution* maxTECSol(vector<Solution*> &v);
    static Solution* minTECSol(vector<Solution*> &v);
    static Solution* maxTFTSol(vector<Solution*> &v);
    static Solution* minTFTSol(vector<Solution*> &v);



    };

#endif //UTIL_H
