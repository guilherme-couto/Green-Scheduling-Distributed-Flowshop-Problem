#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "defines.hpp"

class Factory
{
private:
    int id;               // Factory index
    vector<Job *> jobs;   // List of jobs assigned to the factory
    vector<int> jobs_ids; // Jobs assigned to the factory
    vector<int> jobs_seq; // Jobs sequence ---- não está sendo usado
    int total_jobs;       // Total number of jobs assigned to the factory
    float TEC;            // Total energy consumption of the factory
    float TFT;            // Total flow time of the factory
    int m;                // Number of machines
    static vector<float> speeds;
    bool jobs_start_times_initialized;
    bool TFTChanged =true;
    bool TECChanged =true;

public:
    Factory(int id, int m); // Constructor (id = factory index, m = number of machines)
    Factory(Factory* f);
    Factory(){};
    ~Factory();

    int getID();
    vector<Job *> getJobs();
    vector<int> getJobsSeq();
    int getJobPosAtSeq(int job_id);
    int getTotalJobs();
    float getTEC();
    float getTFT();
    float getTFTTest(vector<Job *> testJobsList);
    float getTFTAfterStartTimesSet();
    int getNumJobs();
    int getNumMachines();
    Job* getJob(int id);

    void addJobId(int i);
    void addJobAtLastPosition(Job *job);
    void addJobAtPosition(Job *job, int pos);
    void setJobs(vector<Job *> jobs);
    void setMachines(int m);
    void setTECTFTChanged();
    void setVForJobMachine(int job, int machine, float speed);

    void randSpeedDown(int seed);
    void speedDown();
    static void setSpeeds(vector<float> s);

    Factory *minTFTAfterInsertion(Job *job);
    Factory *minTECAfterInsertion(Job *job);

    // Auxiliar functions
    void initializeJobsStartTimes();
    void rightShift();
    float getTECAfterStartTimesSet();
    void updateTEC();
    void speedUp();
    void randSpeedUp(int seed);
    void removeJob(int job_id);
    void insertJobAtPos(Job *job, int pos);
    string generateCSV();
    void clearJobs();
};

#endif // FACTORY_HPP