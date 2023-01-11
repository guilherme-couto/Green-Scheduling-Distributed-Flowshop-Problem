#ifndef JOB_HPP
#define JOB_HPP

#include "defines.hpp"
using namespace std;

class Job
{
private:
    int id;          // Job index
    int factory_id;  // Factory index
    int position;    // Position in processing flow
    vector<int> t;   // Standard processing time of operation on each machine j
    vector<float> v; // Processing speed of this job on each machine
    float C;         // Completion time
    int m;           // Number of machines
    vector<float> start_times; // Start time on each machine

public:
    Job(int id, int m); // Constructor (id = job index, m = number of machines
    Job(Job* j);
    ~Job(){};

    float getP(int j); // Get actual processing time of this job on machine j
    float getV(int j); // Get actual processing speed of this job on machine j
    float getTotalP();
    int getId();
    int getFactoryId();
    int getPosition();
    vector<float> getT();
    float getT_j(int j);
    float getC();

    void setFactoryID(int id);
    void setPosition(int pos);
    void setC(float C);
    void setT(vector<int> t) { this->t = t; }
    void setV(vector<float> v) { this->v = v; }
    void setVForMachine(int j, float v);

    void setStartTime(int j, float start_time);
    float getStartTime(int j);
    vector<float> getStartTimes();
};

#endif // JOB_HPP