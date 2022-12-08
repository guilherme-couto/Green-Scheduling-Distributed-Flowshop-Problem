#include "defines.hpp"

Instance::Instance(int F, int n, int m, vector<vector<int>> t)
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

Instance::~Instance()
{
}

void Instance::construtivo()
{

    Xoshiro256plus rand(time(NULL));
    vector<int> prob{1, 1, 1, 1, 1, 1, 1, 1, 0, 0}; // 80%

    int random_number = rand.next() % prob.size();
    
}