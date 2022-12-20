#ifndef MATRICES_HPP
#define MATRICES_HPP

#include "defines.hpp"
#include "Solution.hpp"

class Instance
{
private:
    int F;                              // Number of factories
    int n;                              // Number of jobs
    int m;                              // Number of machines
    vector<vector<int>> t;              // Standard processing time matrix (line = machine, column = job)
    vector<float> speeds;               // Available processing speeds
    vector<Solution*> population;        // Population of solutions

public:
    Instance(int F, int n, int m, vector<vector<int>> t);      // Constructor (F = number of factories, n = number of jobs, m = number of machines)
    ~Instance();

    int getF();
    int get_n();
    int get_m();
    vector<vector<int>> getMatrix_t();
    vector<float> getVectorSpeeds();
    vector<Solution*> getPopulation();
    Solution* getSolution(int i);

    int get_t(int machine_id, int job_id);
    int getSpeed(int i);

    void construtivo();

    Solution* maxSMinTFT();
    Solution* randSMinTFT(int seed);
    void randomSolutionGenerator(int s);

    void printPopulation();
    string generateSolutionsString();
};






#endif // MATRICES_HPP