#include "defines.hpp"

int main(){
    cout << "Hello"<<endl;

    // criar instancia

  //  readFile("../instances/2-4-20__0.txt");

    vector<float> t1 = {1.0,3.0,1.0};
    vector<float> v1 = {1.0,1.0,1.0};
    Job job1 = Job(0);
    job1.setT(t1);
    job1.setV(v1);

    vector<float> t2 = {2.0,1.0,2.0};
    vector<float> v2 = {1.0,1.0,1.0};
    Job job2 = Job(1);
    job2.setT(t2);
    job2.setV(v2);

    vector<float> t3 = {3.0,2.0,1.0};
    vector<float> v3 = {1.0,1.0,1.0};
    Job job3 = Job(2);
    job3.setT(t3);
    job3.setV(v3);

    Factory f = Factory(0);
    f.setM(3);
    f.addJob(job1);
    f.addJob(job2);
    f.addJob(job3);

    float tft = f.getTFT();
    // inicializa o construtivo

    // Leitura retorna uma instancia
    return 0;
    
}