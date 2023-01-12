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
        gettimeofday(&this->end, 0);
        long seconds = this->end.tv_sec - this->begin.tv_sec;
        if (seconds > this->n/2)
            return;
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

        // Time control
        gettimeofday(&this->end, 0);
        long seconds = this->end.tv_sec - this->begin.tv_sec;
        if (seconds > this->n/2)
            return;
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
    string str = "id, TFT, TEC\n";
    Solution *solution;

    for (size_t i = 0; i < this->population.size(); i++)
    {
        solution = this->population[i];
        str += to_string(i) + "," + to_string(solution->getTFT()) + "," + to_string(solution->getTEC()) + "," + to_string(solution->getDominationRank()) + "\n";
    }
    return str;
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
    for(int i=0; inserted < n and i < fronts.size()-1; i++){
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




vector<Solution*> Instance::makeNewPop(vector<Solution*> parents, int seed){
    vector<Solution*> children;
    Xoshiro256plus rand(seed);

    for(int i =0; i< parents.size(); i++){

        Solution* sol = new Solution(parents[i]);

        for(int j=0; j< 30; j++){
            int factory1Id = rand.next() % this->getF();
            int factory2Id = rand.next() % this->getF();
            Factory* factory1 = sol->getFactory(factory1Id);
            Factory* factory2 = sol->getFactory(factory2Id);
            int job1 = rand.next() % factory1->getNumJobs();
            int job2 = rand.next() % factory2->getNumJobs();
            sol->swap(factory1Id, factory2Id, factory1->getJob(job1), factory2->getJob(job2));
        }

        children.push_back(sol);

    }


    return children;
}



void Instance::NSGA2NextGen(int seed){
    vector<Solution*> parents = this->population;
    vector<Solution*> nextGen;

    // Time control
    gettimeofday(&this->end, 0);
    long seconds = this->end.tv_sec - this->begin.tv_sec;
    if (seconds > this->n/2)
        return;

    //todo: Recombine and mutate parents into this vector
    vector<Solution*> children = this->makeNewPop(parents, seed);

    // Time control
    gettimeofday(&this->end, 0);
    seconds = this->end.tv_sec - this->begin.tv_sec;
    if (seconds > this->n/2)
        return;

    //todo: join parents and children into this vector
    vector<Solution*> all;
    this->population = all;

    vector<vector<Solution*>> fronts= this->fastNonDominatedSort();

    // Time control
    gettimeofday(&this->end, 0);
    seconds = this->end.tv_sec - this->begin.tv_sec;
    if (seconds > this->n/2)
        return;

    int inserted = 0;
    int n = parents.size();

    //insere enquanto o numero de elementos inseridos for menor q n
    for(int i=0; inserted < n and i < fronts.size()-1; i++){
        nextGen.reserve(nextGen.size() + fronts[i].size());
        ::assignCrowdingDistance(fronts[i]);

        for(int j=0; j < fronts[i].size() ; j++){
            nextGen.push_back(fronts[i][j]);
            inserted++;
        }
    }

    // Time control
    gettimeofday(&this->end, 0);
    seconds = this->end.tv_sec - this->begin.tv_sec;
    if (seconds > this->n/2)
        return;

    ::assignCrowdingDistance(fronts.back());
    if(nextGen.size() + fronts.back().size() == n){
        nextGen.reserve(fronts.back().size());
        nextGen.insert(nextGen.end(), fronts.back().begin(), fronts.back().end());
    }else{
        sort(fronts.back().begin(), fronts.back().end(), crowdedCompare);
        for(int i=0; nextGen.size() + i< n; i++){
            nextGen.push_back(fronts.back()[i]);
        }
    }
}

int Instance::nMetric() {
    return this->dominationFronts[0].size();
}