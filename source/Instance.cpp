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
    Factory::setSpeeds(this->speeds);
}

Instance::~Instance()
{
    for (int i = 0; i < this->population.size(); i++)
    {
        delete this->population[i];
    }
}

int Instance::getF()
{
    return this->F;
}

int Instance::get_n()
{
    return this->n;
}

int Instance::get_m()
{
    return this->m;
}

int Instance::get_t(int machine_id, int job_id)
{
    return t[machine_id][job_id];
}

void Instance::construtivo()
{

    for (int i = 0; i < 4; i++)
    {
        this->balancedRandomSolutionGenerator(i);
    }
}

bool compareJobsByTotalProcessingTime(Job *a, Job *b)
{
    return a->getTotalP() < b->getTotalP();
}

bool compareSolutionsByTFT(Solution *a, Solution *b)
{
    return a->getTFT() < b->getTFT();
}

bool compareSolutionsByTEC(Solution *a, Solution *b)
{
    return a->getTEC() < b->getTEC();
}

Solution *Instance::maxSMinTFT()
{
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++)
    {
        jobs[i] = new Job(i, this->m);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++)
        {
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[highestSpeed];
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)

    for (int i = 0; i < this->getF(); i++)
    { // assign the first f jobs to each one of the factories
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
    for (int j = this->getF(); j < jobs.size(); j++)
    { // test job j at all the possible positions of PI_k (the factory)
        float minIncreaseTFT = INFINITY;
        float minTFTPos = 0;

        for (int f = 0; f < this->getF(); f++)
        {
            testFactory = solution->getFactory(f)->minTFTAfterInsertion(jobs[j]);
            testFactoryTFT = testFactory->getTFT();
            previousTFT = solution->getFactory(f)->getTFT();
            tftVariation = testFactoryTFT - previousTFT;

            if (tftVariation < minIncreaseTFT)
            {
                minIncreaseTFT = tftVariation;
                minTFTFactory = testFactory;
                minTFTPos = f;
            }
            else
            {
                delete testFactory;
            }
        }

        // factories[minTFTPos] = minTFTFactory;
        solution->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->getF(); i++)
    {
        solution->getFactory(i)->speedDown();
    }
    this->population.push_back(solution);

    // Initialize the of each factory
    for (int f = 0; f < this->getF(); f++)
    {
        solution->getFactory(f)->initializeJobsStartTimes();
    }

    return solution;
}

Solution *Instance::randSMinTFT(int seed)
{
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());
    Xoshiro256plus rand(/*time(NULL) +*/ seed);
    int highestSpeed = this->speeds.size() - 1;

    for (int i = 0; i < this->get_n(); i++)
    {

        jobs[i] = new Job(i, this->m);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++)
        {
            int randomIndex = rand.next() % this->speeds.size();
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[randomIndex];
        }
        jobs[i]->setT(jobTimeByMachine);
        jobs[i]->setV(jobSpeedByMachine);
    }

    sort(jobs.begin(), jobs.end(),
         compareJobsByTotalProcessingTime); // generate job permutation lambda according to non-descending order of Ti (total job i processing time)

    for (int i = 0; i < this->getF(); i++)
    { // assign the first f jobs to each one of the factories
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
    for (int j = this->getF(); j < jobs.size(); j++)
    { // test job j at all the possible positions of PI_k (the factory)
        float minIncreaseTFT = INFINITY;
        float minTFTPos = 0;

        for (int f = 0; f < this->getF(); f++)
        {
            testFactory = solution->getFactory(f)->minTFTAfterInsertion(jobs[j]);
            testFactoryTFT = testFactory->getTFT();
            previousTFT = solution->getFactory(f)->getTFT();
            tftVariation = testFactoryTFT - previousTFT;

            if (tftVariation < minIncreaseTFT)
            {
                minIncreaseTFT = tftVariation;
                minTFTFactory = testFactory;
                minTFTPos = f;
            }
            else
            {
                delete testFactory;
            }
        }

        // factories[minTFTPos] = minTFTFactory;
        solution->replaceFactory(minTFTPos, minTFTFactory); // replaces old factory and deletes it
    }

    for (int i = 0; i < this->getF(); i++)
    {
        solution->getFactory(i)->speedDown();
    }
    this->population.push_back(solution);

    // Initialize the of each factory
    for (int f = 0; f < this->getF(); f++)
    {
        solution->getFactory(f)->initializeJobsStartTimes();
    }

    return solution;
}

Solution *Instance::minSMinTEC()
{
    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->get_n(); i++)
    {
        jobs[i] = new Job(i, this->m);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++)
        {
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[0]; // minimum speed
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
    Factory *testFactory;
    float testFactoryTEC;
    float tecVariation;
    float previousTEC;
    Factory *minTECFactory;

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

    // Initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->getF(); f++)
    {
        solution->getFactory(f)->initializeJobsStartTimes();
        solution->getFactory(f)->rightShift();
    }

    this->population.push_back(solution);
    return solution;
}

Solution *Instance::randSMinTEC(int seed)
{
    Xoshiro256plus rand(/*time(NULL) +*/ seed);

    Solution *solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job *> jobs(this->get_n());

    // First, all the speeds set to the minimum value
    for (int i = 0; i < this->get_n(); i++)
    {
        jobs[i] = new Job(i, this->m);
        vector<int> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for (int j = 0; j < this->get_m(); j++)
        {
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
    for (int k = 0; k < this->getF(); k++)
    {
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

    // Initialize the start_times of each factory and then right shift
    for (int f = 0; f < this->getF(); f++)
    {
        solution->getFactory(f)->initializeJobsStartTimes();
        solution->getFactory(f)->rightShift();
    }

    this->population.push_back(solution);
    return solution;
}

void Instance::balancedRandomSolutionGenerator(int s)
{
    Xoshiro256plus rand(/*time(NULL) +*/ s);

    Solution *sol = new Solution(this->n, this->m, this->F);

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
        Job *job = new Job(jobs_to_allocate[random_num], this->m);
        job->setT(this->t[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++)
        {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
            job->setVForMachine(j, this->speeds[random_num]);
        }

        // Select the factory
        if (f_id == this->F)
        {
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
    for (int f = 0; f < this->getF(); f++)
    {
        sol->getFactory(f)->initializeJobsStartTimes();
    }
}

void Instance::totalRandomSolutionGenerator(int s)
{
    Xoshiro256plus rand(/*time(NULL) +*/ s);

    Solution *sol = new Solution(this->n, this->m, this->F);

    // Vector that indicates if a job (id = index) has already been allocated
    vector<bool> job_allocated(this->n, false);

    vector<int> jobs_to_allocate;
    for (size_t i = 0; i < this->n; i++)
    {
        jobs_to_allocate.push_back(i);
    }

    // Allocate one job in each factory
    for (int i = 0; i < this->F; i++)
    {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num], this->m);
        job->setT(this->t[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++)
        {
            int random_num = rand.next() % this->speeds.size();
            sol->setV(job->getId(), j, this->speeds[random_num]);
            job->setVForMachine(j, this->speeds[random_num]);
        }
        sol->getFactory(i)->addJobAtLastPosition(job);
        jobs_to_allocate.erase(jobs_to_allocate.begin() + random_num);
    }

    // Allocate the remaining jobs randomly
    while (jobs_to_allocate.size())
    {
        int random_num = rand.next() % jobs_to_allocate.size();
        Job *job = new Job(jobs_to_allocate[random_num], this->m);
        job->setT(this->t[jobs_to_allocate[random_num]]);

        // Set a random speed for each machine
        for (int j = 0; j < this->m; j++)
        {
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
    for (int f = 0; f < this->getF(); f++)
    {
        sol->getFactory(f)->initializeJobsStartTimes();
    }
}

void Instance::printPopulation()
{
    cout << "\nPOPULATION "
         << "(with " << this->population.size() << " solutions)" << endl;

    for (size_t i = 0; i < this->population.size(); i++)
    {
        cout << "\nSolution " << i << endl;
        this->population[i]->printSolution();
    }
}

string Instance::generatePopulationCSVString()
{
    string str = "id, TFT, TEC, level, cd\n";
    Solution *solution;

    for (size_t i = 0; i < this->population.size(); i++)
    {
        solution = this->population[i];
        str += to_string(i) + ","
                + to_string(solution->getTFT()) + ","
                + to_string(solution->getTEC()) + ","
                + to_string(solution->getDominationRank()) +","
                + to_string(solution->getCrowdingDistance()) + "\n";
    }
    return str;
}

vector<vector<Solution*>> Instance::fastNonDominatedSort(vector<Solution*> population) {

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());


    for(int i=0; i< population.size(); i++){
        population[i]->setDominationCounter(0);
        //population[i]->setDominationRank(-1);

        for(int j=0; j< population.size(); j++){
            if(population[i]->dominates(population[j])){
                dominatedBy[i].push_back(j);
            }else if(population[j]->dominates(population[i])){
                population[i]->incrementDominationCounter(1);
            }
        }

        if(population[i]->getDominationCounter()==0){
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }

    }

    int i =0;
    while(!fronts[i].empty()){
        vector<int> nextFront;
        for(int j=0; j<fronts[i].size(); j++){
            int frontSolId =  fronts[i][j]; //id (indices) de cada solução na fronteira atual

            for(int k=0; k<dominatedBy[frontSolId].size(); k++){ //itera por cada solução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada solução dominada por frontSolId

                Solution *s = population[dominatedSolIndex]; // cada solução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if(s->getDominationCounter()==0){
                    s->setDominationRank(i+2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Solution*>> solutionFronts(fronts.size());
    for(int i=0; i< fronts.size(); i++){
        vector<Solution*> front(fronts[i].size());
        for(int j=0; j < fronts[i].size(); j++){
            front[j] = population[fronts[i][j]];
        }
        solutionFronts[i] = front;
    }

    solutionFronts.pop_back();
    //this->dominationFronts = solutionFronts;
    return solutionFronts;
}

vector<vector<Solution*>> Instance::fastNonDominatedSort() {

    vector<vector<int>> fronts(1);
    vector<vector<int>> dominatedBy(population.size());


    for(int i=0; i< population.size(); i++){
        population[i]->setDominationCounter(0);
        //population[i]->setDominationRank(-1);

        for(int j=0; j< population.size(); j++){
            if(population[i]->dominates(population[j])){
                dominatedBy[i].push_back(j);
            }else if(population[j]->dominates(population[i])){
                population[i]->incrementDominationCounter(1);
            }
        }

        if(population[i]->getDominationCounter()==0){
            population[i]->setDominationRank(1);
            fronts[0].push_back(i);
        }

    }

    int i =0;
    while(!fronts[i].empty()){
        vector<int> nextFront;
        for(int j=0; j<fronts[i].size(); j++){
            int frontSolId =  fronts[i][j]; //id (indices) de cada solução na fronteira atual

            for(int k=0; k<dominatedBy[frontSolId].size(); k++){ //itera por cada solução dominada pela de indice frontSolId
                int dominatedSolIndex = dominatedBy[frontSolId][k]; // id de cada solução dominada por frontSolId

                Solution *s = population[dominatedSolIndex]; // cada solução dominada por frontSolId

                s->incrementDominationCounter(-1);

                if(s->getDominationCounter()==0){
                    s->setDominationRank(i+2);
                    nextFront.push_back(dominatedSolIndex);
                }
            }
        }
        i++;
        fronts.push_back(nextFront);
    }

    vector<vector<Solution*>> solutionFronts(fronts.size());
    for(int i=0; i< fronts.size(); i++){
        vector<Solution*> front(fronts[i].size());
        for(int j=0; j < fronts[i].size(); j++){
            front[j] = population[fronts[i][j]];
        }
        solutionFronts[i] = front;
    }

    solutionFronts.pop_back();
    this->dominationFronts = solutionFronts;
    return solutionFronts;
}

void Instance::assignCrowdingDistance()
{
    int numObjectives = 2;

    for (int i = 0; i < population.size(); i++)
    {
        population[i]->setCrowdingDistance(0);
    }

    for (int i = 0; i < numObjectives; i++)
    {

        if (i == 0)
        {
            sort(population.begin(), population.end(), compareSolutionsByTFT);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTFT = population[population.size() - 1]->getTFT();
            float minTFT = population[0]->getTFT();
            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance = (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
        else
        {
            sort(population.begin(), population.end(), compareSolutionsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance = (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

void assignCrowdingDistance(vector<Solution*> population)
{
    int numObjectives = 2;
    int infinity = std::numeric_limits<int>::max();

    for (int i = 0; i < population.size(); i++)
    {
        population[i]->setCrowdingDistance(0);
    }

    for (int i = 0; i < numObjectives; i++)
    {

        if (i == 0)
        {
            sort(population.begin(), population.end(), compareSolutionsByTFT);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTFT = population[population.size() - 1]->getTFT();
            float minTFT = population[0]->getTFT();
            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance = (population[j + 1]->getTFT() - population[j - 1]->getTFT()) / (maxTFT - minTFT);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
        else
        {
            sort(population.begin(), population.end(), compareSolutionsByTEC);
            population[0]->setCrowdingDistance(INFINITY);
            population[population.size() - 1]->setCrowdingDistance(INFINITY);
            float maxTEC = population[population.size() - 1]->getTEC();
            float minTEC = population[0]->getTEC();

            for (int j = 1; j < population.size() - 1; j++)
            {
                float normalizedDistance = (population[j + 1]->getTEC() - population[j - 1]->getTEC()) / (maxTEC - minTEC);
                population[j]->incrementCrowdingDistance(normalizedDistance);
            }
        }
    }
}

bool crowdedCompare(Solution *s1, Solution* s2)
{

    if (s1->getDominationRank() < s2->getDominationRank())
    {
        return true;
    }
    else if (s1->getDominationRank() == s2->getDominationRank())
    {
        if (s1->getCrowdingDistance() > s2->getCrowdingDistance())
        {
            return true;
        }
        return false;
    }

    return false;
}

void Instance::NSGA3NextGen(){
    vector<Solution*> parents = this->population;
    vector<Solution*> nextGen;

    //todo: vector of reference points

    //todo: Recombine and mutate parents into this vector
    vector<Solution*> children;

    //todo: join parents and children into this vector
    vector<Solution*> all;
    this->population = all;

    vector<vector<Solution*>> fronts= this->fastNonDominatedSort();

    int inserted = 0;
    int n = 0;

    //insere enquanto o numero de elementos inseridos for menor q n
    for(int i=0; inserted < n && i < fronts.size()-1; i++){
        nextGen.reserve(nextGen.size() + fronts[i].size());
        for(int j=0; j < fronts[i].size() ; j++){
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }

    if(nextGen.size() + fronts.back().size() == n){
        nextGen.reserve(fronts.back().size());
        nextGen.insert(nextGen.end(), fronts.back().begin(), fronts.back().end());
    }else{
        // Points to be chosen from Fl: K = N − |Pt+1|
        // Normalize objectives and create reference set Zr
        // Associate each member s of St with a reference point
        // Compute niche count of reference point j ∈ Zr
        // Choose K members one at a time from Fl to construct Pt+1: Niching
    }
}




vector<Solution*> makeNewPop(vector<Solution*> parents, int seed, int n){
    vector<Solution*> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; //1 chance of swap 0 chance of insertion

    for(int i =0; i< parents.size(); i++){

        Solution* sol = new Solution(parents[i]);

        for(int j=0; j< n/4; j++){
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory* factory1 = sol->getFactory(factory1Id);
            Factory* factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            //choice = 1;
            if(choice == 1) {
                sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                //factory1->speedUp();
                //factory1->speedDown();
                //factory2->speedUp();
                //factory1->speedDown();
            }

            else
            {
                if (factory1->getNumJobs() - 1 > 0) {
                    sol->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                }
                else if (factory2->getNumJobs() - 1 > 0){
                    sol->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                }
                else
                    sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            //factory1->initializeJobsStartTimes();
            //factory2->initializeJobsStartTimes();
        }

        children.push_back(sol);

    }


    return children;
}

vector<Solution*> makeNewPopV2(vector<Solution*> parents, int seed, int n){
    vector<Solution*> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; //1 chance of swap 0 chance of insertion

    for(int i =0; i< parents.size(); i++){

        Solution* sol = new Solution(parents[i]);

        for(int j=0; j< n/4; j++){
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory* factory1 = sol->getFactory(factory1Id);
            Factory* factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            choice = 1;
            if(choice == 1) {
                sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
                factory1->speedUp();
                factory1->speedDown();
                factory2->speedUp();
                factory1->speedDown();
            }

            else
            {
                if (factory1->getNumJobs() - 1 > 0) {
                    sol->insert(factory1Id, factory2Id, factory1->getJob(job1), job2);
                }
                else if (factory2->getNumJobs() - 1 > 0){
                    sol->insert(factory2Id, factory1Id, factory2->getJob(job2), job1);
                }
                else
                    sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
            }
            //factory1->initializeJobsStartTimes();
            //factory2->initializeJobsStartTimes();
        }

        children.push_back(sol);

    }


    return children;
}

vector<Solution*> makeNewPopV3(vector<Solution*> parents, int seed, int n){
    vector<Solution*> children;
    Xoshiro256plus rand(seed);

    vector<int> prob{1, 1, 1, 1, 0, 0, 0, 0, 0, 0}; //1 chance of swap 0 chance of insertion

    for(int i =0; i< parents.size(); i++){

        Solution* sol = new Solution(parents[i]);

        for(int j=0; j< n/4; j++){
            int factory1Id = rand.next() % sol->getNumFactories();
            int factory2Id = rand.next() % sol->getNumFactories();
            Factory* factory1 = sol->getFactory(factory1Id);
            Factory* factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();

            int choice = rand.next() % prob.size();
            sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));

            if(choice == 1) {
                factory1->speedUp();
                factory2->speedUp();
            }
            else{
                factory1->speedDown();
                factory2->speedDown();
            }

        }

        children.push_back(sol);

    }


    return children;
}

void Instance::NSGA2NextGen(int seed){
    vector<Solution*> parents = this->population;
    vector<Solution*> nextGen;


    //todo: Recombine and mutate parents into this vector
    vector<Solution*> children = makeNewPopV3(parents, seed, parents.size());


    //todo: join parents and children into this vector
    vector<Solution*> all = parents;
    //this->population = all;
    all.reserve(this->population.size() + children.size());
    all.insert(all.end(), children.begin(), children.end());
    //std::remove(all.begin()+parents.size(), all.end(), )
    this->population = all;


    vector<vector<Solution*>> fronts= this->fastNonDominatedSort();


    int inserted = 0;
    int n = parents.size();
    nextGen.reserve(n);

    //insere enquanto o numero de elementos inseridos for menor q n
    int l = 0;
    for(int i=0; inserted < n && i < fronts.size()-1; i++){
        //nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        if(inserted+fronts[i].size() > n){
            l = i;
            break;
        }

        for(int j=0; j < fronts[i].size() ; j++){
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }


    //::assignCrowdingDistance(fronts.back());
    /*if(nextGen.size() + fronts[l].size() == n){
        nextGen.reserve(fronts[l].size());
        nextGen.insert(nextGen.end(), fronts[l].begin(), fronts[l].end());
    }*/if(nextGen.size() < n){
        //nextGen.reserve(n);
        sort(fronts[l].begin(), fronts[l].end(), crowdedCompare);
        for(int i=0; nextGen.size()< n; i++){
            nextGen.push_back(fronts[l][i]);
        }
    }

    this->population=nextGen;
}

int Instance::nMetric() {
    return this->dominationFronts[0].size();
}

vector<Solution *> Instance::getParetoFront() {
    return this->dominationFronts[0];
}

void Instance::INGM(Solution *sol, int seed)
{
    Xoshiro256plus rand(seed);
    Solution* new_sol = new Solution(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->F; f++)
        {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest) {
                largest = f_tft;
                largest_index = f;
            }
        }
    }
    else    // Optimize TEC
    {
        for (int f = 0; f < this->F; f++)
        {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest) {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job*> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getTotalJobs();

    while(jobs_to_try.size() > largest_f_total_jobs / 2)
    {
        // Get a random job and extract from the factory
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        new_sol->getFactory(largest_index)->removeJob(random_job_index);
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Change the origin factory
        if (random_obj == 0) // Optimize TFT
        {
            new_sol->getFactory(largest_index)->randSpeedUp(seed);
            new_sol->getFactory(largest_index)->speedUp();
        }
        else    // Optimize TEC
        {
            new_sol->getFactory(largest_index)->randSpeedDown(seed);
            new_sol->getFactory(largest_index)->speedDown();
            new_sol->getFactory(largest_index)->rightShift();
        }

        // Try inserting the job to every position of every factory until the solution dominates the original one
        for (int f = 0; f < this->F; f++)
        {
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++)
            {
                // Insert the job to the factory
                new_sol->getFactory(f)->insertJobAtPos(job, pos);

                // Change the factory
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                }
                else    // Optimize TEC
                {
                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if(new_sol->getTFT() > sol->getTFT() && new_sol->getTEC() > sol->getTEC())    // If new_sol dominates sol
                    this->new_individuals.push_back(new_sol);
                new_sol->getFactory(f)->removeJob(job->getId());
            }
        }
    }
}

void Instance::SNGM(Solution *sol, int seed)
{
    Xoshiro256plus rand(seed);
    Solution* new_sol = new Solution(sol);

    // Randomly choose the objective for optimization
    int random_obj = rand.next() % 2; // 0 = TFT, 1 = TEC

    // Get the factory with the largest TFT or TEC
    float largest = 0;
    int largest_index = -1;

    if (random_obj == 0) // Optimize TFT
    {
        for (int f = 0; f < this->F; f++)
        {
            float f_tft = new_sol->getFactory(f)->getTFT();
            if (f_tft > largest) {
                largest = f_tft;
                largest_index = f;
            }
        }
    }
    else    // Optimize TEC
    {
        for (int f = 0; f < this->F; f++)
        {
            float f_tec = new_sol->getFactory(f)->getTEC();
            if (f_tec > largest) {
                largest = f_tec;
                largest_index = f;
            }
        }
    }

    vector<Job*> jobs_to_try = new_sol->getFactory(largest_index)->getJobs();
    int largest_f_total_jobs = new_sol->getFactory(largest_index)->getTotalJobs();
    while(jobs_to_try.size() > largest_f_total_jobs / 2)
    {
        // Get a random job
        int random_job_index = rand.next() % jobs_to_try.size();
        Job *job = jobs_to_try[random_job_index];
        jobs_to_try.erase(jobs_to_try.begin() + random_job_index);

        // Try inserting the job to every position of every factory until the solution dominates the original one
        for (int f = 0; f < this->F; f++)
        {
            int f_num_of_jobs = new_sol->getFactory(f)->getNumJobs();
            for (int pos = 0; pos < f_num_of_jobs; pos++)
            {
                // Swap the job to the factory
                Job *job2;
                do {
                    job2 = new_sol->getFactory(f)->getJobs().at(pos);
                } while (job2->getId() == job->getId());    // get a different job to swap

                new_sol->swap(largest_index, f, job, job2);

                // Change the factories
                if (random_obj == 0) // Optimize TFT
                {
                    new_sol->getFactory(largest_index)->randSpeedUp(seed);
                    new_sol->getFactory(largest_index)->speedUp();

                    new_sol->getFactory(f)->randSpeedUp(seed);
                    new_sol->getFactory(f)->speedUp();
                }
                else    // Optimize TEC
                {
                    new_sol->getFactory(largest_index)->randSpeedDown(seed);
                    new_sol->getFactory(largest_index)->speedDown();
                    new_sol->getFactory(largest_index)->rightShift();

                    new_sol->getFactory(f)->randSpeedDown(seed);
                    new_sol->getFactory(f)->speedDown();
                    new_sol->getFactory(f)->rightShift();
                }
                if(new_sol->getTFT() > sol->getTFT() && new_sol->getTEC() > sol->getTEC())    // If new_sol dominates sol
                    this->new_individuals.push_back(new_sol);

                new_sol->swap(f, largest_index, job, job2);
            }
        }
    }
}

void Instance::HNGM(Solution *sol, int seed) {
    Xoshiro256plus rand(seed);

    // Randomly choose INGM or SNGM
    int random_gen = rand.next() % 2; // 0 = INGM, 1 = SNGM

    if (random_gen == 0)
        this->INGM(sol, seed);
    else
        this->SNGM(sol, seed);
}

void Instance::makenewpop_operators(vector<Solution*> parents, int seed) {
    Xoshiro256plus rand(seed);

    // clear the new_individuals vector
    this->new_individuals.clear();

    // Generate the same number of new individuals as parents size
    // For each solution in parents, generate a neighbour
    int i = 0;
    while(this->new_individuals.size() < parents.size())
    {
        // Randomly choose which operator will be used to generate a neighbour
        int rand_op = rand.next() % 3;  // 0 = INGM, 1 = SNGM, 2 = HNGM

        if (rand_op == 0)
            this->INGM(parents[i], seed);
        else if (rand_op == 1)
            this->SNGM(parents[i], seed);
        else
            this->HNGM(parents[i], seed);

        if (i == parents.size())
            i = 0;
        else
            i++;
    }

    // Return a generation new_individuals = parents + new_individuals
    this->new_individuals.insert(this->new_individuals.end(), parents.begin(), parents.end());
}

