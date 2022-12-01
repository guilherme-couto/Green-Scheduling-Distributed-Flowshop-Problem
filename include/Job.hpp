#ifndef JOB_HPP
#define JOB_HPP

#include "defines.hpp"

class Job
{
private:
    int id;          // Job index
    int factory_id;  // Factory index
    int position;    // Position in processing flow
    vector<float> t; // Standard processing time of operation on each machine j
    float C;         // Completion time

public:
    Job(int id);
    ~Job();

    int getID();
    int getFactoryID();
    int getPosition();
    vector<float> getT();
    float getT_j(int j);
    float getC();

    void setFactoryID(int id);
    void setPosition(int pos);
    void setC(float C);
};

#endif // JOB_HPP