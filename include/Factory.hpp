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
    int m;

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
    float getTFTTest(vector<Job> testJobsList);
    void setM(int m){this->m = m;}
    void addJobId(int i);
    void addJob(Job j){this->jobs.push_back(j);}
    Factory minTFTAfterInsertion(Job job);

};

#endif // FACTORY_HPP