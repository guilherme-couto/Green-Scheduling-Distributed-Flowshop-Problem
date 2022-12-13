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
    for(int i=0; i< this->population.size(); i++){
        delete this->population[i];
    }
}

int Instance::getF() {
    return this->F;
}

int Instance::get_n() {
    return this->n;
}

int Instance::get_m() {
    return this->m;
}

int Instance::get_t(int machine_id, int job_id) {
    return t[machine_id][job_id];
}

void Instance::construtivo() {

    for(int i = 0; i < 6; i++)
    {
        this->randomSolutionGenerator(i);
    }

}

bool compareJobsByTotalProcessingTime(Job* a, Job* b) {
    return a->getTotalP() < b->getTotalP();
}


Solution* Instance::maxSMinTFT() {
    Solution* solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job*> jobs(this->get_n());
    //vector<Factory*> factories(this->getF());
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++) {
        jobs[i] = new Job(i);
        vector<float> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++) {
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[highestSpeed];
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }


    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)


    for (int i = 0; i < this->getF(); i++) { //assign the first f jobs to each one of the factories
        //factories[i] = new Factory(i, this->m);
        solution->getFactory(i)->addJobAtLastPosition(jobs[i]);
    }

    //remove the assigned jobs from lambda

    Factory* testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;
    Factory* minTFTFactory;

    //for each factory f
    for (int j = this->getF(); j < jobs.size(); j++) {//test job j at all the possible positions of PI_k (the factory)
        float minIncreaseTFT = INFINITY;
        float minTFTPos = 0;

        for (int f = 0; f < this->getF(); f++) {
            testFactory = solution->getFactory(f)->minTFTAfterInsertion(jobs[j]);
            testFactoryTFT = testFactory->getTFT();
            previousTFT = solution->getFactory(f)->getTFT();
            tftVariation = testFactoryTFT - previousTFT;

            if (tftVariation < minIncreaseTFT) {
                minIncreaseTFT = tftVariation;
                minTFTFactory = testFactory;
                minTFTPos = f;
            }else{
                delete testFactory;
            }
        }

        //factories[minTFTPos] = minTFTFactory;
        solution->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    //todo: speedDown for each factory

    return solution;
}

void Instance::randomSolutionGenerator(int s)
{
    Xoshiro256plus rand(time(NULL)+s);

    Solution* sol = new Solution(this->n, this->m, this->F); cout<<"112"<<endl;

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> job_allocated(this->n, false); cout<<"115"<<endl;

    // Allocate one job for each factory
    for (int i = 0; i < this->F; i++)
    {
        int random_num = rand.next() % this->n;
        
        while (job_allocated[random_num])
        {
            random_num = rand.next() % this->n;
        }
        
        Job* job = new Job(random_num);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++)
        {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
        }

        sol->getFactory(i)->addJobAtLastPosition(job);
        job_allocated[random_num] = true;
    }

    // Allocate the remaining jobs
    for (int i = 0; i < this->n; i++)
    {
        if (!job_allocated[i])
        {
            Job* job = new Job(i);

            // Set a random speed for each machine
            for (int j = 0; j < this->m; j++)
            {
                int random_num = rand.next() % this->speeds.size();
                sol->setV(job->getId(), j, this->speeds[random_num]);
            }

            // Choose a random factory to allocate the job
            int random_factory = rand.next() % this->F;

            // Choose a random position to allocate the job
            int random_position = rand.next() % (sol->getFactory(random_factory)->getJobs().size() + 1);
            sol->getFactory(random_factory)->addJobAtPosition(job, random_position);
            job_allocated[i] = true;
        }
    }

    // Add solution to population
    this->population.push_back(sol);
}

void Instance::printPopulation()
{
    cout << "\nPOPULATION " << "(with " << this->population.size() << " solutions) ---------------\n" << endl;

    for (size_t i = 0; i < this->population.size(); i++)
    {
        cout << "\nSolution " << i << endl;
        this->population[i]->printSolution();
    }
}