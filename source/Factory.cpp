#include "defines.hpp"

vector<float> Factory::speeds(1, 0.0);

Factory::Factory(Factory *f)
{
    this->id = f->id;
    // vector<Job *> jobs;
    this->total_jobs = f->total_jobs;
    this->TEC = f->TEC;
    this->TFT = f->TFT;
    this->m = f->m;
    // static vector<float> speeds;
    this->jobs_start_times_initialized = f->jobs_start_times_initialized;

    this->jobs.reserve(f->jobs.size());
    for (int i = 0; i < f->jobs.size(); i++)
    {
        this->jobs.push_back(new Job(f->jobs[i]));
    }

    this->TECChanged = f->TECChanged;
    this->TFTChanged = f->TFTChanged;
}

Factory::Factory(int id, int m)
{
    this->id = id;
    this->m = m;
    this->total_jobs = 0;
    this->TEC = 0.0;
    this->TFT = 0.0;
    this->jobs_start_times_initialized = false;
    this->TECChanged = true;
    this->TFTChanged = true;
}

Factory::~Factory()
{
   // for(int i=0; i< this->jobs.size(); i++){
   //     delete jobs[i];
   // }
}

void Factory::clearJobs(){
    this->jobs.clear();
}

void Factory::setSpeeds(vector<float> s)
{
    Factory::speeds = s;

}

vector<Job *> Factory::getJobs()
{
    return this->jobs;
}

int Factory::getJobPosAtSeq(int job_id)
{
    for (int i = 0; i < this->jobs.size(); i++)
    {
        if (this->jobs[i]->getId() == job_id)
        {
            return i;
        }
    }
}

int Factory::getTotalJobs()
{
    return this->total_jobs;
}

float Factory::getTEC()
{
    if(!this->TECChanged){
        return this->TEC;
    }

    if (!this->jobs_start_times_initialized)
    {
        float tec = 0.0;

        // Get the energy consumption of each job when processing in each machine
        for (int i = 0; i < this->jobs.size(); i++)
        {
            for (int j = 0; j < this->m; j++)
            {
                tec += 4 * this->jobs[i]->getV(j) * this->jobs[i]->getV(j) * this->jobs[i]->getP(j); // 4 * v^2 * p
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
        this->TEC = tec;
        //return tec;
    }
    else {
        this->TEC = this->getTECAfterStartTimesSet();
        //return this->getTECAfterStartTimesSet();
    }

    this->TECChanged = false;
    return this->TEC;

}

float Factory::getTFT()
{
    if(!this->TFTChanged){
        return this->TFT;
    }


    if (!this->jobs_start_times_initialized)
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
        this->TFT =tft;
        //return tft;
    }
    else
    {
        this->TFT =this->getTFTAfterStartTimesSet();

        //return this->getTFTAfterStartTimesSet();
    }
    this->TFTChanged = false;
    return this->TFT;
}

float Factory::getTFTAfterStartTimesSet()
{
    float resultTFT = 0.0;

    for (int i = 0; i < this->jobs.size(); i++)
    {
        resultTFT += this->jobs[i]->getStartTimes()[this->m - 1] + this->jobs[i]->getP(this->m - 1);
    }

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
    Factory *testFactory = new Factory(this->id, this->m);
    //*testFactory = *this;

    for (int i = 0; i <= this->jobs.size(); i++)
    {
        testJobs = this->jobs;
        testJobs.insert(testJobs.begin() + i, job);
        testFactory->setJobs(testJobs);
        //float testFactoryTFT = this->getTFTTest(testJobs);
        float testFactoryTFT = testFactory->getTFT();

        tftVariation = testFactoryTFT - tftBeforeInsertion;
        if (tftVariation < minIncreaseTFT)
        {
            minIncreaseTFT = tftVariation;
            minTFTJobs = testJobs;
        }
    }

    //testFactory->jobs = minTFTJobs;
    testFactory->setJobs(minTFTJobs);
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
        //auxFactory->clearJobs();
        delete auxFactory;
    }

    Factory *testFactory = new Factory();
    *testFactory = *this;
    //testFactory->jobs = minTECJobs;
    testFactory->setJobs(minTECJobs);

    return testFactory;
}

void Factory::addJobAtLastPosition(Job *job)
{
    this->TECChanged = true;
    this->TFTChanged = true;

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
    this->TECChanged = true;
    this->TFTChanged = true;

    for (int i = 0; i < this->jobs.size(); i++)
    {
        // if job already exists in factory, do nothing
        if (this->jobs[i]->getId() == job->getId())
            return;
    }
    this->jobs.insert(this->jobs.begin() + pos, job);
    this->total_jobs++;


}

int auxFindIndex(vector<float> v, float element)
{
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i] == element)
        {
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

        for (int j = 1; j < this->m - 1; j++)
        {
            previousSpeed = job->getV(j);
            indexPreviousSpeed = auxFindIndex(Factory::speeds, previousSpeed);

            for (int v = indexPreviousSpeed; v > 1; v--)
            {
                newSpeed = Factory::speeds[v - 1];
                //job->setVForMachine(j, newSpeed); // diminui para a próxima velocidade
                this->setVForJobMachine(i, j, newSpeed);
                newTFT = this->getTFT();

                if (newTFT > previousTFT)
                {
                    //job->setVForMachine(j, previousSpeed); // retorna à velocidade anterior
                    this->setVForJobMachine(i, j, previousSpeed);
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

    this->TECChanged = true;
    this->TFTChanged = true;
}

// obs, não testada ainda
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
            if(indexPreviousSpeed>0){
                int indexNewSpeed = rand.next() % indexPreviousSpeed;
               // job->setVForMachine(j, Factory::speeds[indexNewSpeed]);
                this->setVForJobMachine(i, j, Factory::speeds[indexNewSpeed]);
            }
        }
    }

    this->TECChanged = true;
    this->TFTChanged = true;
}

void Factory::setJobs(vector<Job *> jobs)
{
    this->jobs = jobs;
    this->TECChanged = true;
    this->TFTChanged = true;
}

void Factory::setMachines(int m)
{
    this->m = m;
}

void Factory::initializeJobsStartTimes()
{
    // The first job of the sequence starts at time 0 and it's executed with no standby time on the other machines
    float global_sum = 0.0;
    float individual_sum = 0.0;
    for (int i = 0; i < this->jobs.size(); i++)
    {
        for (int j = 0; j < this->m; j++)
        {
            if (i > 0)
                if (this->jobs[i - 1]->getStartTime(j) + this->jobs[i - 1]->getP(j) > individual_sum)
                    individual_sum = this->jobs[i - 1]->getStartTime(j) + this->jobs[i - 1]->getP(j);

            this->jobs[i]->setStartTime(j, individual_sum);
            individual_sum += this->jobs[i]->getP(j);
        }

        // Update variables
        global_sum += this->jobs[i]->getP(0);
        individual_sum = global_sum;
    }
    this->jobs_start_times_initialized = true;
}

void Factory::rightShift()
{
    // Begin at the last machine
    for (int j = this->m - 1; j > 0; j--)
    {
        // Begin with the last job and its predecessor
        for (int i = this->jobs.size() - 1; i > 0; i--)
        {
            // If there is standby time between the job and its predecessor, shift the predecessor to the right, postponing it and reducing the standby time
            if (this->jobs[i - 1]->getStartTime(j) + this->jobs[i - 1]->getP(j) < this->jobs[i]->getStartTime(j))
            {
                // Shift the job to the right the maximum amount possible without overlapping the next machine start time
                if (j != this->m - 1)
                {
                    if (this->jobs[i]->getStartTime(j) - this->jobs[i - 1]->getP(j) > this->jobs[i - 1]->getStartTime(j + 1))
                    {
                        float time = this->jobs[i - 1]->getStartTime(j + 1) - this->jobs[i - 1]->getP(j);
                        this->jobs[i - 1]->setStartTime(j, time);
                    }
                    else
                    {
                        float time = this->jobs[i]->getStartTime(j) - this->jobs[i - 1]->getP(j);
                        this->jobs[i - 1]->setStartTime(j, time);
                    }
                }
                else
                {
                    float time = this->jobs[i]->getStartTime(j) - this->jobs[i - 1]->getP(j);
                    this->jobs[i - 1]->setStartTime(j, time);
                }
            }
        }
    }

    this->TECChanged = true;
    this->TFTChanged = true;
}

float Factory::getTECAfterStartTimesSet()
{
    if (this->jobs_start_times_initialized)
    {
        float tec = 0.0;

        // First, calculate the processing consumption
        for (int j = 0; j < this->m; j++)
        {
            for (int i = 0; i < this->jobs.size(); i++)
            {
                tec += 4 * this->jobs[i]->getV(j) * this->jobs[i]->getV(j) * this->jobs[i]->getP(j); // 4 * v^2 * p
            }
        }

        // Then, calculate the standby consumption
        for (int j = 0; j < this->m; j++)
        {
            for (int i = 1; i < this->jobs.size(); i++)
            {
                if (this->jobs[i]->getStartTime(j) > this->jobs[i - 1]->getStartTime(j) + this->jobs[i - 1]->getP(j))
                    tec += this->jobs[i]->getStartTime(j) - this->jobs[i - 1]->getStartTime(j) + this->jobs[i - 1]->getP(j);
            }
        }
        return tec;
    }
    else
        return 0;
}

void Factory::speedUp()
{
    // The critical job is the last job of the sequence
    int critical_job_id = this->jobs.size() - 1;

    // if factory has more than one job
    if(this->getNumJobs() > 1) {
        float min_idle_time = INFINITY;
        int min_idle_time_machine = -1;

        // Find where is the smallest idle time between the critical job and its predecessor
        for (int j = 1; j < this->m; j++) {
            float idle_time = this->jobs[critical_job_id]->getStartTime(j) -
                              (this->jobs[critical_job_id - 1]->getStartTime(j) +
                               this->jobs[critical_job_id - 1]->getP(j));
            if (idle_time < min_idle_time && idle_time > 0) {
                min_idle_time = idle_time;
                min_idle_time_machine = j;
            }
        }

        // Speed up the critical job to the maximum possible speed without overlapping the next machine start time
        if (min_idle_time_machine != -1) {
            int j = 0;
            while (j < min_idle_time_machine) {
                float speed = this->jobs[critical_job_id]->getV(j);

                // If the job isnt at the maximum speed, speed it up and left shift the critical job on the following machines
                if (speed < this->speeds[this->speeds.size() - 1]) {
                    float new_speed = this->speeds[0];

                    // Select the new speed. Will be the closest higher speed to the current one
                    for (int s = 0; s < this->speeds.size(); s++) {
                        if (speed == this->speeds[s]) {
                            new_speed = this->speeds[s + 1];
                            break;
                        }
                    }

                    // Check if the new speed will overlap the available idle time
                    float actual_job_p = this->jobs[critical_job_id]->getP(j);
                    float job_t_j = this->jobs[critical_job_id]->getT_j(j);
                    float new_job_p = job_t_j / new_speed;

                    float reduction = actual_job_p - new_job_p;

                    if (reduction <= min_idle_time) {
                        // Speed up the job
                        this->jobs[critical_job_id]->setVForMachine(j, new_speed);

                        // Left shift the critical job on the following machines
                        for (int k = j + 1; k < this->m; k++) {
                            float new_start_time = this->jobs[critical_job_id]->getStartTime(k) - reduction;
                            this->jobs[critical_job_id]->setStartTime(k, new_start_time);
                        }

                        this->TECChanged = true;
                        this->TFTChanged = true;

                        return;
                    }
                }
                j++;
            }
        }
    }
    else if (this->getNumJobs() == 1)
    {
        // First try the first machine
        int j = 0;

        while(j < this->m) {

            float speed = this->jobs[critical_job_id]->getV(j);

            // If the job isnt at the maximum speed, speed it up and left shift the critical job on the following machines
            if (speed < this->speeds.back()) {

                float new_speed = this->speeds[0];

                // Select the new speed. Will be the closest higher speed to the current one
                for (int s = 0; s < this->speeds.size(); s++) {
                    if (speed == this->speeds[s]) {
                        new_speed = this->speeds[s + 1];
                        break;
                    }
                }

                float actual_job_p = this->jobs[critical_job_id]->getP(j);
                float job_t_j = this->jobs[critical_job_id]->getT_j(j);
                float new_job_p = job_t_j / new_speed;

                float reduction = actual_job_p - new_job_p;

                // Left shift the critical job on the following machines
                for (int k = j + 1; k < this->m; k++) {
                    float new_start_time = this->jobs[critical_job_id]->getStartTime(k) - reduction;
                    this->jobs[critical_job_id]->setStartTime(k, new_start_time);
                }
                this->TECChanged = true;
                this->TFTChanged = true;

                return;
            }
            j++;
        }
    }
    this->TECChanged = true;
    this->TFTChanged = true;

}

void Factory::randSpeedUp(int seed)
{
    // Choose a random job
    Xoshiro256plus rand(seed);

    int job_id;
    do
    {
        job_id = rand.next() % this->jobs.size();
    } while (job_id == 0);

    if(this->getNumJobs() > 1) {
        float min_idle_time = INFINITY;
        int min_idle_time_machine = -1;

        // Find where is the smallest idle time between the critical job and its predecessor
        for (int j = 1; j < this->m; j++) {
            float idle_time = this->jobs[job_id]->getStartTime(j) -
                              (this->jobs[job_id - 1]->getStartTime(j) + this->jobs[job_id - 1]->getP(j));

            if (idle_time < min_idle_time && idle_time > 0) {
                min_idle_time = idle_time;
                min_idle_time_machine = j;
            }
        }

        // Speed up the critical job to the maximum possible speed without overlapping the next machine start time
        if (min_idle_time_machine != -1) {
            int j = 0;
            while (j < min_idle_time_machine) {
                float speed = this->jobs[job_id]->getV(j);

                // If the job isnt at the maximum speed for the machine, speed it up and left shift the criticaç job on the following machines
                if (speed < this->speeds[this->speeds.size() - 1]) {
                    float new_speed;

                    // Select the new speed. Will be the closest higher speed to the current one
                    for (int s = 0; s < this->speeds.size(); s++) {
                        if (speed == this->speeds[s]) {
                            new_speed = this->speeds[s + 1];
                            break;
                        }
                    }

                    // Check if the new speed will overlap the available idle time
                    float actual_job_p = this->jobs[job_id]->getP(j);
                    float job_t_j = this->jobs[job_id]->getT_j(j);
                    float new_job_p = job_t_j / new_speed;

                    float reduction = actual_job_p - new_job_p;

                    if (reduction <= min_idle_time) {
                        // Speed up the job
                        this->jobs[job_id]->setVForMachine(j, new_speed);

                        // Left shift the critical job and its predecessors on the following machines
                        for (int k = j + 1; k < this->m; k++) {

                            float new_start_time = this->jobs[job_id]->getStartTime(k) - reduction;
                            this->jobs[job_id]->setStartTime(k, new_start_time);
                            if (job_id != this->jobs.back()->getId())
                            {
                                for (int i = job_id+1; i < this->jobs.size(); ++i) {
                                    new_start_time = this->jobs[i]->getStartTime(k) - reduction;
                                    this->jobs[i]->setStartTime(k, new_start_time);
                                }
                            }
                        }

                        this->TECChanged = true;
                        this->TFTChanged = true;
                        return;
                    }
                }
                j++;
            }
        }
    }
    else if (this->getNumJobs() == 1)
    {
        // First try the first machine
        int j = 0;

        while(j < this->m) {

            float speed = this->jobs[job_id]->getV(j);

            // If the job isnt at the maximum speed, speed it up and left shift the critical job on the following machines
            if (speed < this->speeds.back()) {

                float new_speed = this->speeds[0];

                // Select the new speed. Will be the closest higher speed to the current one
                for (int s = 0; s < this->speeds.size(); s++) {
                    if (speed == this->speeds[s]) {
                        new_speed = this->speeds[s + 1];
                        break;
                    }
                }

                float actual_job_p = this->jobs[job_id]->getP(j);
                float job_t_j = this->jobs[job_id]->getT_j(j);
                float new_job_p = job_t_j / new_speed;

                float reduction = actual_job_p - new_job_p;

                // Left shift the critical job on the following machines
                for (int k = j + 1; k < this->m; k++) {
                    float new_start_time = this->jobs[job_id]->getStartTime(k) - reduction;
                    this->jobs[job_id]->setStartTime(k, new_start_time);

                }
                this->TECChanged = true;
                this->TFTChanged = true;

                return;
            }
            j++;
        }
    }
}

void Factory::removeJob(int job_id)
{
    for (int i = 0; i < this->jobs.size(); i++)
    {
        if (this->jobs[i]->getId() == job_id)
        {
            // Update the jobs start times before removing the job
            for (int k = i + 1; k < this->jobs.size(); k++)
            {
                for (int j = 0; j < this->m; j++)
                {
                    this->jobs[k]->setStartTime(j, this->jobs[k]->getStartTime(j) - this->jobs[i]->getP(j));
                }
            }

            // Remove the job
            this->jobs.erase(this->jobs.begin() + i);
            break;
        }
    }

    this->TECChanged = true;
    this->TFTChanged = true;
}

void Factory::insertJobAtPos(Job *job, int pos)
{
    if (pos >= this->jobs.size())
    {
        // Insert the job at the last position
        this->jobs.push_back(job);

        // Update the start times of the job
        int index = this->jobs.size() - 1;
        for (int j = 0; j < this->m; j++)
        {
            if (j == 0)
                job->setStartTime(j, this->jobs[index]->getStartTime(j) + this->jobs[index]->getP(j));
            else
            {
                if (job->getStartTime(j - 1) + job->getP(j - 1) > this->jobs[index]->getStartTime(j) + this->jobs[index]->getP(j))
                    job->setStartTime(j, job->getStartTime(j - 1) + job->getP(j - 1));
                else
                    job->setStartTime(j, this->jobs[index]->getStartTime(j) + this->jobs[index]->getP(j));
            }
        }


    }
    else
    {
        // Update the start times of the job and the jobs after it

        for (int j = 0; j < this->m; j++)
        {
            job->setStartTime(j, this->jobs[pos]->getStartTime(j));
            if (j == 0)
            {
                for (int i = pos; i < this->jobs.size(); i++)
                {
                    this->jobs[i]->setStartTime(j, this->jobs[i]->getStartTime(j) + job->getP(j));
                }
            }
            else
            {
                for (int i = pos; i < this->jobs.size(); i++)
                {
                    if (i > pos)
                    {
                        if (this->jobs[i]->getStartTime(j) >= this->jobs[i - 1]->getStartTime(j) + this->jobs[i - 1]->getP(j))
                        {
                            if (this->jobs[i]->getStartTime(j) >= this->jobs[i]->getStartTime(j - 1) + this->jobs[i]->getP(j - 1))
                                break;
                            else
                                this->jobs[i]->setStartTime(j, this->jobs[i]->getStartTime(j - 1) + this->jobs[i]->getP(j - 1));
                        }
                        else
                        {
                            float time = this->jobs[i - 1]->getStartTime(j) + this->jobs[i - 1]->getP(j);
                            if (time < this->jobs[i]->getStartTime(j - 1) + this->jobs[i]->getP(j - 1))
                                time = this->jobs[i]->getStartTime(j - 1) + this->jobs[i]->getP(j - 1);
                            this->jobs[i]->setStartTime(j, time);
                        }
                    }
                    else
                        this->jobs[i]->setStartTime(j, this->jobs[i]->getStartTime(j) + job->getP(j));
                }
            }
        }

        // Insert the job
        this->jobs.insert(this->jobs.begin() + pos, job);
    }

    this->TECChanged = true;
    this->TFTChanged = true;
}
string Factory::generateCSV()
{
    this->initializeJobsStartTimes();
    string csvString = "";

    for (int i = 0; i < this->jobs.size(); i++)
    {
        csvString += "tj" + to_string(i) + ",td" + to_string(i) + ",";
    }
    csvString.pop_back();
    csvString += "\n";

    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->jobs.size(); j++)
        {
            csvString += to_string(this->jobs[j]->getStartTime(i)) + "," + to_string(this->jobs[j]->getP(i)) + ",";
        }
        csvString.pop_back();
        csvString += "\n";
    }

    return csvString;
}

int Factory::getNumJobs()
{

    return this->jobs.size();
}

int Factory::getNumMachines()
{

    return this->m;
}

Job *Factory::getJob(int id)
{
    return this->jobs[id];
}

void Factory::setTECTFTChanged(){
    this->TECChanged =true;
    this->TFTChanged=true;
}

void Factory::setVForJobMachine(int job, int machine, float speed){
    this->jobs[job]->setVForMachine(machine, speed);
    this->TECChanged =true;
    this->TFTChanged=true;
}