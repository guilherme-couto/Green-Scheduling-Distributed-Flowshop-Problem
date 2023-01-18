#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "defines.hpp"

class Solution
{
private:
    vector<vector<int>> distribution; // Sequence of jobs in each factory
    vector<vector<float>> V;          // Speed matrix of each job in each machine
    float TEC;                        // Total energy consumption
    float TFT;                        // Total flow time
    vector<float> EC_f;               // Energy consumption of each factory
    vector<float> FT_f;               // Flow time of each factory
    vector<int> job_allocation;       // Factory id of each job
    vector<Factory *> factories;
    int n;
    int m;
    int F;                   // List of factories
    vector<vector<float>> p; // Actual processing time of each job in each machine
    vector<float> PP;        // Energy consumption when processing of each machine
    vector<float> SP;        // Energy consumption when stand-by of each machine

    // auxiliar attributes
    int dominationRank;
    int dominationCounter;
    float crowdingDistance;

public:
    Solution(int n, int m, int F); // Constructor (n = number of jobs, m = number of machines, F = number of factories)
    Solution(Solution* sol);
    ~Solution();

    vector<vector<int>> getDistribution();
    vector<vector<float>> getV();
    float getTEC();
    float getTFT();
    float getTFTUsingMatrix();
    vector<int> getAllJobsAllocation();
    int getJobAllocation(int job_id);
    Factory *getFactory(int f_id);
    int getNumFactories();

    bool dominates(Solution *other); // true if this instance dominates other
    bool crowdedCompare(Solution *other);

    void replaceFactory(int f_id, Factory *factory);
    void setSequence(int f_id, vector<int> seq);
    void setV(int job_id, int mach_id, float v);
    void setEC_f(int f_id, float ec);
    void setFT_f(int f_id, float ft);
    void setJobAllocation(int job_id, int f_id);
    void setDominationRank(int val);
    int getDominationRank();
    void setDominationCounter(int val);
    int getDominationCounter();
    void setCrowdingDistance(float val);
    float getCrowdingDistance();
    void incrementCrowdingDistance(float val);
    void incrementDominationCounter(int val);
    vector<Factory*> getFactories();

    // Operators
    void speedUp(int f_id);
    void randSpeedUp(int f_id);
    void speedDown(int f_id);
    void randSpeedDown(int f_id);
    void rightShift(int f_id);
    void insert(int from_f_id, int to_f_id, Job *job, int pos);
    void swap(int f1_id, int f2_id, Job *job1, Job *job2);

    // Auxiliar functions
    void printSolution();
};

#endif // SOLUTION_HPP