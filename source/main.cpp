#include "defines.hpp"


void outputToFile(string path, string text){
    ofstream outputf(path);
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

string runExperiment(string path, int iterations, int nsgaIterations, float stopTime, int baseSeed){

    string csv = "id,baseSeed,nsgaIterations,Nmetric\n";
    vector<vector<Solution*>> allNonDominatedFronts;

    clock_t start, end;

    for(int i=0; i<iterations; i++){
        Instance *instance = readFile(path);

        start = clock();

        for (int i = 0; i < 10; i++)
        {
            instance->balancedRandomSolutionGenerator(i+baseSeed);
            instance->randSMinTEC(i+baseSeed);
            instance->randSMinTFT(i+baseSeed);
        }
        instance->minSMinTEC();
        instance->maxSMinTFT();
        instance->assignCrowdingDistance();

        //for (int j = 0; j < 100; j++)
        int its = 0;
        while(true)
        {
            end = clock();
            double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
            if (time_taken > instance->get_n()/2)
            {
                cout << "Time's up! " << its << " iterations in " << time_taken << " seconds" << endl;
                break;
            }

            instance->NSGA2NextGen(its+baseSeed);
            its++;
        }

        csv += path + "," + to_string(baseSeed) + "," + to_string(nsgaIterations) + "," + to_string(instance->nMetric()) + "\n";

        //allNonDominatedFronts.push_back(instance->getParetoFront())
        delete instance;
    }


    return csv;

}

void test5(){

    string path =  "../instances/928/2-4-20__0.txt";
    string csv = runExperiment("../instances/928/2-4-20__0.txt", 10, 10, 0.0, 0);
    outputToFile("../analysis/2-4-20__0_results.csv", csv);

    //csv = runExperiment("../instances/928/2-4-40__1.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-4-40__1_results.csv", csv);

    //csv = runExperiment("../instances/928/2-4-60__2.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-4-60__2_results.csv", csv);

    //csv = runExperiment("../instances/928/2-4-80__3.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-4-80__3_results.csv", csv);

    //csv = runExperiment("../instances/928/2-4-100__4.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-4-100__4_results.csv", csv);

    //csv = runExperiment("../instances/928/2-8-20__5.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-8-20__5_results.csv", csv);

    //csv = runExperiment("../instances/928/2-8-40__6.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-8-40__6_results.csv", csv);

    //csv = runExperiment("../instances/928/2-8-60__7.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-8-60__7_results.csv", csv);

    //csv = runExperiment("../instances/928/2-8-80__8.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-8-80__8_results.csv", csv);

    //csv = runExperiment("../instances/928/2-8-100__9.txt", 10, 10, 0.0, 0);
    //outputToFile("../analysis/2-8-100__9_results.csv", csv);

}

void test4(){
    string path =  "../instances/928/2-4-20__0.txt";
    Instance *instance = readFile(path);
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
    outputToFile("../analysis/initial_pop_1.csv", instance->generatePopulationCSVString());
    for (int i = 0; i < 10; i++)
    {
        instance->NSGA2NextGen(i);
    }
    outputToFile("../analysis/after_nsga2_1.csv", instance->generatePopulationCSVString());

    delete instance;

}

void test3(){
    Instance *instance = readFile("../instances/test_instance1.txt");
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

void test2()
{
    //string path =  "../instances/test_instance1.txt";
    string path =  "../instances/928/2-4-20__0.txt";
    Instance *instance = readFile(path);
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



    Instance *instance2 = readFile(path);
    instance2->maxSMinTFT();
    ofstream output2("../analysis/max_s_min_tft.csv");
    output2 << instance2->generatePopulationCSVString();
    output2.close();

    delete instance2;

    Instance *instance3 = readFile(path);
    for (int i = 0; i < 100; i++)
    {
        instance3->randSMinTFT(i);
    }
    ofstream output3("../analysis/rand_s_min_tft.csv");
    output3 << instance3->generatePopulationCSVString();
    output3.close();

    delete instance3;




    Instance *instance5 = readFile(path);
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



    Instance *instance4 = readFile(path);
    instance4->minSMinTEC();
    ofstream output4("../analysis/min_s_min_tec.csv");
    output4 << instance4->generatePopulationCSVString();
    output4.close();

    delete instance4;



}

void test()
{
    // criar instancia

    Instance *instance = readFile("../instances/928/2-4-20__0.txt");
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
    outputToFile("../analysis/factory_test_right_shift_after.csv", f->generateCSV());

    Factory *f2 = s->getFactory(1);
    f2->addJobAtLastPosition(job4);
    f2->addJobAtLastPosition(job5);
    f2->addJobAtLastPosition(job6);

    delete s;

    // float tft = f->getTFT();
}

int main()
{
    cout << "Hello" << endl;

    test5();
    return 0;

    // inicializa o construtivo

    // Leitura retorna uma instancia

    cout << "Iniciando leitura da instância...\n"
         << endl;
    Instance *inst = readFile("./instances/294/3-4-20__15.txt");
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
