#include "read.hpp"
#include <iostream>


using namespace std;

vector<float> vectorFromText(string text){
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

//Teste de leitura da instância
void readFile(string path){
    ifstream input(path);
    string text;
    int numFactories;
    int numMachines;
    int numJobs;
    vector<vector<float>> processingTimes;
    vector<float> speedModes;
    vector<float> vectorFromFile;

    getline(input, text);
    numFactories = stoi(text);

    getline(input, text);
    numMachines = stoi(text);

    getline(input, text);
    numJobs = stoi(text);
    getline(input, text);

    speedModes = vectorFromText(text);
    getline(input, text);

    vectorFromFile = vectorFromText(text);
    processingTimes.reserve(numMachines);

    for(int i=0; i< numMachines;i++){
        vector<float> line(numJobs);
        for(int j=0; j< numJobs; j++){
            line[j] = vectorFromFile[i*numJobs + j];
        }
        processingTimes.push_back(line);
    }

    cout<<numFactories<<" "<<numMachines<<" "<< numJobs<<endl;

    input.close();

}