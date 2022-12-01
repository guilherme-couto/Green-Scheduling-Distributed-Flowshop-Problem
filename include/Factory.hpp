#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "defines.hpp"

class Factory
{
private:
    int id;                 // Factory index
    vector<int> jobs;       // Jobs assigned to the factory
    vector<int> jobs_seq;   // Jobs sequence
    int total_jobs;         // Total number of jobs assigned to the factory
    float TEC;              // Total energy consumption of the factory
    float TFT;              // Total flow time of the factory

public:
    Factory(int id);
    ~Factory();

    int getID();
    vector<int> getJobs();
    vector<int> getJobsSeq();
    int getJob_pos(int pos);
    int getTotalJobs();
    float getTEC();
    float getTFT();

    void addJob(int i);

};

#endif // FACTORY_HPP