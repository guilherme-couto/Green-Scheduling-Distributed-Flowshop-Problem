#include "defines.hpp"
#include <stdlib.h>

void outputToFile(string path, string text, bool append){
    ofstream outputf;

    if(append){
        outputf.open(path, std::ios_base::app);
    }else{
        outputf.open(path);
    }

    outputf << text;
    outputf.close();
}

vector<Solution*> joinFronts(vector<vector<Solution*>> fronts){
    vector<Solution*> result ;

    for(int i=0; i< fronts.size(); i++){
        for(int j=0; j< fronts[i].size(); j++){
            result.push_back(fronts[i][j]);
        }
    }

    return result;
}

float meanDMetric(vector<vector<Solution*>> &paretoArchive, vector<Solution*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::DMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float meanSMetric(vector<vector<Solution*>> &paretoArchive, vector<Solution*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::SMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float meanGDMetric(vector<vector<Solution*>> &paretoArchive, vector<Solution*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::GDMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}

float meanIGDMetric(vector<vector<Solution*>> &paretoArchive, vector<Solution*> &PF){

    float sum = 0;
    for(int i=0; i< paretoArchive.size(); i++){
        sum+= Util::IGDMetric(PF, paretoArchive[i]);
    }

    return sum/paretoArchive.size();
}


string runExperiment(string path, int iterations, float stopTime, int baseSeed){

    //string csv = "id,baseSeed,iterations,nsgaIterations,N,D(antiga), GD, IGD, S\n";
    string csv = "";
    vector<vector<Solution*>> paretoArchive;
    vector<Problem*> instances;
    clock_t start, end;

    int nsgaIterationsSum = 0;
    for(int i=0; i<iterations; i++){
        Problem *instance = readFile(path);
        instances.push_back(instance);

        start = clock();

        for (int j = 0; j < 10; j++)
        {
            instance->balancedRandomSolutionGenerator(j+baseSeed);
            instance->randSMinTEC(j+baseSeed);
            instance->randSMinTFT(j+baseSeed);
        }
        instance->minSMinTEC();
        instance->maxSMinTFT();

        instance->assignCrowdingDistance();

        int counter = 0;
        while(true)
        {
            end = clock();
            double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
            if (time_taken > instance->get_n()/2)
            {
                cout << "Time's up! " << counter << " iterations in " << time_taken << " seconds" << endl;
                break;
            }

            instance->NSGA2NextGen(nsgaIterationsSum+baseSeed);

            nsgaIterationsSum++;
            counter ++;
        }

        instance->fastNonDominatedSort();
        paretoArchive.push_back(instance->getParetoFront());
        //csv += path + "," + to_string(baseSeed) + "," + to_string(nsgaIterations) + "," + to_string(instance->nMetric()) + "\n";

        //delete instance;
    }

    vector<Solution*> joinedParetoArchive = joinFronts(paretoArchive);
    vector<Solution*> archiveParetoFront = Util::fastNonDominatedSort(joinedParetoArchive)[0];

    csv += path + "," + to_string(baseSeed)
            + "," + to_string(iterations)
            + "," + to_string((float)nsgaIterationsSum/(float)iterations)
            + "," + to_string(archiveParetoFront.size())
            + "," + to_string(meanDMetric(paretoArchive, archiveParetoFront))
            + "," + to_string(meanGDMetric(paretoArchive, archiveParetoFront))
            + "," + to_string(meanIGDMetric(paretoArchive, archiveParetoFront))
            + "," + to_string(meanSMetric(paretoArchive, archiveParetoFront))
            + "\n";

    //for(Instance* i:instances){
    //    delete i;
   // }

    Util::deallocate();

    return csv;

}

string runExperiment2(string path, int iterations, float stopTime, int baseSeed, string option){

    //string csv = "id,baseSeed,iterations,nsgaIterations,N,D(antiga), GD, IGD, S\n";
    string csv = "";
    vector<vector<Solution*>> paretoArchive;
    vector<vector<Solution*>> alg1ParetoArchive;
    vector<vector<Solution*>> alg2ParetoArchive;
    vector<vector<Solution*>> alg3ParetoArchive;
    vector<Problem*> instances;
    clock_t start, end;

    int nsgaIterationsSum = 0;
    for(int i=0; i<iterations; i++){
        Problem *instance = readFile(path);
        instances.push_back(instance);

        start = clock();

        for (int j = 0; j < 10; j++)
        {
            instance->balancedRandomSolutionGenerator(j+baseSeed);
            instance->randSMinTEC(j+baseSeed);
            instance->randSMinTFT(j+baseSeed);
        }
        instance->minSMinTEC();
        instance->maxSMinTFT();

        instance->assignCrowdingDistance();

        int counter = 0;
        while(true)
        {
            end = clock();
            double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
            if (time_taken > instance->get_n()/2)
            {
                cout << "Time's up! " << counter << " iterations in " << time_taken << " seconds" << endl;
                break;
            }

            if(option == "v3")
                instance->NSGA2NextGen(nsgaIterationsSum+baseSeed);
            else if (option == "op")
                instance->NSGA2NextGen_operators(nsgaIterationsSum+baseSeed);

            nsgaIterationsSum++;
            counter ++;
        }

        instance->fastNonDominatedSort();
        paretoArchive.push_back(instance->getParetoFront());
        //csv += path + "," + to_string(baseSeed) + "," + to_string(nsgaIterations) + "," + to_string(instance->nMetric()) + "\n";

        //delete instance;
    }

    vector<Solution*> joinedParetoArchive = joinFronts(paretoArchive);
    vector<Solution*> archiveParetoFront = Util::fastNonDominatedSort(joinedParetoArchive)[0];

    csv += path + "," + to_string(baseSeed)
           + "," + to_string(iterations)
           + "," + to_string((float)nsgaIterationsSum/(float)iterations)
           + "," + to_string(archiveParetoFront.size())
           + "," + to_string(meanDMetric(paretoArchive, archiveParetoFront))
           + "," + to_string(meanGDMetric(paretoArchive, archiveParetoFront))
           + "," + to_string(meanIGDMetric(paretoArchive, archiveParetoFront))
           + "," + to_string(meanSMetric(paretoArchive, archiveParetoFront))
           + "\n";

    //for(Instance* i:instances){
    //    delete i;
    // }

    Util::deallocate();

    return csv;

}

vector<vector<Solution*>> getExperimentArchives(string path, int iterations, float stopTime, int baseSeed, int &its, string option){

    //string csv = "id,baseSeed,iterations,nsgaIterations,N,D(antiga), GD, IGD, S\n";
    string csv = "";
    vector<vector<Solution*>> paretoArchive;
    vector<Problem*> instances;
    clock_t start, end;

    int nsgaIterationsSum = 0;
    for(int i=0; i<iterations; i++){
        Problem *instance = readFile(path);
        instances.push_back(instance);

        start = clock();

        for (int j = 0; j < 10; j++)
        {
            instance->balancedRandomSolutionGenerator(j+baseSeed);
            instance->randSMinTEC(j+baseSeed);
            instance->randSMinTFT(j+baseSeed);
        }
        instance->minSMinTEC();
        instance->maxSMinTFT();

        instance->assignCrowdingDistance();

        int counter = 0;
        while(true)
        {
            end = clock();
            double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
            if (time_taken > instance->get_n()/2 || counter >=1000)
            {
                cout << "Time's up! " << counter << " iterations in " << time_taken << " seconds" << endl;
                break;
            }

            if(option == "nsga2-v3")
                instance->NSGA2NextGen(nsgaIterationsSum+baseSeed);
            else if (option == "nsga2-nd")
                instance->NSGA2NextGen_operators_ND(nsgaIterationsSum+baseSeed);
            else if (option == "nsga3-v3")
                instance->NSGA3NextGen(nsgaIterationsSum+baseSeed);
            else if (option == "nsga3-nd")
                instance->NSGA3NextGen_operators_ND(nsgaIterationsSum+baseSeed);

            nsgaIterationsSum++;
            counter ++;
        }

        its = nsgaIterationsSum;
        instance->fastNonDominatedSort();
        paretoArchive.push_back(instance->getParetoFront());
        //csv += path + "," + to_string(baseSeed) + "," + to_string(nsgaIterations) + "," + to_string(instance->nMetric()) + "\n";

        //delete instance;
    }

    return paretoArchive;

    vector<Solution*> joinedParetoArchive = joinFronts(paretoArchive);
    vector<Solution*> archiveParetoFront = Util::fastNonDominatedSort(joinedParetoArchive)[0];

    csv += path + "," + to_string(baseSeed)
           + "," + to_string(iterations)
           + "," + to_string((float)nsgaIterationsSum/(float)iterations)
           + "," + to_string(archiveParetoFront.size())
           + "," + to_string(meanDMetric(paretoArchive, archiveParetoFront))
           + "," + to_string(meanGDMetric(paretoArchive, archiveParetoFront))
           + "," + to_string(meanIGDMetric(paretoArchive, archiveParetoFront))
           + "," + to_string(meanSMetric(paretoArchive, archiveParetoFront))
           + "\n";

    //for(Instance* i:instances){
    //    delete i;
    // }

    Util::deallocate();

   // return csv;

}

string algorithmCSV(string path, string alg, int baseSeed, int nsgaIterationsSum, int iterations, int N, vector<Solution*> trueParetoFront, vector<vector<Solution*>> algParetoArchive){
    return path + ","+alg +"," + to_string(baseSeed)
           + "," + to_string(iterations)
           + "," + to_string((float)nsgaIterationsSum/(float)iterations)
           + "," + to_string(N)
           + "," + to_string(meanDMetric(algParetoArchive, trueParetoFront))
           + "," + to_string(meanGDMetric(algParetoArchive, trueParetoFront))
           + "," + to_string(meanIGDMetric(algParetoArchive, trueParetoFront))
           + "," + to_string(meanSMetric(algParetoArchive, trueParetoFront))
           + "\n";
}

string runExperiment3(string path, int iterations, float stopTime, int baseSeed, string option){
    string csv = "";

    vector<vector<Solution*>> alg1ParetoFronts;
    vector<vector<Solution*>> alg2ParetoFronts;
    vector<vector<Solution*>> alg3ParetoFronts;
    vector<vector<Solution*>> alg4ParetoFronts;

    int alg1Its=0;
    alg1ParetoFronts = getExperimentArchives(path, iterations, stopTime, baseSeed, alg1Its, "nsga2-v3");
    int alg2Its=0;
    alg2ParetoFronts = getExperimentArchives(path, iterations, stopTime, baseSeed, alg2Its, "nsga2-nd");
    int alg3Its=0;
    alg3ParetoFronts = getExperimentArchives(path, iterations, stopTime, baseSeed,alg3Its, "nsga3-v3");
    int alg4Its=0;
    alg4ParetoFronts = getExperimentArchives(path, iterations, stopTime, baseSeed,alg4Its, "nsga3-nd");

    vector<Solution*> alg1UnifiedParetoArchive = joinFronts(alg1ParetoFronts);
    vector<vector<Solution*>> alg1FirstFront = Util::fastNonDominatedSort(alg1UnifiedParetoArchive);

    vector<Solution*> alg2UnifiedParetoArchive = joinFronts(alg2ParetoFronts);
    vector<vector<Solution*>> alg2FirstFront = Util::fastNonDominatedSort(alg2UnifiedParetoArchive);

    vector<Solution*> alg3UnifiedParetoArchive = joinFronts(alg3ParetoFronts);
    vector<vector<Solution*>> alg3FirstFront = Util::fastNonDominatedSort(alg3UnifiedParetoArchive);

    vector<Solution*> alg4UnifiedParetoArchive = joinFronts(alg4ParetoFronts);
    vector<vector<Solution*>> alg4FirstFront = Util::fastNonDominatedSort(alg4UnifiedParetoArchive);


    vector<vector<Solution*>> allAlgsFronts{alg1UnifiedParetoArchive, alg2UnifiedParetoArchive, alg3UnifiedParetoArchive, alg4UnifiedParetoArchive};
    vector<Solution*> trueParetoFront = joinFronts(allAlgsFronts);

    trueParetoFront = Util::fastNonDominatedSort(trueParetoFront)[0];

    csv += algorithmCSV(path, "nsga2-v3", baseSeed, alg1Its, iterations, alg1FirstFront.size(), trueParetoFront,
                        alg1ParetoFronts);

    csv += algorithmCSV(path, "nsga2-nd", baseSeed, alg2Its, iterations, alg2FirstFront.size(), trueParetoFront,
                        alg2ParetoFronts);

    csv += algorithmCSV(path, "nsga3-v3", baseSeed, alg3Its, iterations, alg3FirstFront.size(), trueParetoFront,
                        alg3ParetoFronts);

    csv += algorithmCSV(path, "nsga3-nd", baseSeed, alg4Its, iterations, alg4FirstFront.size(), trueParetoFront,
                        alg4ParetoFronts);


    Util::deallocate();

    return csv;

}




void test_final(){

    string csv;

    csv = "id, alg, baseSeed,iterations,nsgaIterations,N,D(antiga),GD,IGD,S\n";
    outputToFile("../analysis/results_op.csv", csv, true);

    string folderPath = "../instances/566";
    for(int i=0; i<10; i++){
        for(int j=20; j<=100; j+=20){
            for(int m=4; m<=16; m=m*2){
                for(int f=2; f<=5; f++){
                    string jobs = to_string(j);
                    string mach = to_string(m);
                    string fac = to_string(f);
                    string id = to_string(i);
                    string filePath = folderPath +"/"+ fac+ "-"+mach+"-" +jobs+"__"+id+".txt";
                    if(i==0){
                        csv = runExperiment3(filePath, 10, 0.0, 0, "op");
                        outputToFile("../analysis/results_op.csv", csv, true);
                    }
                }
            }
        }


    }
}

void test_final_amostra(){

    string csv;


    csv = "id, alg, baseSeed,iterations,nsgaIterations,N,D(antiga),GD,IGD,S\n";
    outputToFile("../analysis/results_op.csv", csv, true);


    csv = runExperiment3("../instances/566/2-4-20__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-4-60__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-4-100__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-4-20__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-4-60__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-4-100__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-8-20__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-8-60__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-8-100__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-8-20__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-8-60__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-8-100__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-16-20__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-16-60__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-16-100__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-16-20__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-16-60__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-16-100__0.txt", 10, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);


}

void test_final_amostra_2_fab(){

    string csv;

    csv = "id, alg, baseSeed,iterations,nsgaIterations,N,D(antiga),GD,IGD,S\n";
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-4-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-4-60__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-4-100__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-8-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-8-60__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-8-100__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-16-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-16-60__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-16-100__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);


}

void test_final_amostra_20(){

    string csv;


    csv = "id, alg, baseSeed,iterations,nsgaIterations,N,D(antiga),GD,IGD,S\n";
    outputToFile("../analysis/results_op.csv", csv, true);


    csv = runExperiment3("../instances/566/2-4-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-4-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-8-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-8-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/2-16-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

    csv = runExperiment3("../instances/566/5-16-20__0.txt", 5, 0.0, 0, "op");
    outputToFile("../analysis/results_op.csv", csv, true);

}


void test()
{
    // criar instancia

    Problem *instance = readFile("../instances/928/2-4-20__0.txt");
    Solution *test = instance->maxSMinTFT();
    Solution *test2 = instance->randSMinTFT(0);

    for (int i = 0; i < 100; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
    }
    ofstream output("../pop_output.csv");
    output << instance->generatePopulationCSVString();
    output.close();

    delete instance;

    Solution *s = new Solution(6, 3, 2);

    vector<int> t1 = {1, 3, 3};
    vector<float> v1 = {2.1, 2.1, 2.1};
    Job *job1 = new Job(0, 3);
    job1->setT(t1);
    job1->setV(v1);

    vector<int> t2 = {2, 1, 2};
    vector<float> v2 = {2.1, 2.1, 2.1};
    Job *job2 = new Job(1, 3);
    job2->setT(t2);
    job2->setV(v2);

    vector<int> t3 = {3, 2, 1};
    vector<float> v3 = {2.1, 2.1, 2.1};
    Job *job3 = new Job(2, 3);
    job3->setT(t3);
    job3->setV(v3);

    vector<int> t4 = {2, 2, 4};
    vector<float> v4 = {1.0, 1.0, 1.0};
    Job *job4 = new Job(3, 3);
    job4->setT(t4);
    job4->setV(v4);

    vector<int> t5 = {2, 1, 3};
    vector<float> v5 = {1.0, 1.0, 1.0};
    Job *job5 = new Job(4, 3);
    job5->setT(t5);
    job5->setV(v5);

    vector<int> t6 = {3, 3, 2};
    vector<float> v6 = {1.0, 1.0, 1.0};
    Job *job6 = new Job(5, 3);
    job6->setT(t6);
    job6->setV(v6);

    Factory *f = s->getFactory(0);
    f->addJobAtLastPosition(job1);
    f->addJobAtLastPosition(job2);
    f->addJobAtLastPosition(job3);

    ofstream outputf("../analysis/factory_test_right_shift_before.csv");
    outputf << f->generateCSV();
    outputf.close();

    /*ofstream outputf2("../analysis/factory_test_speed_up.csv");
    f->speedUp();
    outputf2 << f->generateCSV();
    outputf2.close();*/


    //f->speedDown();
    f->rightShift();
    outputToFile("../analysis/factory_test_right_shift_after.csv", f->generateCSV(),false);

    Factory *f2 = s->getFactory(1);
    f2->addJobAtLastPosition(job4);
    f2->addJobAtLastPosition(job5);
    f2->addJobAtLastPosition(job6);

    delete s;

    // float tft = f->getTFT();
}

void test2()
{
    //string path =  "../instances/test_instance1.txt";
    string path =  "../instances/566/2-8-100__0.txt";
    Problem *instance = readFile(path);
    for (int i = 0; i < 100; i++)
    {
        instance->totalRandomSolutionGenerator(i);
    }
    vector<vector<Solution *>> fronts = instance->fastNonDominatedSort();

    ofstream output("../analysis/pop_output.csv");
    output << instance->generatePopulationCSVString();
    output.close();
    instance->assignCrowdingDistance();

    delete instance;



    Problem *instance2 = readFile(path);
    instance2->maxSMinTFT();
    ofstream output2("../analysis/max_s_min_tft.csv");
    output2 << instance2->generatePopulationCSVString();
    output2.close();

    delete instance2;

    Problem *instance3 = readFile(path);
    for (int i = 0; i < 100; i++)
    {
        instance3->randSMinTFT(i);
    }
    ofstream output3("../analysis/rand_s_min_tft.csv");
    output3 << instance3->generatePopulationCSVString();
    output3.close();

    delete instance3;




    Problem *instance5 = readFile(path);
    for (int i = 0; i < 100; i++)
    {
        instance5->randSMinTEC(i);
    }
    instance5->fastNonDominatedSort();

    ofstream output5("../analysis/rand_s_min_tec.csv");
    output5 << instance5->generatePopulationCSVString();
    output5.close();
    instance5->assignCrowdingDistance();

    delete instance5;



    Problem *instance4 = readFile(path);
    instance4->minSMinTEC();
    ofstream output4("../analysis/min_s_min_tec.csv");
    output4 << instance4->generatePopulationCSVString();
    output4.close();

    delete instance4;



}

void test3(){
    Problem *instance = readFile("../instances/test_instance1.txt");
    for (int i = 0; i < 100; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
    }
    instance->maxSMinTFT();
    vector<vector<Solution *>> fronts = instance->fastNonDominatedSort();
    instance->printPopulation();
    ofstream output("../analysis/test_instance_pop.csv");
    output << instance->generatePopulationCSVString();
    output.close();

    delete instance;
}

void test4(){
    string path =  "../instances/928/2-4-20__0.txt";
    Problem *instance = readFile(path);
    for (int i = 0; i < 10; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();
    //
    outputToFile("../analysis/initial_pop_1.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 10; i++)
    {
        instance->NSGA2NextGen(i);
    }
    outputToFile("../analysis/after_nsga2_1.csv", instance->generatePopulationCSVString(), false);

    delete instance;

}

void test5(){

    //string path =  "../instances/928/2-4-20__0.txt";
    string csv = "id,baseSeed,iterations,nsgaIterations,N,D(antiga),GD,IGD,S\n";

    outputToFile("../analysis/results.csv", csv, true);

    //outputToFile("../analysis/test_results.csv", csv, true);

    //csv = runExperiment("../instances/test_instance1.txt", 10, 0.0, 0);
    //outputToFile("../analysis/test_results.csv", csv, true);


    csv = runExperiment("../instances/928/2-4-20__0.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-4-40__1.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-4-60__2.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-4-80__3.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-4-100__4.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-8-20__5.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-8-40__6.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-8-60__7.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-8-80__8.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

    csv = runExperiment("../instances/928/2-8-100__9.txt", 10, 0.0, 0);
    outputToFile("../analysis/results.csv", csv, true);

}

void test6(){
    string path =  "../instances/928/2-4-20__0.txt";
    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();

    outputToFile("../analysis/test_6_nsga2_only_swap/csv/before.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 100; i++)
    {
        instance->NSGA2NextGen_operators(i);
        //if(i%5==0) {
            outputToFile("../analysis/test_6_nsga2_only_swap/csv/after_" + to_string(i + 1) + ".csv",
                         instance->generatePopulationCSVString(), false);
        //}
    }
    //outputToFile("../analysis/after_nsga2_1.csv", instance->generatePopulationCSVString(), false);

    //delete instance;

}

void test7(){
    string path =  "../instances/928/2-4-20__0.txt";
    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();

    outputToFile("../analysis/test_7_nsga2_mnpv2/csv/before.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 100; i++)
    {
        instance->NSGA2NextGen(i);
        //if(i%5==0) {
        outputToFile("../analysis/test_7_nsga2_mnpv2/csv/after_" + to_string(i + 1) + ".csv",
                     instance->generatePopulationCSVString(), false);
        //}
    }
    //outputToFile("../analysis/after_nsga2_1.csv", instance->generatePopulationCSVString(), false);

    delete instance;

}

void test8(){
    string path =  "../instances/928/2-4-20__0.txt";
    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();

    outputToFile("../analysis/test_8_nsga2_mnpv3/csv/before.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 100; i++)
    {
        instance->NSGA2NextGen(i);
        //if(i%5==0) {
        outputToFile("../analysis/test_8_nsga2_mnpv3/csv/after_" + to_string(i + 1) + ".csv",
                     instance->generatePopulationCSVString(), false);
        //}
    }

    delete instance;

}

void test9(){
    string path =  "../instances/928/2-4-20__0.txt";
    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();

    outputToFile("../analysis/test_9_nsga2_mnpv3/csv/before.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 1000; i++)
    {
        instance->NSGA2NextGen(i);
        //if(i%5==0) {
        outputToFile("../analysis/test_9_nsga2_mnpv3/csv/after_" + to_string(i + 1) + ".csv",
                     instance->generatePopulationCSVString(), false);
        //}
    }

    delete instance;

}

void test10(){
    string path =  "../instances/928/2-4-20__0.txt";
    string outputDir = "../analysis/test_10_nsga2_swap_insert/csv/";
    //std::system(("python ./../create_folders.py "+ outputDir).c_str());

    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();
    outputToFile(outputDir+"/before.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 100; i++)
    {
        instance->NSGA2NextGen(i);
        //if(i%5==0) {
        outputToFile(outputDir+"/after_" + to_string(i + 1) + ".csv",
                     instance->generatePopulationCSVString(), false);
        //}
    }

    delete instance;

}

void test11(){
    string path =  "../instances/928/2-4-20__0.txt";
    string outputDir = "../analysis/test_11_ingm_sngm_hngm_rand/csv/";
    //std::system(("python ./../create_folders.py "+ outputDir).c_str());

    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();
    outputToFile(outputDir+"/before.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 100; i++)
    {
        instance->NSGA2NextGen(i);
        //if(i%5==0) {
        outputToFile(outputDir+"/after_" + to_string(i + 1) + ".csv",
                     instance->generatePopulationCSVString(), false);
        //}
    }


    delete instance;

}

void test12(){
    string path =  "../instances/928/2-4-20__0.txt";
    string outputDir = "../analysis/test_12_nsga3_mnpv3/csv/";
    //std::system(("python ./../create_folders.py "+ outputDir).c_str());

    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();
    outputToFile(outputDir+"/before.csv", instance->generatePopulationCSVString(), false);
    for (int i = 0; i < 100; i++)
    {
        instance->NSGA3NextGen(i);
        //if(i%5==0) {
        outputToFile(outputDir+"/after_" + to_string(i + 1) + ".csv",
                     instance->generatePopulationCSVString(), false);
        //}
    }


    delete instance;

}


void test13(){
    string path =  "../instances/566/5-4-20__0.txt";
    string outputDir = "../analysis/test_13_nsga2/csv/";
    //std::system(("python ./../create_folders.py "+ outputDir).c_str());

    Problem *instance = readFile(path);
    for (int i = 0; i < 7; i++)
    {
        instance->balancedRandomSolutionGenerator(i);
        instance->randSMinTEC(i);
        instance->randSMinTFT(i);
    }
    instance->minSMinTEC();
    instance->maxSMinTFT();
    instance->assignCrowdingDistance();
    outputToFile(outputDir+"/before.csv", instance->generatePopulationCSVString(), false);

    for (int i = 0; i < 100; i++)
    {
        cout << "Iteration "+ to_string(i)<<endl;
        instance->NSGA2NextGen(i);
        //if(i%5==0) {

        outputToFile(outputDir+"/after_" + to_string(i + 1) + ".csv",
                     instance->generatePopulationCSVString(), false);
        //}
    }


    delete instance;

}

int main()
{
    cout << "Hello" << endl;
    //test_final_amostra_2_fab();
    test2();
    return 0;

    // inicializa o construtivo

    // Leitura retorna uma instancia

    cout << "Iniciando leitura da instância...\n"
         << endl;
    Problem *inst = readFile("./instances/294/3-4-20__15.txt");
    cout << "Instância lida com sucesso!\n"
         << endl;

    cout << "F = " << inst->getF() << endl;
    cout << "n = " << inst->get_n() << endl;
    cout << "m = " << inst->get_m() << endl;

    inst->construtivo();
    inst->printPopulation();

    cout << "\nFIM\n";

    return 0;
}
