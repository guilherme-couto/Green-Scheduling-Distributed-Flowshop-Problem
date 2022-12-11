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
    vector<float> t; // Standard processing time of operation on each machine j
    vector<float> v; // Processing speed of this job on each machine
    float C;         // Completion time

public:
    Job(int id){this->id=id;}
    Job(){}
    ~Job(){};

    float getP(int j){ return t[j]/v[j];}   //Get actual processing time of this job on machine j
    float getTotalP();
    int getID();
    int getFactoryID();
    int getPosition();
    vector<float> getT();
    float getT_j(int j);
    float getC();

    void setFactoryID(int id);
    void setPosition(int pos);
    void setC(float C);
    void setT(vector<float> t){this->t = t;}
    void setV(vector<float> v){this->v = v;}
};

#endif // JOB_HPP