#include "defines.hpp"


void test(){
    // criar instancia

    Instance* instance = readFile("../instances/928/2-4-20__0.txt");
    Solution* test = instance->maxSMinTFT();

    delete instance;
    delete test;

    Solution* s = new Solution(6, 3, 2);

    vector<float> t1 = {4.0,2.0,1.0};
    vector<float> v1 = {1.0,1.0,1.0};
    Job* job1 = new Job(0);
    job1->setT(t1);
    job1->setV(v1);

    vector<float> t2 = {2.0,1.0,2.0};
    vector<float> v2 = {1.0,1.0,1.0};
    Job* job2 = new Job(1);
    job2->setT(t2);
    job2->setV(v2);

    vector<float> t3 = {3.5,2.0,3.0};
    vector<float> v3 = {1.0,1.0,1.0};
    Job* job3 = new Job(2);
    job3->setT(t3);
    job3->setV(v3);

    vector<float> t4 = {2.0,2.0,4.0};
    vector<float> v4 = {1.0,1.0,1.0};
    Job* job4 = new Job(3);
    job4->setT(t4);
    job4->setV(v4);

    vector<float> t5 = {2.0,1.0,3.0};
    vector<float> v5 = {1.0,1.0,1.0};
    Job* job5 = new Job(4);
    job5->setT(t5);
    job5->setV(v5);

    vector<float> t6 = {3.0,3.0,2.0};
    vector<float> v6 = {1.0,1.0,1.0};
    Job* job6 = new Job(5);
    job6->setT(t6);
    job6->setV(v6);

    Factory* f = s->getFactory(0);
    f->addJobAtLastPosition(job5);
    f->addJobAtLastPosition(job2);
    f->addJobAtLastPosition(job1);

    Factory* f2 = s->getFactory(1);
    f2->addJobAtLastPosition(job4);
    f2->addJobAtLastPosition(job3);
    f2->addJobAtLastPosition(job6);



    delete s;

    //float tft = f->getTFT();

}

int main(){
    cout << "Hello"<<endl;
    test();

    // inicializa o construtivo

    // Leitura retorna uma instancia

    cout << "Iniciando leitura da instância...\n" << endl;
    Instance* inst = readFile("./instances/294/3-4-20__15.txt");
    cout << "Instância lida com sucesso!\n" << endl;

    cout << "F = " << inst->getF() << endl;
    cout << "n = " << inst->get_n() << endl;
    cout << "m = " << inst->get_m() << endl;

    inst->construtivo();
    inst->printPopulation();

    cout << "\nFIM\n";

    return 0;
    
}

