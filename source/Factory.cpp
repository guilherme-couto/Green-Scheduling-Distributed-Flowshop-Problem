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

Factory::~Factory()
{
    /*for(int i=0; i< this->jobs.size(); i++){
        delete jobs[i];
    }*/
}

void Factory::setSpeeds(vector<float> s)
{
    Factory::speeds = s;
}

vector<Job *> Factory::getJobs()
{
    return this->jobs;
}

int Factory::getTotalJobs()
{
    return this->total_jobs;
}

float Factory::getTEC()
{
    float tec = 0.0;

    // Get the energy consumption of each job when processing in each machine
    for (int i = 0; i < this->jobs.size(); i++)
    {
        for (int j = 0; j < this->m; j++)
        {
            tec += 4 * this->jobs[i]->getV(j) * this->jobs[i]->getV(j); // 4 * v^2
        }
    }

    // Get the energy consumption of the standby times of each machine
    for (int i = 1; i < this->jobs.size(); i++)
    {
        float job_time = 0.0;
        float prev_job_time = 0.0;

        // Get the processing time on the first machine
        for (int j = 0; j < i; j++)
        {
            prev_job_time += this->jobs[j]->getP(0);
        }
        job_time = prev_job_time + this->jobs[i]->getP(0);

        for (int j = 1; j < this->m; j++)
        {
            // Check if standby time between jobs is greater than 0
            prev_job_time += this->jobs[i - 1]->getP(j);
            if (prev_job_time < job_time)
            {
                // Get the standby time
                tec += job_time - prev_job_time;
                job_time += this->jobs[i]->getP(j);
            }
            else
            {
                job_time += prev_job_time + this->jobs[i]->getP(j);
            }
        }
    }
    return tec;
}

float Factory::getTFT()
{
    float tft = 0.0;
    vector<float> partialFTByJob(this->jobs.size(), 0.0);

    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->jobs.size(); j++)
        {
            if (j == 0 || partialFTByJob[j] > partialFTByJob[j - 1])
            {
                partialFTByJob[j] = partialFTByJob[j] + this->jobs[j]->getP(i);
            }
            else
            {
                partialFTByJob[j] = partialFTByJob[j - 1] + this->jobs[j]->getP(i);
                ;
            }
        }
    }

    for (int i = 0; i < partialFTByJob.size(); i++)
    {
        tft += partialFTByJob[i];
    }

    return tft;
}

float Factory::getTFTAfterStartTimesMatrix(){
    float resultTFT = 0.0;

    //for(int i=0; i< this->jobs.size(); i++){
    //    resultTFT += this->jobs[i]->startTimes[this->m-1]*this->jobs[i]->getP(this->m-1);
   // }

    return resultTFT;
}

float Factory::getTFTTest(vector<Job *> testJobsList)
{
    float tft = 0.0;
    vector<float> partialFTByJob(testJobsList.size(), 0.0);

    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < testJobsList.size(); j++)
        {
            if (j == 0 || partialFTByJob[j] > partialFTByJob[j - 1])
            {
                partialFTByJob[j] = partialFTByJob[j] + testJobsList[j]->getP(i);
            }
            else
            {
                partialFTByJob[j] = partialFTByJob[j - 1] + testJobsList[j]->getP(i);
                ;
            }
        }
    }

    for (int i = 0; i < partialFTByJob.size(); i++)
    {
        tft += partialFTByJob[i];
    }

    return tft;
}

Factory *Factory::minTFTAfterInsertion(Job *job)
{
    float tftBeforeInsertion = this->getTFT();
    float minIncreaseTFT = INFINITY;
    float tftVariation;
    vector<Job *> auxJobs = this->jobs;
    vector<Job *> testJobs = this->jobs;
    vector<Job *> minTFTJobs = this->jobs;
    Factory *testFactory = new Factory();
    *testFactory = *this;

    for (int i = 0; i <= this->jobs.size(); i++)
    {

        testJobs = this->jobs;
        testJobs.insert(testJobs.begin() + i, job);
        float testFactoryTFT = this->getTFTTest(testJobs);

        tftVariation = testFactoryTFT - tftBeforeInsertion;
        if (tftVariation < minIncreaseTFT)
        {
            minIncreaseTFT = tftVariation;
            minTFTJobs = testJobs;
        }
    }

    testFactory->jobs = minTFTJobs;

    return testFactory;
}

Factory *Factory::minTECAfterInsertion(Job *job)
{
    float tecBeforeInsertion = this->getTEC();
    float minIncreaseTEC = INFINITY;
    float tecVariation;

    vector<Job *> auxJobs = this->jobs;
    vector<Job *> testJobs = this->jobs;
    vector<Job *> minTECJobs = this->jobs;

    for (int i = 0; i <= this->jobs.size(); i++)
    {
        testJobs = this->jobs;
        testJobs.insert(testJobs.begin() + i, job);

        Factory *auxFactory = new Factory();
        auxFactory->setMachines(this->m);
        auxFactory->setJobs(testJobs);

        float testFactoryTEC = auxFactory->getTEC();

        tecVariation = testFactoryTEC - tecBeforeInsertion;
        if (tecVariation < minIncreaseTEC)
        {
            minIncreaseTEC = tecVariation;
            minTECJobs = testJobs;
        }
    }

    this->jobs = minTECJobs;

    return this;
}

void Factory::addJobAtLastPosition(Job *job)
{

    for (int i = 0; i < this->jobs.size(); i++)
    {
        // if job already exists in factory, do nothing
        if (this->jobs[i]->getId() == job->getId())
            return;
    }

    this->jobs.push_back(job);
    this->total_jobs++;
}

void Factory::addJobAtPosition(Job *job, int pos)
{
    for (int i = 0; i < this->jobs.size(); i++)
    {
        // if job already exists in factory, do nothing
        if (this->jobs[i]->getId() == job->getId())
            return;
    }
    cout << "total jobs: " << this->total_jobs << endl;
    this->jobs.insert(this->jobs.begin() + pos, job);
    this->total_jobs++;
}

int auxFindIndex(vector<float> v, float element){
    for(int i=0; i<v.size(); i++){
        if(v[i] == element){
            return i;
        }
    }
    return -1;
}

void Factory::speedDown()
{
    Job *job;
    float previousTFT = this->getTFT();
    float previousSpeed;
    float newSpeed;
    float newTFT;
    int indexPreviousSpeed;

    for (int i = 0; i < this->jobs.size() - 1; i++)
    {
        job = this->jobs[i];

        for (int j = 1; j < this->m; j++)
        {
            previousSpeed = job->getV(j);
            indexPreviousSpeed = auxFindIndex(Factory::speeds, previousSpeed);

            for (int v = indexPreviousSpeed/*this->speeds.size() - 1*/; v > 1; v--)
            { // todo: pegar vetor de velocidades possíveis, talvez usar um campo estático na Factory
                newSpeed = Factory::speeds[v - 1];
                job->setVForMachine(j, newSpeed); // diminui para a próxima velocidade
                newTFT = this->getTFT();

                if (newTFT > previousTFT)
                {
                    job->setVForMachine(j, previousSpeed); // retorna à velocidade anterior
                    break;
                }
                else
                {
                    previousSpeed = newSpeed;
                    previousTFT = newTFT;
                }
            }
        }
    }
}


//obs, não testada ainda
void Factory::randSpeedDown(int seed)
{
    Job *job;
    Xoshiro256plus rand(seed);

    for (int i = 0; i < this->jobs.size() - 1; i++)
    {
        job = this->jobs[i];

        for (int j = 1; j < this->m; j++)
        {
            float previousSpeed = job->getV(j);
            int indexPreviousSpeed = auxFindIndex(Factory::speeds, previousSpeed);
            int indexNewSpeed = (int)rand.next()%indexPreviousSpeed;
            job->setVForMachine(j, Factory::speeds[indexNewSpeed]);

        }
    }
}


void Factory::setJobs(vector<Job *> jobs)
{
    this->jobs = jobs;
}

void Factory::setMachines(int m)
{
    this->m = m;
}