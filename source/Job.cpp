#include "defines.hpp"


Job::Job(Job *j) {
    this->id = j->id;
    this->factory_id = j->factory_id;
    //this->position = j->position;
    this->t = j->t;
    this->v = j->v;
    this->C = j->C;
    this->m = j->m;
    this->start_times = j->start_times;
}

Job::Job(int id, int m)
{
    this->id = id;
    this->t.resize(m);
    this->v.resize(m);
    this->start_times.resize(m);
}

int Job::getId()
{
    return this->id;
}

float Job::getTotalP()
{
    float total = 0.0;

    for (int m = 0; m < this->t.size(); m++)
    {
        total += getP(m);
    }

    return total;
}

float Job::getP(int j) { return this->t[j] / this->v[j]; }

float Job::getV(int j)
{
    return this->v[j];
}

void Job::setVForMachine(int j, float v)
{
    this->v[j] = v;
}

float Job::getT_j(int j)
{
    return this->t[j];
}

void Job::setStartTime(int j, float start_time)
{
    this->start_times[j] = start_time;
}

float Job::getStartTime(int j)
{
    return this->start_times[j];
}

vector<float> Job::getStartTimes()
{
    return this->start_times;
}
