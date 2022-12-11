#include "defines.hpp"

float Factory::getTFT() {
    float tft=0.0;
    vector<float> partialFTByJob(this->jobs.size(), 0.0);

    for(int i=0; i<this->m; i++){
        for(int j=0; j<this->jobs.size(); j++){
            if(j==0 ||  partialFTByJob[j] > partialFTByJob[j-1]){
                partialFTByJob[j] = partialFTByJob[j] + this->jobs[j].getP(i);
            }else{
                partialFTByJob[j] = partialFTByJob[j-1] + this->jobs[j].getP(i);;
            }
        }
    }

    for(int i=0; i<partialFTByJob.size(); i++){
        tft+=partialFTByJob[i];
    }

    return tft;
}

float Factory::getTFTTest(vector<Job> testJobsList) {
    float tft=0.0;
    vector<float> partialFTByJob(this->jobs.size(), 0.0);

    for(int i=0; i<this->m; i++){
        for(int j=0; j<this->jobs.size(); j++){
            if(j==0 ||  partialFTByJob[j] > partialFTByJob[j-1]){
                partialFTByJob[j] = partialFTByJob[j] + this->jobs[j].getP(i);
            }else{
                partialFTByJob[j] = partialFTByJob[j-1] + this->jobs[j].getP(i);;
            }
        }
    }

    for(int i=0; i<partialFTByJob.size(); i++){
        tft+=partialFTByJob[i];
    }

    return tft;
}

Factory Factory::minTFTAfterInsertion(Job job) {
    float minTFT = infinityf();
    int minTFTPos = 0;

    vector<Job> auxJobs = this->jobs;
    vector<Job> testJobs = this->jobs;
    vector<Job> minTFTJobs = this->jobs;
    Factory testFactory = *this;

    for(int i=0; i<=this->jobs.size(); i++){
        testJobs.insert(this->jobs.begin()+i, job);

        float testFactoryTFT = this->getTFTTest(testJobs);
        if(testFactoryTFT <=minTFT){
            minTFT = testFactoryTFT;
            minTFTJobs = testJobs;
        }
    }

    testFactory.jobs = minTFTJobs;

    return testFactory;

}