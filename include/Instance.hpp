#ifndef MATRICES_HPP
#define MATRICES_HPP

#include "defines.hpp"
#include "Solution.hpp"

class Instance
{
private:
    int F;                         // Number of factories
    int n;                         // Number of jobs
    int m;                         // Number of machines
    vector<vector<int>> t;         // Standard processing time matrix (line = job, column = machine)
    vector<float> speeds;          // Available processing speeds
    vector<Solution *> population; // Population of solutions
    vector<vector<Solution *>> dominationFronts;
    struct timeval begin, end;
    vector<Solution *> new_individuals;

public:
    Instance(int F, int n, int m, vector<vector<int>> t); // Constructor (F = number of factories, n = number of jobs, m = number of machines)
    ~Instance();

    int getF();
    int get_n();
    int get_m();
    vector<vector<int>> getMatrix_t();
    vector<float> getVectorSpeeds();
    vector<Solution *> getPopulation();
    Solution *getSolution(int i);

    void assignCrowdingDistance();
    vector<vector<Solution *>> fastNonDominatedSort();

    int get_t(int machine_id, int job_id);
    int getSpeed(int i);

    void construtivo();

    Solution *maxSMinTFT();
    Solution *randSMinTFT(int seed);
    Solution *minSMinTEC();
    Solution *randSMinTEC(int seed);

    void balancedRandomSolutionGenerator(int s);
    void totalRandomSolutionGenerator(int s);

    void printPopulation();
    string generatePopulationCSVString();
    void NSGA3NextGen(int seed);
    void NSGA2NextGen(int seed);
    //vector<Solution*> makeNewPop(vector<Solution*> parents, int seed);
    int nMetric();

    vector<Solution *> getParetoFront();
    static vector<vector<Solution*>> fastNonDominatedSort(vector<Solution*> population);

    void INGM(Solution * sol, int seed); // Insertion-based new-individual generation method
    void SNGM(Solution * sol, int seed); // Swap-based new-individual generation method
    void HNGM(Solution * sol, int seed); // Hybrid-based new-individual generation method
    void makenewpop_operators(vector<Solution*> parents, int seed);
};

#endif // MATRICES_HPP