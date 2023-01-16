//
// Created by patri on 12/01/2023.
//

#include "defines.hpp"

vector<Solution*> Util::allocatedSolutions;

vector<vector<Solution*>> Util::fastNonDominatedSort(vector<Solution*> population) {

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
    return solutionFronts;
}
Solution* extremeTEC(vector<Solution*> &v, bool maxExtreme)
{
    float extreme;
    Solution* extremeSolution;

    if(maxExtreme){
        extreme=0;
    }else{
        extreme=INFINITY;
    }

    for(int i=0; i<v.size(); i++){
        if(maxExtreme){
            if(v[i]->getTEC() > extreme){
                extreme = v[i]->getTEC();
                extremeSolution = v[i];
            }
        }else{
            if(v[i]->getTEC() < extreme){
                extreme = v[i]->getTEC();
                extremeSolution = v[i];
            }
        }
    }

    return extremeSolution;
}


Solution* Util::maxTECSol(vector<Solution*> &v){
    float max=0;
    Solution* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTEC() > max){
            max = v[i]->getTEC();
            sol = v[i];
        }
    }
    return sol;
}

Solution* Util::maxTFTSol(vector<Solution*> &v){
    float max=0;
    Solution* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTFT() > max){
            max = v[i]->getTFT();
            sol = v[i];
        }
    }
    return sol;
}


Solution* Util::minTECSol(vector<Solution*> &v){
    float min=INFINITY;
    Solution* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTEC() < min){
            min = v[i]->getTEC();
            sol = v[i];
        }
    }
    return sol;
}

Solution* Util::minTFTSol(vector<Solution*> &v){
    float min=INFINITY;
    Solution* sol;

    for(int i=0; i<v.size(); i++){
        if(v[i]->getTFT() < min){
            min = v[i]->getTFT();
            sol = v[i];
        }
    }
    return sol;
}



float euclideanDistance(Solution* sol1, Solution* sol2){
    float sol1Y = sol1->getTEC();
    float sol1X = sol1->getTFT();
    float sol2Y = sol2->getTEC();
    float sol2X = sol2->getTFT();

    float distance = sqrtf(pow(sol1Y-sol2Y, 2) + pow(sol1X - sol2X, 2));
    return distance;
}

float normalizedEuclideanDistance(Solution* sol1, Solution* sol2, float maxTFT, float minTFT, float maxTEC, float minTEC){
    float sol1TEC = sol1->getTEC();
    float sol1TFT = sol1->getTFT();
    float sol2TEC = sol2->getTEC();
    float sol2TFT = sol2->getTFT();

    float distance =
            sqrtf(
                pow((sol1TEC - sol2TEC) / (maxTEC - minTEC), 2) +
                pow((sol1TFT - sol2TFT) / (maxTFT - minTFT), 2)
            );
    return distance;
}

float Util::DMetric(vector<Solution*> &PF, vector<Solution*> &A){

    float sum = 0;
    for(int i=0; i < PF.size(); i++){
        float min =INFINITY;
        for(int j=0; j < A.size(); j++){
            float distance = euclideanDistance(PF[i], A[j]);
            if(distance<min){
                min = distance;
            }
        }
        sum+=min;
    }

    return sum/PF.size();
}

float Util::GDMetric(vector<Solution*> &PF, vector<Solution*> &A){
    float minTEC = minTECSol(PF)->getTEC();
    float minTFT = minTFTSol(PF)->getTFT();
    float maxTEC = maxTECSol(PF)->getTEC();
    float maxTFT = maxTFTSol(PF)->getTFT();

    float sum = 0;
    for(int i=0; i < A.size(); i++){
        float min =INFINITY;
        for(int j=0; j < PF.size(); j++){
            float distance = normalizedEuclideanDistance(PF[j], A[i], maxTFT, minTFT, maxTEC, minTEC);
            if(distance<min){
                min = distance;
            }
        }
        sum+=pow(min, 2);
    }

    return sqrtf(sum)/A.size();
}

float Util::IGDMetric(vector<Solution*> &PF, vector<Solution*> &A){
    float minTEC = minTECSol(PF)->getTEC();
    float minTFT = minTFTSol(PF)->getTFT();
    float maxTEC = maxTECSol(PF)->getTEC();
    float maxTFT = maxTFTSol(PF)->getTFT();


    float sum = 0;
    for(int i=0; i < PF.size(); i++){
        float min =INFINITY;
        for(int j=0; j < A.size(); j++){
            float distance = normalizedEuclideanDistance(PF[i], A[j], maxTFT, minTFT, maxTEC, minTEC);
            if(distance<min){
                min = distance;
            }
        }
        sum+=pow(min, 2);
    }

    return sqrtf(sum)/PF.size();
}



float Util::SMetric(vector<Solution*> &PF, vector<Solution*> &A) {

    float diSum, dMean = 0;
    vector<float> d;
    d.reserve(A.size());

    for(int i=0; i < A.size(); i++){
        float min =INFINITY;
        for(int j=0; j < A.size(); j++){
            float distance = euclideanDistance(A[i], A[j]);
            if(j!=i && distance<min){
                min = distance;
            }
        }
        diSum+=min;
        d.push_back(min);
    }

    dMean = diSum / A.size();
    float term2Sum = 0;
    for(int i=0; i< d.size(); i++){
        term2Sum+= fabsf(d[i] - dMean);
    }


    float term1Sum = euclideanDistance(minTECSol(PF), minTECSol(A))
            + euclideanDistance(minTFTSol(PF), minTFTSol(A));

    float result = (term1Sum + term2Sum)/(term1Sum + A.size()*dMean);

    return result;
}

string Util::generateCSV(Factory* factory)
{
    factory->initializeJobsStartTimes();
    string csvString = "";

    for (int i = 0; i < factory->getNumJobs(); i++)
    {
        csvString += "tj" + to_string(i) + ",td" + to_string(i) + ",";
    }
    csvString.pop_back();
    csvString += "\n";

    for (int i = 0; i < factory->getNumMachines(); i++)
    {
        vector<Job*> jobs = factory->getJobs();
        for (int j = 0; j < factory->getNumJobs(); j++)
        {
            csvString += to_string(jobs[j]->getStartTime(i)) + "," + to_string(jobs[j]->getP(i)) + ",";
        }
        csvString.pop_back();
        csvString += "\n";
    }

    return csvString;
}

void Util::allocate(Solution* sol){
    Util::allocatedSolutions.push_back(sol);

}

void Util::deallocate(){
    for(Solution* s: Util::allocatedSolutions){
        delete s;
    }

    Util::allocatedSolutions.clear();
}