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
}


Instance::~Instance()
{
}

int Instance::get_t(int machine_id, int job_id) {
    return t[machine_id][job_id];
}

void Instance::construtivo()
{

    Xoshiro256plus rand(time(NULL));
    vector<int> prob{1, 1, 1, 1, 1, 1, 1, 1, 0, 0}; // 80%

    int random_number = rand.next() % prob.size();
    
}

bool compareJobsByTotalProcessingTime(Job a, Job b){
    return a.getTotalP()< b.getTotalP();
}


Solution* Instance::maxSMinTFT(){
    Solution* solution = new Solution(this->get_n(), this->get_m(), this->getF());
    vector<Job> jobs(this->get_n());
    vector<Factory> factories(this->getF());
    int highestSpeed = this->speeds.size()-1;

    for(int i=0; i<this->get_n(); i++){
        jobs[i] = Job(i);
        vector<float> jobTimeByMachine(this->get_m());
        vector<float> jobSpeedByMachine(this->get_m());

        for(int j=0; j< this->get_m(); j++){
            jobTimeByMachine[j] = this->t[i][j];
            jobSpeedByMachine[j] = this->speeds[highestSpeed];
        }
        jobs[i].setT(jobTimeByMachine);
        jobs[i].setV(jobSpeedByMachine);
    }


    sort(jobs.begin(), jobs.end(), compareJobsByTotalProcessingTime);     // generate job permutation lambda according to non-descending order of Ti (total job i processing time)


    for(int i=0; i<this->getF(); i++){ //assign the first f jobs to each one of the factories
        factories[i] = Factory(i);
        factories[i].addJob(jobs[i]);
    }

    //remove the assigned jobs from lambda

    for(int i = this->getF(); i<jobs.size(); i++) {                 //for each element in lambda
        Job job = jobs[i];                                          //get first job j of lambda
        for(int f=0; f<this->getF(); f++) {                         //for each factory f
            float dk = infinityf();                                 //dk = minimum increase in total flow time
            float ek = 0;                                           //ek =  position that resulted in dk
            for(int j=0; j<factories[f].getJobs().size(); j++) {    //test job j at all the possible positions of PI_k (the factory)
                //todo: cálculo do TFT na fábrica
                //todo: implementar teste de inserção do job em cada posição j
                if(factories[f].getTFT() < dk){
                    dk = factories[f].getTFT();
                    ek = j;
                }

            }
        }
        //k*= arg (min dk), k =1,2,...f
        //insert job j at position ek* of PI_k*
        //todo: função inserir job em uma posição

    }

    //speedDown(f), for all f's
    //return solution


/*
    vector<vector<float>> newSpeedVector(this->get_m());
    vector<vector<float>> processingSpeeds(this->get_m());
    vector<float> jobTimes(this->get_n(), 0.0);

    for(int i=0; i<this->get_m(); i++){
        vector<float> s_line(this->get_n());
        vector<float> p_line(this->get_n());

        for(int j=0;j<this->get_n(); j++){
            s_line[j] = this->speeds[this->speeds.size()-1];
            p_line[j] = this->get_t(i, j)/s_line[j];
            jobTimes[j]+=p_line[j];
        }
        newSpeedVector[i] = s_line;
        processingSpeeds[i] = p_line;
    }*/



    return solution;
}