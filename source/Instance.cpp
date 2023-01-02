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
    Factory::setSpeeds(this->speeds);
}


Instance::~Instance() {
    for (int i = 0; i < this->population.size(); i++) {
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

    for (int i = 0; i < 4; i++) {
        this->randomSolutionGenerator(i);
    }

}

bool compareJobsByTotalProcessingTime(Job *a, Job *b) {
    return a->getTotalP() < b->getTotalP();
}

bool compareSolutionsByTFT(Solution *a, Solution *b) {
    return a->getTFT() < b->getTFT();
}

bool compareSolutionsByTEC(Solution *a, Solution *b) {
    return a->getTEC() < b->getTEC();
}

Solution *Instance::maxSMinTFT() {
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++) {
        jobs[i] = new Job(i);
        vector<int> jobTimeByMachine(this->get_m());
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

    Factory *testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;
    Factory *minTFTFactory;

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
            } else {
                delete testFactory;
            }
        }

        //factories[minTFTPos] = minTFTFactory;
        solution->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->getF(); i++) {
        solution->getFactory(i)->speedDown();
    }
    this->population.push_back(solution);

    return solution;
}

Solution *Instance::randSMinTFT(int seed) {
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());
    Xoshiro256plus rand(time(NULL) + seed);
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++) {

        jobs[i] = new Job(i);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++) {
            int randomIndex = rand.next() % 5;
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[randomIndex];
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

    Factory *testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;
    Factory *minTFTFactory;

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
            } else {
                delete testFactory;
            }
        }

        //factories[minTFTPos] = minTFTFactory;
        solution->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->getF(); i++) {
        solution->getFactory(i)->speedDown();
    }
    this->population.push_back(solution);

    return solution;
}

void Instance::randomSolutionGenerator(int s) {
    Xoshiro256plus rand(time(NULL) + s);

    Solution *sol = new Solution(this->n, this->m, this->F);

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> job_allocated(this->n, false);

    vector<int> jobs_to_allocate;
    for (size_t i = 0; i < this->n; i++) {
        jobs_to_allocate.push_back(i);
    }

    // Allocate the jobs equally to the factories
    int f_id = 0;
    while (jobs_to_allocate.size()) {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num]);
        job->setT(this->t[jobs_to_allocate[random_num]]);

        vector<float> v(this->get_m()); //todo: colocar isso em algum construtor
        job->setV(v);
        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++) {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
            job->setVForMachine(j, this->speeds[random_num]);
        }

        if (f_id == this->F) {
            f_id = 0;
        }

        // Choose a random position to allocate the job
        int random_position = rand.next() % (sol->getFactory(f_id)->getJobs().size() + 1);
        sol->getFactory(f_id)->addJobAtPosition(job, random_position);

        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
        f_id++;
    }

    // Add solution to population
    this->population.push_back(sol);
}

void Instance::printPopulation() {
    cout << "\nPOPULATION " << "(with " << this->population.size() << " solutions)" << endl;

    for (size_t i = 0; i < this->population.size(); i++) {
        cout << "\nSolution " << i << endl;
        this->population[i]->printSolution();
    }
}

string Instance::generatePopulationCSVString() {
    string str = "id, TFT, TEC\n";
    Solution *solution;

    for (size_t i = 0; i < this->population.size(); i++) {
        solution = this->population[i];
        str += to_string(i) + "," + to_string(solution->getTFT()) + "," + to_string(solution->getTEC())+ ","+ to_string(solution->getDominationRank()) + "\n";
    }
    return str;
}



vector<vector<Solution*>> Instance::fastNonDominatedSort() {
    //todo: não está funcionando, investigar. Parece gerar apenas primeira fronteira


    vector<vector<Solution*>> fronts(1);
    vector<vector<Solution*>> dominatedBy(population.size());

    for(int i=0; i< population.size(); i++){
        population[i]->setDominationCounter(0);

        for(int j=0; j< population.size(); j++){
            if(population[i]->dominates(population[j])){
                dominatedBy[i].push_back(population[j]);
            }else if(population[j]->dominates(population[i])){
                population[i]->setDominationCounter(population[i]->getDominationCounter() + 1);
            }
        }

        if(population[i]->getDominationCounter()==0){
            population[i]->setDominationRank(1);
            fronts[0].push_back(population[i]);
        }

    }

    int i =0;
    while(!fronts[i].empty()){
        vector<Solution*> nextFront;
        for(int j=0; j<fronts[i].size(); j++){
            for(int k=0; k<dominatedBy[j].size(); k++){
                dominatedBy[j][k]->setDominationCounter(dominatedBy[j][k]->getDominationCounter() - 1);
                if(dominatedBy[j][k]->getDominationCounter()==0){
                    dominatedBy[j][k]->setDominationRank(i+2);
                    nextFront.push_back(dominatedBy[j][k]);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    this->dominationFronts = fronts;
    return fronts;
}

void Instance::assignCrowdingDistance() {
    int numObjectives = 2;

    for(int i=0; i< population.size(); i++){
        population[i]->setCrowdingDistance(0);
    }

    for (int i = 0; i < numObjectives; i++) {

        if (i == 0) {
            sort(population.begin(), population.end(), compareSolutionsByTFT);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTFT = population[population.size() - 1]->getTFT();
            float minTFT = population[0]->getTFT();
            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance = (population[j + 1]->getTFT() - population[j - 1]->getTFT())/(maxTFT-minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }

        } else {
            sort(population.begin(), population.end(), compareSolutionsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance = (population[j + 1]->getTEC() - population[j - 1]->getTEC())/(maxTEC-minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }

        }

    }
}