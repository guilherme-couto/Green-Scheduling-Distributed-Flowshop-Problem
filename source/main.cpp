#include "defines.hpp"


void test3(){
    Instance *instance = readFile("../instances/test_instance1.txt");
    for (int i = 0; i < 100; i++)
    {
       instance->randomSolutionGenerator(i);
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
        instance->randomSolutionGenerator(i);
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
        instance->randomSolutionGenerator(i);
    }
    ofstream output("../pop_output.csv");
    output << instance->generatePopulationCSVString();
    output.close();

    delete instance;

    Solution *s = new Solution(6, 3, 2);

    vector<int> t1 = {1, 3, 1};
    vector<float> v1 = {1.0, 1.0, 1.0};
    Job *job1 = new Job(0, 3);
    job1->setT(t1);
    job1->setV(v1);

    vector<int> t2 = {2, 1, 2};
    vector<float> v2 = {1.0, 1.0, 1.0};
    Job *job2 = new Job(1, 3);
    job2->setT(t2);
    job2->setV(v2);

    vector<int> t3 = {3, 2, 1};
    vector<float> v3 = {1.0, 1.0, 1.0};
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
    test();
    test3();
    test2();
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
