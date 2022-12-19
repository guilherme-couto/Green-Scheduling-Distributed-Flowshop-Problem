#include "defines.hpp"

vector<float> Factory::speeds(1, 0.0);

Factory::Factory(int id, int m)
{
    this->id = id;
    this->m = m;
    this->total_jobs = 0;
    this->TEC = 0.0;
    this->TFT = 0.0;
}

Factory::~Factory() {
    /*for(int i=0; i< this->jobs.size(); i++){
        delete jobs[i];
    }*/
}

void Factory::setSpeeds(vector<float> s) {
    Factory::speeds = s;
}

vector<Job*> Factory::getJobs()
{
    return this->jobs;
}

int Factory::getTotalJobs()
{
    return this->total_jobs;
}


float Factory::getTEC() {
    return 0.0;
}
float Factory::getTFT() {
    float tft=0.0;
    vector<float> partialFTByJob(this->jobs.size(), 0.0);

    for(int i=0; i<this->m; i++){
        for(int j=0; j<this->jobs.size(); j++){
            if(j==0 ||  partialFTByJob[j] > partialFTByJob[j-1]){
                partialFTByJob[j] = partialFTByJob[j] + this->jobs[j]->getP(i);
            }else{
                partialFTByJob[j] = partialFTByJob[j-1] + this->jobs[j]->getP(i);;
            }
        }
    }

    for(int i=0; i<partialFTByJob.size(); i++){
        tft+=partialFTByJob[i];
    }

    return tft;
}

float Factory::getTFTTest(vector<Job*> testJobsList) {
    float tft=0.0;
    vector<float> partialFTByJob(testJobsList.size(), 0.0);

    for(int i=0; i<this->m; i++){
        for(int j=0; j<testJobsList.size(); j++){
            if(j==0 ||  partialFTByJob[j] > partialFTByJob[j-1]){
                partialFTByJob[j] = partialFTByJob[j] + testJobsList[j]->getP(i);
            }else{
                partialFTByJob[j] = partialFTByJob[j-1] + testJobsList[j]->getP(i);;
            }
        }
    }

    for(int i=0; i<partialFTByJob.size(); i++){
        tft+=partialFTByJob[i];
    }

    return tft;
}

Factory* Factory::minTFTAfterInsertion(Job* job) {
    float tftBeforeInsertion = this->getTFT();
    float minIncreaseTFT = INFINITY;
    float tftVariation;
    vector<Job*> auxJobs = this->jobs;
    vector<Job*> testJobs = this->jobs;
    vector<Job*> minTFTJobs = this->jobs;
    Factory* testFactory = new Factory();
    *testFactory = *this;

    for(int i=0; i<=this->jobs.size(); i++){

        testJobs = this->jobs;
        testJobs.insert(testJobs.begin()+i, job);
        float testFactoryTFT = this->getTFTTest(testJobs);

        tftVariation = testFactoryTFT - tftBeforeInsertion;
        if(tftVariation < minIncreaseTFT){
            minIncreaseTFT = tftVariation;
            minTFTJobs = testJobs;
        }
    }

    testFactory->jobs = minTFTJobs;

    return testFactory;

}

void Factory::addJobAtLastPosition(Job* job)
{

    for(int i = 0; i < this->jobs.size(); i++)
    {    
        // if job already exists in factory, do nothing
        if(this->jobs[i]->getId() == job->getId())
            return;
    }

    this->jobs.push_back(job);
    this->total_jobs++;
}

void Factory::addJobAtPosition(Job* job, int pos)
{
    for(int i = 0; i < this->jobs.size(); i++)
    {    
        // if job already exists in factory, do nothing
        if(this->jobs[i]->getId() == job->getId())
            return;
    }
cout << "total jobs: " << this->total_jobs << endl;
    this->jobs.insert(this->jobs.begin()+pos, job);
    this->total_jobs++;
}

void Factory::speedDown() {
    Job *job;
    float previousTFT = this->getTFT();
    float previousSpeed;

    for (int i = 0; i < this->jobs.size() - 1; i++) {
        job = this->jobs[i];

        for (int j = 1; j < this->m; j++) {
            previousSpeed = job->getV()[j];

            for (int v = this->speeds.size() - 1; v > 1; v--) {//todo: pegar vetor de velocidades possíveis, talvez usar um campo estático na Factory
                job->setVForMachine(j, this->speeds[v - 1]); //diminui para a próxima velocidade
                if (this->getTFT() > previousTFT) {
                    job->setVForMachine(j, previousSpeed); //retorna à velocidade anterior
                }
            }
        }
    }
}