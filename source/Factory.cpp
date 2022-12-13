#include "defines.hpp"

Factory::Factory(int id, int m)
{
    this->id = id;
    this->m = m;
    this->total_jobs = 0;
    this->TEC = 0.0;
    this->TFT = 0.0;
}

vector<Job*> Factory::getJobs()
{
    return this->jobs;
}

int Factory::getTotalJobs()
{
    return this->total_jobs;
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

Factory* Factory::minTFTAfterInsertion(Job* job) {
    float minTFT = INFINITY;
    int minTFTPos = 0;

    vector<Job*> auxJobs = this->jobs;
    vector<Job*> testJobs = this->jobs;
    vector<Job*> minTFTJobs = this->jobs;
    Factory* testFactory = this;

    for(int i=0; i<=this->jobs.size(); i++){
        testJobs.insert(testJobs.begin()+i, job);

        float testFactoryTFT = this->getTFTTest(testJobs);
        if(testFactoryTFT <=minTFT){
            minTFT = testFactoryTFT;
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