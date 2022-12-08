#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "defines.hpp"

class Solution
{
private:
    vector<vector<int>> distribution;   // Sequence of jobs in each factory
    vector<vector<float>> V;            // Speed matrix of each job in each machine
    float TEC;                          // Total energy consumption  
    float TFT;                          // Total flow time  
    vector<float> EC_f;                 // Energy consumption of each factory
    vector<float> FT_f;                 // Flow time of each factory    
    vector<int> job_allocation;         // Factory id of each job

    vector<Factory> factories;          // List of factories

public:
    Solution(int n, int m, int F);      // Constructor (n = number of jobs, m = number of machines, F = number of factories)  
    ~Solution();

    vector<vector<int>> getDistribution();
    vector<vector<float>> getV();
    float getTEC();
    float getTFT();
    vector<int> getAllJobsAllocation();
    int getJobAllocation(int job_id);

    void setSequence(int f_id, vector<int> seq);
    void setV(int i, int j, float v);
    void setEC_f(int f_id, float ec);
    void setFT_f(int f_id, float ft);
    void setJobAllocation(int job_id, int f_id);

    // Operators
    void speedUp(int f_id);
    void randSpeedUp(int f_id);
    void speedDown(int f_id);
    void randSpeedDown(int f_id);
    void rightShift(int f_id);
    void insert(int f1_id, int f2_id, int job1_id, int job2_id);
    void swap(int f1_id, int f2_id, int job1_id, int job2_id);

};

#endif // SOLUTION_HPP