#include "defines.hpp"

Instance::Instance(int F, int n, int m, vector<vector<int>> t) {
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


Instance::~Instance() {
}

int Instance::get_t(int machine_id, int job_id) {
    return t[machine_id][job_id];
}

void Instance::construtivo() {

    Xoshiro256plus rand(time(NULL));
    vector<int> prob{1, 1, 1, 1, 1, 1, 1, 1, 0, 0}; // 80%

    int random_number = rand.next() % prob.size();

}

bool compareJobsByTotalProcessingTime(Job a, Job b) {
    return a.getTotalP() < b.getTotalP();
}


Solution Instance::maxSMinTFT() {
    Solution solution = Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job> jobs(this->get_n());
    vector<Factory> factories(this->getF());
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++) {
        jobs[i] = Job(i);
        vector<float> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++) {
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[highestSpeed];
        }
        jobs[i].setT(jobTimeByMachine);
        jobs[i].setV(jobSpeedByMachine);
    }


    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)


    for (int i = 0; i < this->getF(); i++) { //assign the first f jobs to each one of the factories
        factories[i] = Factory(i);
        factories[i].addJob(jobs[i]);
        factories[i].setM(this->get_m());
    }

    //remove the assigned jobs from lambda

    Factory testFactory;
    float testFactoryTFT;
    Factory minTFTFactory;
    float minTFT = infinityf();
    float minTFTPos = 0;


    //for each factory f
    for (int j = this->getF(); j < jobs.size(); j++) {    //test job j at all the possible positions of PI_k (the factory)
        for (int f = 0; f < this->getF(); f++) {
            testFactory = factories[f].minTFTAfterInsertion(jobs[j]);
            testFactoryTFT = testFactory.getTFT();
            if (testFactoryTFT < minTFT) {
                minTFT = testFactoryTFT;
                minTFTFactory = testFactory;
                minTFTPos = f;
            }
        }

        factories[minTFTPos] = minTFTFactory;
    }



    return solution;
}