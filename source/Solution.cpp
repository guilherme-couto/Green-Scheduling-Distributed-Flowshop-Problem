#include "defines.hpp"

Solution::Solution(int n, int m, int F)
{
    this->F = F;
    this->n = n;
    this->m = m;

    this->distribution.resize(F);
    this->V.resize(n);
    this->EC_f.resize(F);
    this->FT_f.resize(F);
    this->job_allocation.resize(n);
    this->p.resize(n);

    for (int i = 0; i < m; i++)
    {
        this->PP.push_back(0);
        this->SP.push_back(0);
    }

    for (int i = 0; i < F; i++)
    {
        this->EC_f[i] = 0;
        this->FT_f[i] = 0;

        Factory* f = new Factory(i, m);
        this->factories.push_back(f);
    }

    for (int i = 0; i < n; i++)
    {
        this->V[i].resize(m);
        this->p[i].resize(m);

        this->job_allocation[i] = -1;
    }

    this->TEC = 0;
    this->TFT = 0;
}

Solution::~Solution()
{
}

float Solution::getTEC() {
    float tft = 0.0;
    for(int i=0; i< this->factories.size(); i++){
        tft+=this->factories[i]->getTEC();
    }

    return tft;
}

float Solution::getTFT() {
    float tft = 0.0;
    for(int i=0; i< this->factories.size(); i++){
        tft+=this->factories[i]->getTFT();
    }

    return tft;
}
void Solution::setDominationRank(int val){
    this->dominationRank = val;
}
void Solution::setDominationCounter(int val){
    this->dominationCounter = val;

}
void Solution::setCrowdingDistance(float val){
    this->crowdingDistance = val;
}

int Solution::getDominationRank(){
    return dominationRank;
}
int Solution::getDominationCounter(){
    return dominationCounter;
}
int Solution::getCrowdingDistance(){
    return crowdingDistance;
}

void Solution::incrementCrowdingDistance(float val){
    this->crowdingDistance +=val;
}

void Solution::incrementDominationCounter(int val) {
    this->dominationCounter += val;
}


Factory* Solution::getFactory(int f_id)
{
    return this->factories[f_id];
}

void Solution::setV(int job_id, int mach_id, float v)
{
    this->V[job_id][mach_id] = v;
}

void Solution::replaceFactory(int f_id, Factory* factory) {
    delete this->factories[f_id];
    this->factories[f_id] = factory;
}


bool Solution::dominates(Solution *other) {
    if(this->getTFT() < other->getTFT() && this->getTEC() < other->getTEC()){
        return true;
    }

    return false;
}

bool Solution::crowdedCompare(Solution *other) {

    if(this->getDominationRank() < other->getDominationRank()){
        return true;
    }else if(this->getDominationRank() == other->getDominationRank()){
        if(this->getCrowdingDistance() > other->getCrowdingDistance()){
            return true;
        }
        return false;
    }
}

void Solution::printSolution()
{
    for(int f = 0; f < this->F; f++)
    {
        cout << "Factory " << f << " -> ";
        
        for(int i = 0; i < this->factories[f]->getTotalJobs(); i++)
        {
            cout << this->factories[f]->getJobs()[i]->getId() << " ";
        }

        cout << endl;
    }
}

