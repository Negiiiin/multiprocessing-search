#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

vector < vector<string> > lines;
vector <string> li;

vector<string> removeDash(char str[]) 
{
    vector<string> words;
    vector<string> line;
    char *token = strtok(str, "-"); 
    while (token != NULL) 
    { 
        words.push_back(token);
        token = strtok(NULL, "-"); 
    } 
    for(int i = 0; i < words.size(); i++) {
        words[i].pop_back();
        line.push_back(words[i]);
    }
    return line;
} 

vector <string> removeAnd(string in) {
    vector <string> result;
    string temp = "";
    for(int i = 0; i < in.size(); i++) {
        if(in[i] == '&') {
            result.push_back(temp);
            temp = "";
        }
        else {
            temp += in[i];
        }
    }
    return result;
}

vector<int> findIndex(string str) {
    vector<string> words;
    vector<string> line;
    vector<int> indexes;
    words = removeAnd(str);
    for(int i = 0; i < words.size(); i+=2) {
        for(int j = 0; j < lines[0].size(); j++) {
            string s = lines[0][j];
            if(s.compare(" " + words[i]) == 0) {
                indexes.push_back(j);
            }
            if(s.compare(words[i]) == 0) {
                indexes.push_back(j);
            }
        }
    }
    return indexes;
}

vector<string> findFilter(string str) {
    vector<string> words;
    vector<string> result;
    words = removeAnd(str);
    for(int i = 0; i < words.size(); i+=2) {
        result.push_back(words[i+1]);
    }
    return result;
}

int main(int argc, char const *argv[]){
    int fileCount = atoi(argv[1])-1;
    string dir = argv[2];
    string filter = argv[3];
    int processes = atoi(argv[4]);
    int currentProcess = atoi(argv[5]);
    int limit = fileCount / processes;
    if(currentProcess < ((fileCount%processes))) {
        limit++;
    }
    int addThis;
    if(currentProcess > (fileCount%processes)) {
        addThis = (fileCount%processes);
    }
    else {
        addThis = currentProcess;
    }
    for(int fileNumber = ((currentProcess*(fileCount/processes))+1+addThis); fileNumber < (limit+(currentProcess*(fileCount/processes))+1+addThis); fileNumber++) {
        vector<string> dataWithComma;
        string datasetAddress = dir + "/dataset"  + to_string(fileNumber).c_str();
        ifstream datasetFile;
        datasetFile.open(datasetAddress);
        string line;
        vector <string> data;
        lines.clear();
        while (datasetFile.good())
        {
            getline (datasetFile,line);
            int n = line.length(); 
            char char_array[n + 1]; 
            strcpy(char_array, line.c_str()); 
            lines.push_back(removeDash(char_array));
            data.push_back(line);
        }
        int n = filter.length(); 
        char char_array[n + 1]; 
        strcpy(char_array, filter.c_str()); 
        vector<string> filters = findFilter(filter);
        vector<int> indexes = findIndex(filter);
        for(int i = 1; i < lines.size()-1; i++) {
            int flag = 0;
            for(int j = 0; j < indexes.size(); j++) {
                string s = lines[i][indexes[j]];
                if(indexes[j] == 0) {
                    if(s != filters[j]) {
                    flag = 1;
                    break;
                    }
                }
                else {
                    if(s != " "+filters[j]) {
                    flag = 1;
                    break;
                    }
                }
            }
            if(flag == 0) {
                li.push_back(to_string(fileNumber) + "_" + to_string(i));
            }
        }
    }
    string result = "";
    for(int i = 0; i < li.size(); i++){
        result += "^" + li[i]; 
    }
    string unnamedPipeAddress = "./unnamedPipe_" + to_string(currentProcess);
    int fileDes = open(unnamedPipeAddress.c_str(), O_WRONLY);
    write(fileDes, result.c_str(), result.length()+1);
    close(fileDes);
}