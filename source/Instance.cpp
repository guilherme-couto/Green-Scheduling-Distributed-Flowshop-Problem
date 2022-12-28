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

    for(int i = 0; i < 4; i++)
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

Solution* Instance::minSMinTEC()
{
    Solution* solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job*> jobs(this->get_n());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->get_n(); i++)
    {
        jobs[i] = new Job(i);
        vector<float> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++)
        {
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[0];     // minimum speed
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    // Sort the jobs according to non-descending order of total processing time
    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // Assign the first f jobs to each one of the factories
    for (int k = 0; k < this->getF(); k++)
    {
        solution->getFactory(k)->addJobAtLastPosition(jobs[k]);
    }

    // Remove the assigned jobs from lambda = access the jobs vector from the f position to the end

    // Insert remaining jobs in the position of the factory that can lead to the minimum increse of total energy consumption
    Factory* testFactory;
    float testFactoryTEC;
    float tecVariation;
    float previousTEC;
    Factory* minTECFactory;

    // For each factory f
    for (int i = this->getF(); i < jobs.size(); i++)
    {
        float minIncreaseTEC = INFINITY;
        float minTECPos = 0;

        // Test job j at all the possible positions of PI_k (the factory)
        for (int k = 0; k < this->getF(); k++)
        {
            testFactory = solution->getFactory(k)->minTECAfterInsertion(jobs[i]);
            testFactoryTEC = testFactory->getTEC();
            previousTEC = solution->getFactory(k)->getTEC();
            tecVariation = testFactoryTEC - previousTEC;

            if (tecVariation < minIncreaseTEC)
            {
                minIncreaseTEC = tecVariation;
                minTECFactory = testFactory;
                minTECPos = k;
            }
            else
            {
                delete testFactory;
            }
        }

        // Add the job to the factory that leads to the minimum increase of total energy consumption
        solution->replaceFactory(minTECPos, minTECFactory); // replaces old factory and deletes it
    }

    // TODO: rightShift for each factory

    return solution;
}

void Instance::randomSolutionGenerator(int s)
{
    Xoshiro256plus rand(time(NULL)+s);

    Solution* sol = new Solution(this->n, this->m, this->F); 

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> job_allocated(this->n, false); 

    vector<int> jobs_to_allocate;
    for (size_t i = 0; i < this->n; i++)
    {
        jobs_to_allocate.push_back(i);
    }
    
    // Allocate the jobs equally to the factories
    int f_id = 0;
    while (jobs_to_allocate.size())
    {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job* job = new Job(jobs_to_allocate[random_num]);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++)
        {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
        }

        if (f_id == this->F)
        {
            f_id = 0;
        }

        // Choose a random position to allocate the job
        int random_position = rand.next() % (sol->getFactory(f_id)->getJobs().size() + 1);
        sol->getFactory(f_id)->addJobAtPosition(job, random_position);

        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
        f_id ++;
    }

    // Add solution to population
    this->population.push_back(sol);
}

void Instance::printPopulation()
{
    cout << "\nPOPULATION " << "(with " << this->population.size() << " solutions)" << endl;

    for (size_t i = 0; i < this->population.size(); i++)
    {
        cout << "\nSolution " << i << endl;
        this->population[i]->printSolution();
    }
}