#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "defines.hpp"

class Factory
{
private:
    int id;                 // Factory index
    vector<Job> jobs;       //List of jobs assigned to the factory
    vector<int> jobs_ids;    // Jobs assigned to the factory
    vector<int> jobs_seq;   // Jobs sequence
    int total_jobs;         // Total number of jobs assigned to the factory
    float TEC;              // Total energy consumption of the factory
    float TFT;              // Total flow time of the factory

public:
    Factory(int id){this->id = id;}
    Factory(){}
    ~Factory(){};

    int getID();
    vector<Job> getJobs(){return this->jobs;}
    vector<int> getJobsSeq();
    int getJob_pos(int pos);
    int getTotalJobs();
    float getTEC();
    float getTFT();

    void addJobId(int i);
    void addJob(Job j){this->jobs.push_back(j);}

};

#endif // FACTORY_HPP