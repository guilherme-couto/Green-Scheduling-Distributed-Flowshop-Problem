#include "defines.hpp"

Problem::Problem(int F, int n, int m, vector<vector<int>> t) {
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

Problem::~Problem() {

    if(this->population.size() > 0) {
        for (int i = 0; i < this->population.size(); i++) {
            delete this->population[i];
        }
    }
}

int Problem::getF() {
    return this->F;
}

int Problem::get_n() {
    return this->n;
}

int Problem::get_m() {
    return this->m;
}

vector<Solution*> Problem::getParetoArchive(){
    return this->paretoArchive;
}

void Problem::updateArchive(Solution *sol) {
    for(int i=0; i<this->paretoArchive.size(); i++){
        if(paretoArchive[i]->dominates(sol)){
            return;
        }
    }

    for(int i=0; i<this->paretoArchive.size(); i++){
        if(sol->dominates(paretoArchive[i])){
            paretoArchive.erase(paretoArchive.begin()+i);
        }
    }

    paretoArchive.push_back(sol);
}

int Problem::get_t(int machine_id, int job_id) {
    return t[machine_id][job_id];
}

void Problem::construtivo() {

    for (int i = 0; i < 4; i++) {
        this->balancedRandomSolutionGenerator(i);
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

Solution *Problem::maxSMinTFT() {
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++) {
        jobs[i] = new Job(i, this->m);
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

    for (int i = 0; i < this->getF(); i++) { // assign the first f jobs to each one of the factories
        // factories[i] = new Factory(i, this->m);
        solution->getFactory(i)->addJobAtLastPosition(jobs[i]);
    }

    // remove the assigned jobs from lambda

    Factory *testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;
    Factory *minTFTFactory;

    // for each factory f
    for (int j = this->getF(); j < jobs.size(); j++) { // test job j at all the possible positions of PI_k (the factory)
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
                //testFactory->clearJobs();
                delete testFactory;
            }
        }

        // factories[minTFTPos] = minTFTFactory;
        solution->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->getF(); i++) {
        solution->getFactory(i)->speedDown();
    }
    this->population.push_back(solution);

    // Initialize the of each factory
    for (int f = 0; f < this->getF(); f++) {
        solution->getFactory(f)->initializeJobsStartTimes();
    }

    return solution;
}

Solution *Problem::randSMinTFT(int seed) {
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());
    Xoshiro256plus rand(/*time(NULL) +*/ seed);
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++) {

        jobs[i] = new Job(i, this->m);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++) {
            int randomIndex = rand.next() % this->speeds.size();
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[randomIndex];
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)

    for (int i = 0; i < this->getF(); i++) { // assign the first f jobs to each one of the factories
        // factories[i] = new Factory(i, this->m);
        solution->getFactory(i)->addJobAtLastPosition(jobs[i]);
    }

    // remove the assigned jobs from lambda

    Factory *testFactory;
    float testFactoryTFT;
    float tftVariation;
    float previousTFT;
    Factory *minTFTFactory;

    // for each factory f
    for (int j = this->getF(); j < jobs.size(); j++) { // test job j at all the possible positions of PI_k (the factory)
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
                //testFactory->clearJobs();
                delete testFactory;
            }
        }

        // factories[minTFTPos] = minTFTFactory;
        solution->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->getF(); i++) {
        solution->getFactory(i)->speedDown();
    }
    this->population.push_back(solution);

    // Initialize the of each factory
    for (int f = 0; f < this->getF(); f++) {
        solution->getFactory(f)->initializeJobsStartTimes();
    }

    return solution;
}

Solution *Problem::minSMinTEC() {
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->get_n(); i++) {
        jobs[i] = new Job(i, this->m);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++) {
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[0]; // minimum speed
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    // Sort the jobs according to non-descending order of total processing time
    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // Assign the first f jobs to each one of the factories
    for (int k = 0; k < this->getF(); k++) {
        solution->getFactory(k)->addJobAtLastPosition(jobs[k]);
    }

    // Remove the assigned jobs from lambda = access the jobs vector from the f position to the end

    // Insert remaining jobs in the position of the factory that can lead to the minimum increse of total energy consumption
    Factory *testFactory;
    float testFactoryTEC;
    float tecVariation;
    float previousTEC;
    Factory *minTECFactory;

    // For each factory f
    for (int i = this->getF(); i < jobs.size(); i++) {
        float minIncreaseTEC = INFINITY;
        float minTECPos = 0;

        // Test job j at all the possible positions of PI_k (the factory)
        for (int k = 0; k < this->getF(); k++) {
            testFactory = solution->getFactory(k)->minTECAfterInsertion(jobs[i]);
            testFactoryTEC = testFactory->getTEC();
            previousTEC = solution->getFactory(k)->getTEC();
            tecVariation = testFactoryTEC - previousTEC;

            if (tecVariation < minIncreaseTEC) {
                minIncreaseTEC = tecVariation;
                minTECFactory = testFactory;
                minTECPos = k;
            } else {
                //testFactory->clearJobs();
                delete testFactory;
            }
        }

        // Add the job to the factory that leads to the minimum increase of total energy consumption
        solution->replaceFactory(minTECPos, minTECFactory); // replaces old factory and deletes it
    }

    // Initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->getF(); f++) {
        solution->getFactory(f)->initializeJobsStartTimes();
        solution->getFactory(f)->rightShift();
    }

    this->population.push_back(solution);
    return solution;
}

Solution *Problem::randSMinTEC(int seed) {
    Xoshiro256plus rand(/*time(NULL) +*/ seed);

    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->get_n(); i++) {
        jobs[i] = new Job(i, this->m);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++) {
            int randomSpeedIndex = rand.next() % this->speeds.size();
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[randomSpeedIndex]; // random speed
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    // Sort the jobs according to non-descending order of total processing time
    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);

    // Assign the first f jobs to each one of the factories
    for (int k = 0; k < this->getF(); k++) {
        solution->getFactory(k)->addJobAtLastPosition(jobs[k]);
    }

    // Remove the assigned jobs from lambda = access the jobs vector from the f position to the end

    // Insert remaining jobs in the position of the factory that can lead to the minimum increse of total energy consumption
    Factory *testFactory;
    float testFactoryTEC;
    float tecVariation;
    float previousTEC;
    Factory *minTECFactory;

    // For each factory f
    for (int i = this->getF(); i < jobs.size(); i++) {
        float minIncreaseTEC = INFINITY;
        float minTECPos = 0;

        // Test job j at all the possible positions of PI_k (the factory)
        for (int k = 0; k < this->getF(); k++) {
            testFactory = solution->getFactory(k)->minTECAfterInsertion(jobs[i]);
            testFactoryTEC = testFactory->getTEC();
            previousTEC = solution->getFactory(k)->getTEC();
            tecVariation = testFactoryTEC - previousTEC;

            if (tecVariation < minIncreaseTEC) {
                minIncreaseTEC = tecVariation;
                minTECFactory = testFactory;
                minTECPos = k;
            } else {
                //testFactory->clearJobs();
                delete testFactory;
            }
        }

        // Add the job to the factory that leads to the minimum increase of total energy consumption
        solution->replaceFactory(minTECPos, minTECFactory); // replaces old factory and deletes it
    }

    // Initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->getF(); f++) {
        solution->getFactory(f)->initializeJobsStartTimes();
        solution->getFactory(f)->rightShift();
    }

    this->population.push_back(solution);
    return solution;
}

void Problem::balancedRandomSolutionGenerator(int s) {
    Xoshiro256plus rand(/*time(NULL) +*/ s);

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
        Job *job = new Job(jobs_to_allocate[random_num], this->m);
        job->setT(this->t[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++) {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
            job->setVForMachine(j, this->speeds[random_num]);
        }

        // Select the factory
        if (f_id == this->F) {
            f_id = 0;
        }

        // Choose a random position to allocate the job
        int random_position = rand.next() % (sol->getFactory(f_id)->getJobs().size() + 1);

        // Allocate the job
        sol->getFactory(f_id)->addJobAtPosition(job, random_position);

        // Erase the allocated job from the list
        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
        f_id++;

        // Time control
        //gettimeofday(&this->end, 0);
        //long seconds = this->end.tv_sec - this->begin.tv_sec;
        //if (seconds > this->n/2)
        //    return;
    }

    // Add solution to population
    this->population.push_back(sol);

    // Initialize the of each factory
    for (int f = 0; f < this->getF(); f++) {
        sol->getFactory(f)->initializeJobsStartTimes();
    }
}

void Problem::totalRandomSolutionGenerator(int s) {
    Xoshiro256plus rand(/*time(NULL) +*/ s);

    Solution *sol = new Solution(this->n, this->m, this->F);

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> job_allocated(this->n, false);

    vector<int> jobs_to_allocate;
    for (size_t i = 0; i < this->n; i++) {
        jobs_to_allocate.push_back(i);
    }

    // Allocate one job in each factory
    for (int i = 0; i < this->F; i++) {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num], this->m);
        job->setT(this->t[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++) {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
            job->setVForMachine(j, this->speeds[random_num]);
        }
        sol->getFactory(i)->addJobAtLastPosition(job);
        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
    }

    // Allocate the remaining jobs randomly
    while (jobs_to_allocate.size()) {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num], this->m);
        job->setT(this->t[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++) {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
            job->setVForMachine(j, this->speeds[random_num]);
        }

        // Choose a random factory to allocate the job
        int f_id = rand.next() % this->F;

        // Choose a random position to allocate the job
        int random_position = rand.next() % (sol->getFactory(f_id)->getJobs().size() + 1);

        // Allocate the job
        sol->getFactory(f_id)->addJobAtPosition(job, random_position);

        // Erase the allocated job from the list
        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);

    }

    // Add solution to population
    this->population.push_back(sol);

    // Initialize the of each factory
    for (int f = 0; f < this->getF(); f++) {
        sol->getFactory(f)->initializeJobsStartTimes();
    }
}

void Problem::printPopulation() {
    cout << "\nPOPULATION "
         << "(with " << this->population.size() << " solutions)" << endl;

    for (size_t i = 0; i < this->population.size(); i++) {
        cout << "\nSolution " << i << endl;
        this->population[i]->printSolution();
    }
}

string Problem::generatePopulationCSVString() {
    string str = "id, TFT, TEC, level, cd\n";
    Solution *solution;

    for (size_t i = 0; i < this->population.size(); i++) {
        solution = this->population[i];
        str += to_string(i) + ","
               + to_string(solution->getTFT()) + ","
               + to_string(solution->getTEC()) + ","
               + to_string(solution->getDominationRank()) + ","
               + to_string(solution->getCrowdingDistance()) + "\n";
    }
    return str;
}

vector<vector<Solution *>> Problem::fastNonDominatedSort(vector<Solution *> population) {

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());


    for (int i = 0; i < population.size(); i++) {
        population[i]->setDominationCounter(0);
        //population[i]->setDominationRank(-1);

        for (int j = 0; j < population.size(); j++) {
            if (population[i]->dominates(population[j])) {
                dominatedBy[i].push_back(j);
            } else if (population[j]->dominates(population[i])) {
                population[i]->incrementDominationCounter(1);
            }
        }

        if (population[i]->getDominationCounter() == 0) {
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }

    }

    int i = 0;
    while (!fronts[i].empty()) {
        vector<int> nextFront;
        for (int j = 0; j < fronts[i].size(); j++) {
            int frontSolId = fronts[i][j]; //id (indices) de cada solução na fronteira atual

            for (int k = 0;
                 k < dominatedBy[frontSolId].size(); k++) { //itera por cada solução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada solução dominada por frontSolId

                Solution *s = population[dominatedSolIndex]; // cada solução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if (s->getDominationCounter() == 0) {
                    s->setDominationRank(i + 2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Solution *>> solutionFronts(fronts.size());
    for (int i = 0; i < fronts.size(); i++) {
        vector<Solution *> front(fronts[i].size());
        for (int j = 0; j < fronts[i].size(); j++) {
            front[j] = population[fronts[i][j]];
        }
        solutionFronts[i] = front;
    }

    solutionFronts.pop_back();
    //this->dominationFronts = solutionFronts;
    return solutionFronts;
}

vector<vector<Solution *>> Problem::fastNonDominatedSort() {

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());


    for (int i = 0; i < population.size(); i++) {
        population[i]->setDominationCounter(0);
        //population[i]->setDominationRank(-1);

        for (int j = 0; j < population.size(); j++) {
            if (population[i]->dominates(population[j])) {
                dominatedBy[i].push_back(j);
            } else if (population[j]->dominates(population[i])) {
                population[i]->incrementDominationCounter(1);
            }
        }

        if (population[i]->getDominationCounter() == 0) {
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }

    }

    int i = 0;
    while (!fronts[i].empty()) {
        vector<int> nextFront;
        for (int j = 0; j < fronts[i].size(); j++) {
            int frontSolId = fronts[i][j]; //id (indices) de cada solução na fronteira atual

            for (int k = 0;
                 k < dominatedBy[frontSolId].size(); k++) { //itera por cada solução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada solução dominada por frontSolId

                Solution *s = population[dominatedSolIndex]; // cada solução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if (s->getDominationCounter() == 0) {
                    s->setDominationRank(i + 2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Solution *>> solutionFronts(fronts.size());
    for (int i = 0; i < fronts.size(); i++) {
        vector<Solution *> front(fronts[i].size());
        for (int j = 0; j < fronts[i].size(); j++) {
            front[j] = population[fronts[i][j]];
        }
        solutionFronts[i] = front;
    }

    solutionFronts.pop_back();
    this->dominationFronts = solutionFronts;
    return solutionFronts;
}

void Problem::assignCrowdingDistance() {
    int numObjectives = 2;

    for (int i = 0; i < population.size(); i++) {
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
                float normalizedDistance =
                        (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        } else {
            sort(population.begin(), population.end(), compareSolutionsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance =
                        (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

void assignCrowdingDistance(vector<Solution *> population) {
    int numObjectives = 2;
    int infinity = std::numeric_limits<int>::max();

    for (int i = 0; i < population.size(); i++) {
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
                float normalizedDistance =
                        (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        } else {
            sort(population.begin(), population.end(), compareSolutionsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++) {
                float normalizedDistance =
                        (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

bool crowdedCompare(Solution *s1, Solution *s2) {

    if (s1->getDominationRank() < s2->getDominationRank()) {
        return true;
    } else if (s1->getDominationRank() == s2->getDominationRank()) {
        if (s1->getCrowdingDistance() > s2->getCrowdingDistance()) {
            return true;
        }
        return false;
    }

    return false;
}


vector<Solution *> makeNewPop(vector<Solution *> parents, int seed, int n) {
    vector<Solution *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; //1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++) {

        Solution *sol = new Solution(parents[i]);

        for (int j = 0; j < n / 4; j++) {
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory *factory1 = sol->getFactory(factory1Id);
            Factory *factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            //choice = 1;
            if (choice == 1) {
                sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                //factory1->speedUp();
                //factory1->speedDown();
                //factory2->speedUp();
                //factory1->speedDown();
            } else {
                if (factory1->getNumJobs() - 1 > 0) {
                    sol->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                } else if (factory2->getNumJobs() - 1 > 0) {
                    sol->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                } else
                    sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            //factory1->initializeJobsStartTimes();
            //factory2->initializeJobsStartTimes();
        }

        children.push_back(sol);

    }


    return children;
}

vector<Solution *> makeNewPopV2(vector<Solution *> parents, int seed, int n) {
    vector<Solution *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; //1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++) {

        Solution *sol = new Solution(parents[i]);

        for (int j = 0; j < n / 4; j++) {
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory *factory1 = sol->getFactory(factory1Id);
            Factory *factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            choice = 1;
            if (choice == 1) {
                sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                factory1->speedUp();
                factory1->speedDown();
                factory2->speedUp();
                factory1->speedDown();
            } else {
                if (factory1->getNumJobs() - 1 > 0) {
                    sol->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                } else if (factory2->getNumJobs() - 1 > 0) {
                    sol->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                } else
                    sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            //factory1->initializeJobsStartTimes();
            //factory2->initializeJobsStartTimes();
        }

        children.push_back(sol);

    }


    return children;
}

vector<Solution *> makeNewPopV3(vector<Solution *> parents, int seed, int n) {
    vector<Solution *> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; //1 chance of swap 0 chance of insertion

    for (int i = 0; i < parents.size(); i++) {

        Solution *sol = new Solution(parents[i]);

        for (int j = 0; j < n / 4; j++) {
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory *factory1 = sol->getFactory(factory1Id);
            Factory *factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));

            if (prob[choice] == 1) {
                factory1->speedUp();
                factory2->speedUp();
            } else {
                factory1->speedDown();
                factory2->speedDown();
            }

        }

        children.push_back(sol);

    }


    return children;
}

void Problem::NSGA2NextGen(int seed) {
    Xoshiro256plus rand(seed);
    vector<Solution *> parents = this->population;
    vector<Solution *> nextGen;

    //Recombine and mutate parents into this vector
    vector<Solution *> children = makeNewPopV3(parents, rand.next()%30000, parents.size());

    //join parents and children into this vector
    vector<Solution *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Solution *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    //insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        //nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }


    //::assignCrowdingDistance(fronts.back());
    /*if(nextGen.size() + fronts[l].size() == n){
        nextGen.reserve(fronts[l].size());
        nextGen.insert(nextGen.end(), fronts[l].begin(), fronts[l].end());
    }*/if (nextGen.size() < n) {
        //nextGen.reserve(n);
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(fronts[l][i]);
        }
    }

    this->population = nextGen;
}

void Problem::NSGA2NextGen_operators(int seed) {
    vector<Solution *> parents = this->population;
    vector<Solution *> nextGen;


    //Recombine and mutate parents into this vector
    vector<Solution *> children = makenewpop_operators(parents, seed);

    //join parents and children into this vector
    vector<Solution *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Solution *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    //insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        //nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }


    //::assignCrowdingDistance(fronts.back());
    /*if(nextGen.size() + fronts[l].size() == n){
        nextGen.reserve(fronts[l].size());
        nextGen.insert(nextGen.end(), fronts[l].begin(), fronts[l].end());
    }*/if (nextGen.size() < n) {
        //nextGen.reserve(n);
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(fronts[l][i]);
        }
    }

    this->population = nextGen;
}

void Problem::NSGA2NextGen_operators_ND(int seed) {
    vector<Solution *> parents = this->population;
    vector<Solution *> nextGen;


    //Recombine and mutate parents into this vector
    vector<Solution *> children = makenewpop_operators_ND(parents, seed);

    //join parents and children into this vector
    vector<Solution *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Solution *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    //insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        //nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }


    //::assignCrowdingDistance(fronts.back());
    /*if(nextGen.size() + fronts[l].size() == n){
        nextGen.reserve(fronts[l].size());
        nextGen.insert(nextGen.end(), fronts[l].begin(), fronts[l].end());
    }*/if (nextGen.size() < n) {
        //nextGen.reserve(n);
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(fronts[l][i]);
        }
    }

    this->population = nextGen;
}

void normalize(vector<Solution *> solutions,
               vector<tuple<float, float, int, int>> &refPoints,
               vector<tuple<float, float, Solution*>> &normalizedSolutions) {
    int numPoints = 10;
    refPoints.reserve(numPoints);
    //TFT, TEC, Niche Count, id
    //vector<tuple<float, float, int, int>> refPoints(numPoints);
    //TFT, TEC
    //vector<tuple<float, float, Solution*>> normalizedSolutions(numPoints);

    //vector<float> refPointTFTs(numPoints);
    //vector<float> refPointTECs(numPoints);
    //vector<float> TFTs(solutions.size(), 0.0);
    //vector<float> TECs(solutions.size(), 0.0);
    float minTFT = Util::minTFTSol(solutions)->getTFT();
    float minTEC = Util::minTECSol(solutions)->getTEC();
    float maxTFT = Util::maxTFTSol(solutions)->getTFT();
    float maxTEC = Util::maxTECSol(solutions)->getTEC();

    for (int i = 0; i < solutions.size(); i++) {
        float nTFT = (solutions[i]->getTFT() - minTFT) / (maxTFT - minTFT);
        float nTEC = (solutions[i]->getTEC() - minTEC) / (maxTEC - minTEC);
        tuple<float, float, Solution*> ns = make_tuple(nTFT, nTEC, solutions[i]);
        normalizedSolutions.push_back(ns);

        //TFTs[i] = (solutions[i]->getTFT() - minTFT) / maxTFT;
        //TECs[i] = (solutions[i]->getTEC() - minTEC) / maxTEC;
    }

    for (int i = 0; i < numPoints; i++) {
        float x = ((float)1 / (float)numPoints) * (float)i;
        float y = 1-((float)1 / (float)numPoints) * (float)i;
        tuple<float, float, int, int> ref = make_tuple(x, y, 0, 0);
        refPoints.push_back(ref);
        //refPointTECs.push_back((1 / numPoints) * i);
        //refPointTFTs.push_back((1 / numPoints) * i);
    }

}

void associate(vector<tuple<float, float, int, int>> &refPoints,
               vector<tuple<float, float, Solution*>> &normSol,
               vector<tuple<Solution *, float, int>> &assocVec) {

    int numSolutions = normSol.size();
    int numRefPoints = refPoints.size();

    vector<int> associationVector(numSolutions, 0);
    for (int i = 0; i < numSolutions; i++) {
        float minDistance = INFINITY;
        int refPointPos;

        for (int j = 0; j < numRefPoints; j++) {
            //compute distance of solution from each line
            float a = get<1>(refPoints[j]);
            float b = -get<0>(refPoints[j]);
            float x0 = get<0>(normSol[i]);
            float y0 = get<1>(normSol[i]);
            float ax0 = a * x0;
            float by0 =  b * y0;
            float distance = fabsf(ax0 + by0) / sqrtf(powf(a, 2) + powf(b, 2));

            if (distance < minDistance) {
                minDistance = distance;
                refPointPos = j;
            }
        }

        tuple<Solution *, float, int> assoc = make_tuple(get<2>(normSol[i]), minDistance, refPointPos);
        assocVec.push_back(assoc);
        //associationVector[i] = refPointPos;

        //assign PI(s) = line w closest from s
        //assign d(s) distance of sol. s to the closest line w

    }


}

bool nicheCompare(vector<tuple<Solution *, float>> &a, vector<tuple<Solution *, float>> &b) {
    return a.size() < b.size();
}

bool distanceCompare(tuple<Solution *, float> &a, tuple<Solution *, float> &b) {
    return get<0>(a) < get<0>(b);
}

vector<tuple<Solution *, float>> getIntersection(vector<Solution *> &solV, vector<tuple<Solution *, float>> &niche) {
    vector<tuple<Solution *, float>> intersection;

    for (Solution *s: solV) {
        for (tuple<Solution *, float> n: niche) {
            if (s == get<0>(n)) {
                intersection.push_back(n);
            }
        }
    }

    return intersection;
}

int minDistanceIndex(vector<tuple<Solution *, float>> v) {

    float minDistance = INFINITY;
    int minDistancePos = 0;
    for (tuple<Solution *, float> t: v) {
        if (get<1>(t) < minDistance) {
            minDistance = get<1>(t);
        }
    }

    return minDistancePos;
}

vector<int> getLeastNicheCountPoints(vector<tuple<float, float, int, int>> &refPoints) {
    vector<int> leastNicheCountPoints;
    int leastNicheCount = std::numeric_limits<int>::max();
    for (tuple<float, float, int, int> p: refPoints) {
        if (get<2>(p) < leastNicheCount && get<3>(p)!=-1) {
            leastNicheCount = get<2>(p);
        }
    }

    for (int i=0; i< refPoints.size(); i++) {
        if (get<2>(refPoints[i]) == leastNicheCount && get<3>(refPoints[i])!=-1) {
            leastNicheCountPoints.push_back(i);
        }
    }
    return leastNicheCountPoints;
}

int minDistancePointIndex(vector<tuple<Solution*, float, int>> &assoc){
    float minDistance = INFINITY;
    int minDistancePoint;

    for(int i=0; i<assoc.size(); i++){
        tuple<Solution*, float, int> p = assoc[i];
        if(get<1>(p) < minDistance){
            minDistance = get<1>(p);
            minDistancePoint = i;
        }
    }

    return minDistancePoint;
}

void niching(int K, int seed, vector<tuple<float, float, int, int>> &refPoints,
             vector<tuple<Solution *, float, int>> &assoc,
             vector<Solution *> &lastFront,
             vector<Solution *> &selected) {

    //vector<Solution *> selected(K);
    Xoshiro256plus rand(seed);



    int k=0;
    while(k<K) {
        //conjunto J = indices dos pontos com menor niche count
        vector<int> J = getLeastNicheCountPoints(refPoints);
        //escolhe elemento j aleatório em J
        int j = J[rand.next() % J.size()];

        //conjunto I = elementos de Fl que estão associados a j
        vector<tuple<Solution *, float, int>> I;
        for(tuple<Solution *, float, int> el:assoc){
            if (get<2>(el) == j) {
                for(Solution *s: lastFront) {
                    if (get<0>(el) == s){
                        I.push_back(el);
                    }
                }
            }
        }

        //se I não está vazio
        if(!I.empty()) {
            Solution * s;
            int pos;
            if(get<2>(refPoints[j])==0) {
                //seleciona elemento de I com o menor distância ao ponto de referência
                pos = minDistancePointIndex(I);
            }
            else{
                //seleciona elemento aleatório de I
                pos = rand.next() % I.size();
            }
            s = get<0>(I[pos]);
            selected.push_back(s);
            //incrementa niche count de j
            get<2>(refPoints[j])++;
            //remove elemento selecionado de Fl
            //std::remove(lastFront.begin(), lastFront.end(), s);
            lastFront.erase(std::remove(lastFront.begin(), lastFront.end(), s), lastFront.end());
            k++;
        }

        else{
            get<3>(refPoints[j])=-1;
            //refPoints.erase(refPoints.begin()+j);
        }
    }
}


void Problem::NSGA3NextGen(int seed) {
    Xoshiro256plus rand(seed);
    vector<Solution *> parents = this->population;
    vector<Solution *> nextGen;

    //Recombine and mutate parents into this vector
    vector<Solution *> children = makeNewPopV3(parents, rand.next()%30000, parents.size());

    //join parents and children into this vector
    vector<Solution *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Solution *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    //insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        //nextGen.reserve(nextGen.size() + fronts[i].size());
        //::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }

    if (nextGen.size() < n) {
        //sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);

        // Points to be chosen from Fl: K = N − |Pt+1|
        int K = n - nextGen.size();
        //TFT, TEC, niche count, *id
        vector<tuple<float, float, int, int>> refPoints;

        //normTFT, normTEC, Solution
        vector<tuple<float, float, Solution*>> normalizedSolutions;

        //Solution, distance to assoc. point, id of point in refPoints
        vector<tuple<Solution *, float, int>> associationVector;

        //Selected solutions
        vector<Solution*> selected;
        selected.reserve(K);

        // Normalize objectives and create reference set Zr
        normalize(this->population, refPoints, normalizedSolutions);

        // Associate each member s of St with a reference point
        associate(refPoints, normalizedSolutions, associationVector);

        // Compute niche count of reference point j ∈ Zr

        for(tuple<Solution *, float, int> a:associationVector){
            for(Solution *s: nextGen){
                if(s==get<0>(a)){
                    int assocRefIndex = get<2>(a);
                    get<2>(refPoints[assocRefIndex])++;
                }
            }
        }
        // Choose K members one at a time from Fl to construct Pt+1: Niching
        niching(K, seed, refPoints, associationVector, fronts[l], selected);

        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(selected[i]);
        }
    }

    this->population = nextGen;

}


void Problem::NSGA3NextGen_operators_ND(int seed) {
    Xoshiro256plus rand(seed);
    vector<Solution *> parents = this->population;
    vector<Solution *> nextGen;

    //Recombine and mutate parents into this vector
    vector<Solution *> children = makenewpop_operators_ND(parents, rand.next()%30000);

    //join parents and children into this vector
    vector<Solution *> all = parents;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    this->population = all;

    vector<vector<Solution *>> fronts = this->fastNonDominatedSort();

    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    //insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for (int i = 0; inserted < n && i < fronts.size() - 1; i++) {
        //nextGen.reserve(nextGen.size() + fronts[i].size());
        //::assignCrowdingDistance(fronts[i]);

        if (inserted + fronts[i].size() > n) {
            l = i;
            break;
        }

        for (int j = 0; j < fronts[i].size(); j++) {
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }

    if (nextGen.size() < n) {
        //sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);

        // Points to be chosen from Fl: K = N − |Pt+1|
        int K = n - nextGen.size();
        //TFT, TEC, niche count, *id
        vector<tuple<float, float, int, int>> refPoints;

        //normTFT, normTEC, Solution
        vector<tuple<float, float, Solution*>> normalizedSolutions;

        //Solution, distance to assoc. point, id of point in refPoints
        vector<tuple<Solution *, float, int>> associationVector;

        //Selected solutions
        vector<Solution*> selected;
        selected.reserve(K);

        // Normalize objectives and create reference set Zr
        normalize(this->population, refPoints, normalizedSolutions);

        // Associate each member s of St with a reference point
        associate(refPoints, normalizedSolutions, associationVector);

        // Compute niche count of reference point j ∈ Zr

        for(tuple<Solution *, float, int> a:associationVector){
            for(Solution *s: nextGen){
                if(s==get<0>(a)){
                    int assocRefIndex = get<2>(a);
                    get<2>(refPoints[assocRefIndex])++;
                }
            }
        }
        // Choose K members one at a time from Fl to construct Pt+1: Niching
        niching(K, seed, refPoints, associationVector, fronts[l], selected);

        for (int i = 0; nextGen.size() < n; i++) {
            nextGen.push_back(selected[i]);
        }
    }

    this->population = nextGen;

}

int Problem::nMetric() {
    return this->dominationFronts[0].size();
}

vector<Solution *> Problem::getParetoFront() {
    return this->dominationFronts[0];
}

Solution* Problem::INGM(Solution *sol, int seed) {
    Xoshiro256plus rand(seed);
    Solution *new_sol = new Solution(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->F; f++) {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest) {
                largest = f_tft;
                largest_index = f;
            }
        }
    } else    // Optimize TEC
    {
        for (int f = 0; f < this->F; f++) {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest) {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();

    while (jobs_to_try.size() > largest_f_total_jobs / 2) {
        // Get a random job and extract from the factory
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Change the origin factory
        if (random_obj == 0) // Optimize TFT
        {
            new_sol->getFactory(largest_index)->randSpeedUp(seed);
            new_sol->getFactory(largest_index)->speedUp();
        } else    // Optimize TEC
        {
            new_sol->getFactory(largest_index)->randSpeedDown(seed);
            new_sol->getFactory(largest_index)->speedDown();
            new_sol->getFactory(largest_index)->rightShift();
        }

        // Try inserting the job to every position of every factory until the solution dominates the original one
        for (int f = 0; f < this->F; f++) {
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++) {
                // Insert the job to the factory
                new_sol->insert(largest_index, f, job, pos);

                // Change the factory
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                } else    // Optimize TEC
                {
                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() &&
                    new_sol->getTEC() < sol->getTEC())   // If new_sol dominates sol
                {
                    return new_sol;
                }else if(new_sol->getTFT() < sol->getTFT() ||
                         new_sol->getTEC() < sol->getTEC()){

                    //this->updateArchive(new_sol);
                }
                new_sol->insert(f, largest_index, job, random_job_index);
            }
        }
    }
    return nullptr;
}

Solution* Problem::SNGM(Solution *sol, int seed) {
    Xoshiro256plus rand(seed);
    Solution *new_sol = new Solution(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->F; f++) {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest) {
                largest = f_tft;
                largest_index = f;
            }
        }
    } else    // Optimize TEC
    {
        for (int f = 0; f < this->F; f++) {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest) {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();
    while (jobs_to_try.size() > largest_f_total_jobs / 2) {
        // Get a random job
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Try inserting the job to every position of every factory until the solution dominates the original one
        for (int f = 0; f < this->F; f++) {
            bool tag = false;
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++) {
                // Swap the job to the factory
                Job *job2 = new_sol->getFactory(f)->getJobs().at(pos);
                while (job2->getId() == job->getId())    // get a different job to swap
                {
                    // if all possibilities of positions on f have been tried
                    if (pos + 1 == f_num_of_jobs)
                    {
                        tag = true;
                        break;
                    }
                    else {
                        pos ++;
                        job2 = new_sol->getFactory(f)->getJobs().at(pos);
                    }
                }
                // Needs to change factory
                if(tag)
                    break;

                new_sol->swap(largest_index, f, job, job2);

                // Change the factories
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(largest_index)->randSpeedUp(seed);
                    new_sol->getFactory(largest_index)->speedUp();

                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                } else    // Optimize TEC
                {
                    new_sol->getFactory(largest_index)->randSpeedDown(seed);
                    new_sol->getFactory(largest_index)->speedDown();
                    new_sol->getFactory(largest_index)->rightShift();

                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() &&
                    new_sol->getTEC() < sol->getTEC())    // If new_sol dominates sol
                {
                    return new_sol;
                }else if(new_sol->getTFT() < sol->getTFT() ||
                         new_sol->getTEC() < sol->getTEC()){

                   // this->updateArchive(new_sol);
                }
                new_sol->swap(f, largest_index, job, job2);
            }
        }
    }
    return nullptr;
}

Solution* Problem::HNGM(Solution *sol, int seed) {
    Xoshiro256plus rand(time(NULL));

    // Randomly choose INGM or SNGM
    int random_gen = rand.next() % 2; // 0 = INGM, 1 = SNGM

    if (random_gen == 0)
        return this->INGM(sol, seed);
    // else
    return this->SNGM(sol, seed);
}

vector<Solution*> Problem::makenewpop_operators(vector<Solution *> parents, int seed) {
    Xoshiro256plus rand(seed);

    // clear the new_individuals vector
    vector<Solution*> children;
    children.clear();
    Solution* sol_ptr = nullptr;

    // Generate the same number of new individuals as parents size
    // For each solution in parents, generate a neighbour
    int i = 0;
    int cont =0;
    while (i < parents.size()) {
        // Randomly choose which operator will be used to generate a neighbour
        int rand_op = rand.next() % 3;  // 0 = INGM, 1 = SNGM, 2 = HNGM

        if (rand_op == 0) {
            sol_ptr = this->INGM(parents[i], rand.next()%30000);
        }
        else if (rand_op == 1) {
            sol_ptr = this->SNGM(parents[i], rand.next()%30000);
        }
        else {
            sol_ptr = this->HNGM(parents[i], rand.next()%30000);
        }
        i++;
        if (sol_ptr != nullptr) {
            children.push_back(sol_ptr);
        }


       // if(cont == 3*parents.size()){
        //    break;
        //}

        cont ++;
    }

    // Return children
    return children;
}

Solution* Problem::INGM_ND(Solution *sol, int seed) {
    Xoshiro256plus rand(seed);
    Solution *new_sol = new Solution(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->F; f++) {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest) {
                largest = f_tft;
                largest_index = f;
            }
        }
    } else    // Optimize TEC
    {
        for (int f = 0; f < this->F; f++) {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest) {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();

    while (jobs_to_try.size() > largest_f_total_jobs / 2) {
        // Get a random job and extract from the factory
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Change the origin factory
        if (random_obj == 0) // Optimize TFT
        {
            new_sol->getFactory(largest_index)->randSpeedUp(seed);
            new_sol->getFactory(largest_index)->speedUp();
        } else    // Optimize TEC
        {
            new_sol->getFactory(largest_index)->randSpeedDown(seed);
            new_sol->getFactory(largest_index)->speedDown();
            new_sol->getFactory(largest_index)->rightShift();
        }

        // Try inserting the job to every position of every factory until the solution dominates the original one
        for (int f = 0; f < this->F; f++) {
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++) {
                // Insert the job to the factory
                new_sol->insert(largest_index, f, job, pos);

                // Change the factory
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                } else    // Optimize TEC
                {
                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() ||
                    new_sol->getTEC() < sol->getTEC())    // If new_sol is non dominated
                {
                    //this->updateArchive(new_sol);
                    return new_sol;
                }
                new_sol->insert(f, largest_index, job, random_job_index);
            }
        }
    }
    return nullptr;
}

Solution* Problem::SNGM_ND(Solution *sol, int seed) {
    Xoshiro256plus rand(seed);
    Solution *new_sol = new Solution(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->F; f++) {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest) {
                largest = f_tft;
                largest_index = f;
            }
        }
    } else    // Optimize TEC
    {
        for (int f = 0; f < this->F; f++) {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest) {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job *> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getNumJobs();
    while (jobs_to_try.size() > largest_f_total_jobs / 2) {
        // Get a random job
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Try inserting the job to every position of every factory until the solution dominates the original one
        for (int f = 0; f < this->F; f++) {
            bool tag = false;
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++) {
                // Swap the job to the factory
                Job *job2 = new_sol->getFactory(f)->getJobs().at(pos);
                while (job2->getId() == job->getId())    // get a different job to swap
                {
                    // if all possibilities of positions on f have been tried
                    if (pos + 1 == f_num_of_jobs)
                    {
                        tag = true;
                        break;
                    }
                    else
                        job2 = new_sol->getFactory(f)->getJobs().at(pos++);
                }
                // Needs to change factory
                if(tag)
                    break;

                new_sol->swap(largest_index, f, job, job2);

                // Change the factories
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(largest_index)->randSpeedUp(seed);
                    new_sol->getFactory(largest_index)->speedUp();

                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                } else    // Optimize TEC
                {
                    new_sol->getFactory(largest_index)->randSpeedDown(seed);
                    new_sol->getFactory(largest_index)->speedDown();
                    new_sol->getFactory(largest_index)->rightShift();

                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if (new_sol->getTFT() < sol->getTFT() ||
                    new_sol->getTEC() < sol->getTEC())    // If new_sol is non dominated by sol
                {
                    //this->updateArchive(new_sol);
                    return new_sol;
                }
                new_sol->swap(f, largest_index, job, job2);
            }
        }
    }
    return nullptr;
}

Solution* Problem::HNGM_ND(Solution *sol, int seed) {
    Xoshiro256plus rand(time(NULL));

    // Randomly choose INGM or SNGM
    int random_gen = rand.next() % 2; // 0 = INGM, 1 = SNGM

    if (random_gen == 0)
        return this->INGM_ND(sol, seed);
    // else
    return this->SNGM_ND(sol, seed);
}

vector<Solution*> Problem::makenewpop_operators_ND(vector<Solution *> parents, int seed) {
    Xoshiro256plus rand(seed);

    // clear the new_individuals vector
    vector<Solution*> children;
    children.clear();
    Solution* sol_ptr = nullptr;

    // Generate the same number of new individuals as parents size
    // For each solution in parents, generate a neighbour
    int i = 0;
    int cont=0;
    while (children.size() < parents.size()) {
        // Randomly choose which operator will be used to generate a neighbour
        int rand_op = rand.next() % 3;  // 0 = INGM, 1 = SNGM, 2 = HNGM

        if (rand_op == 0) {
            sol_ptr = this->INGM_ND(parents[i], rand.next()%30000);
        }
        else if (rand_op == 1) {
            sol_ptr = this->SNGM_ND(parents[i], rand.next()%30000);
        }
        else {
            sol_ptr = this->HNGM_ND(parents[i], rand.next()%30000);
        }
        i++;
        if (sol_ptr != nullptr) {
            children.push_back(sol_ptr);
            cont =0;
        }
        if (i == parents.size())
            i = 0;

         if(cont == 10*parents.size()){
            break;
        }
         cont++;
    }

    // Return children
    return children;
}