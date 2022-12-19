#include "defines.hpp"

int main(){
    cout << "Hello"<<endl;

    // criar instancia

     Instance* instance = readFile("../instances/928/2-4-20__0.txt");
     Solution* test = instance->maxSMinTFT();

    // delete instance;
    // delete test;

    vector<float> t1 = {1.0,3.0,1.0};
    vector<float> v1 = {1.0,1.0,1.0};
    Job* job1 = new Job(0);
    job1->setT(t1);
    job1->setV(v1);

    vector<float> t2 = {2.0,1.0,2.0};
    vector<float> v2 = {1.0,1.0,1.0};
    Job* job2 = new Job(1);
    job2->setT(t2);
    job2->setV(v2);

    vector<float> t3 = {3.0,2.0,1.0};
    vector<float> v3 = {1.0,1.0,1.0};
    Job* job3 = new Job(2);
    job3->setT(t3);
    job3->setV(v3);

    Factory* f = new Factory(0, 3);
    f->addJobAtLastPosition(job1);
    f->addJobAtLastPosition(job2);
    f->addJobAtLastPosition(job3);

    float tft = f->getTFT();
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