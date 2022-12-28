#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "defines.hpp"

class Factory
{
private:
    int id;                     // Factory index
    vector<Job*> jobs;           // List of jobs assigned to the factory
    vector<int> jobs_ids;       // Jobs assigned to the factory
    vector<int> jobs_seq;       // Jobs sequence ---- não está sendo usado
    int total_jobs;             // Total number of jobs assigned to the factory
    float TEC;                  // Total energy consumption of the factory
    float TFT;                  // Total flow time of the factory
    int m;                      // Number of machines

public:
    Factory(int id, int m);         // Constructor (id = factory index, m = number of machines)
    Factory(){};
    ~Factory();

    int getID();
    vector<Job*> getJobs();
    vector<int> getJobsSeq();
    int getJob_pos(int pos);
    int getTotalJobs();
    float getTEC();
    float getTFT();
    float getTFTTest(vector<Job*> testJobsList);

    void addJobId(int i);
    void addJobAtLastPosition(Job* job);
    void addJobAtPosition(Job* job, int pos);
    void setJobs(vector<Job*> jobs);
    void setMachines(int m);

    Factory* minTFTAfterInsertion(Job* job);
    Factory* minTECAfterInsertion(Job* job);

};

#endif // FACTORY_HPP