#include "read.hpp"
#include "Matrices.hpp"
#include <iostream>


using namespace std;

vector<float> floatVectorFromText(string text){
    vector<float> result;
    string part;

    for(int i=0; i< text.size(); i++){

        if(text[i]==' ' || text[i]=='\n' || text[i]=='\r'){
            result.push_back(stof(part));
            part = "";
        }else{
            part+=(text[i]);
        }

    }

    return result;

}

vector<int> intVectorFromText(string text){
    vector<int> result;
    string part;

    for(int i=0; i< text.size(); i++){

        if(text[i]==' ' || text[i]=='\n' || text[i]=='\r'){
            result.push_back(stof(part));
            part = "";
        }else{
            part+=(text[i]);
        }

    }

    return result;

}

//Teste de leitura da instância
void readFile(string path){
    ifstream input(path);
    string textAux;
    int numFactories;
    int numMachines;
    int numJobs;
    vector<vector<int>> processingTimes;
    vector<float> speedModes;
    vector<int> vectorFromFile;

    getline(input, textAux);
    numFactories = stoi(textAux);

    getline(input, textAux);
    numMachines = stoi(textAux);

    getline(input, textAux);
    numJobs = stoi(textAux);
    getline(input, textAux);

    speedModes = floatVectorFromText(textAux);
    getline(input, textAux);
    input.close();

    vectorFromFile = intVectorFromText(textAux);
    processingTimes.reserve(numMachines);

    for(int i=0; i< numMachines;i++){
        vector<int> line(numJobs);
        for(int j=0; j< numJobs; j++){
            line[j] = vectorFromFile[i*numJobs + j];
        }
        processingTimes.push_back(line);
    }

    Matrices instance = Matrices(numFactories, numJobs, numMachines, processingTimes);


}