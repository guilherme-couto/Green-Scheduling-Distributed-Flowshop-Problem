#ifndef MATRICES_HPP
#define MATRICES_HPP

#include "defines.hpp"

class Matrices
{
private:
    int F;                              // Number of factories
    int n;                              // Number of jobs
    int m;                              // Number of machines
    vector<vector<int>> t;              // Standard processing time matrix (line = machine, column = job)
    vector<float> speeds;               // Available processing speeds

public:
    Matrices(int F, int n, int m, vector<vector<int>> t);      // Constructor (F = number of factories, n = number of jobs, m = number of machines)
    ~Matrices();

    int getF();
    int get_n();
    int get_m();
    vector<vector<int>> getMatrix_t();
    vector<float> getVectorSpeeds();

    int get_t(int machine_id, int job_id);
    int getSpeed(int i);
};

Matrices::Matrices(int F, int n, int m, vector<vector<int>> t)
{
    this->F = F;
    this->n = n;
    this->m = m;
    this->t = t;

    this->speeds.push_back(1.0);
    this->speeds.push_back(1.3);
    this->speeds.push_back(1.55);
    this->speeds.push_back(1.75);
    this->speeds.push_back(2.1);
}

Matrices::~Matrices()
{
}




#endif // MATRICES_HPP