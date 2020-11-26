#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <vector>

using namespace std;

#define ARRAY_SIZE 100000

vector < vector < string > > everything;
vector < vector < string > > ANS;
vector <string> v;
vector <string> vIndex;
vector <string> firstLine;
string by;

vector<string> removeDupWord(char str[]) 
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

int findIndex() {
    for(int i = 0; i < firstLine.size(); i++) {
        if(i == 0) {
            if(firstLine[i] == by) {
                return i;
            }
        }
        else {
            if(firstLine[i] == " "+by) {
                return i;
            }
        }
    }
    return -1;
}

void swap(int min_idx, int i)  
{
    vector <string> temp = ANS[min_idx];  
    ANS[min_idx] = ANS[i];  
    ANS[i] = temp;  
    string temp2 = vIndex[min_idx];  
    vIndex[min_idx] = vIndex[i];  
    vIndex[i] = temp2;  
}

void selectionSortDescending()  
{ 
    int index = findIndex();
    // cout << index << endl;
    int n = ANS.size();
    int i, j, min_idx;  
    for (i = n-1; i >= 0; i--)  
    {
        min_idx = i;  
        for (j = 0; j <= i; j++) {
            if (ANS[j][index] < ANS[min_idx][index]) {
                min_idx = j;
            }
        }
        swap(min_idx, i);  
    }
}


vector<string> tokenizeMsg(string in){
    vector <string> result;
    string temp = "";
    for(int i = 0; i < in.size(); i++) {
        if(in[i] == '^') {
            result.push_back(temp);
            temp = "";
        }
        else {
            temp += in[i];
        }
    }
    result.push_back(temp);
    return result; 
}

int before_(string in){
    string temp = "";
    for(int i = 0; i < in.size(); i++) {
        if(in[i] == '_') {
            break;
        }
        else {
            temp += in[i];
        }
    }
    return stoi(temp); 
}

int after_(string in){
    string temp = "";
    int flag = 0;
    for(int i = 0; i < in.size(); i++) {
        if(in[i] == '_') {
            flag = 1;
        }
        else if(flag == 1) {
            temp += in[i];
        }
    }
    return stoi(temp); 
}

int main(int argc, char const *argv[]){
    string parentNamedPipe = "./LoadBalancerNamedPipe";
    int processes = atoi(argv[1]);
    string sort = argv[2];
    string dir = argv[3];
    int fileCount = atoi(argv[4]);
    by = argv[5];
    for(int i = 1; i < fileCount; i++) {
        string datasetAddress = dir + "/dataset"  + to_string(i).c_str();
        ifstream datasetFile;
        datasetFile.open(datasetAddress);
        string line;
        vector <string> data;
        data.clear();
        while (datasetFile.good())
        {
            getline (datasetFile,line);
            int n = line.length(); 
            char char_array[n + 1]; 
            strcpy(char_array, line.c_str()); 
            data.push_back(line);
        }
        int n = data[0].length(); 
        char char_array[n + 1]; 
        strcpy(char_array, data[0].c_str()); 
        firstLine = removeDupWord(char_array);
        everything.push_back(data);
    }
    for(int i = 0; i < processes; i++){
        string namedPipeAddress = "./unnamedPipe_" + to_string(i);
        int fileDes = open(namedPipeAddress.c_str(), O_RDONLY);
        char msg[ARRAY_SIZE];
        read(fileDes, msg, ARRAY_SIZE);
        vector<string> li = tokenizeMsg(msg);
        for(int j = 0; j < li.size(); j++) {
            if(li[j] == "") {
                continue;
            }
            vIndex.push_back(li[j]);
            string s = everything[before_(li[j]) - 1][after_(li[j])];
            v.push_back(s);
        }
    }
    for(int k = 0; k < v.size(); k++) {
        vector <string> vec;
        int n = v[k].length(); 
        char char_array[n + 1]; 
        strcpy(char_array, v[k].c_str()); 
        vec = removeDupWord(char_array);
        ANS.push_back(vec);
    }
    if(sort == "ascending") {
        selectionSortDescending();
        if(ANS.size() == 0) {
            cout << "Nothing\n";
        }
        for(int i = ANS.size()-1; i >= 0; i--) {
            for(int j = 0; j < ANS[i].size(); j++) {
                if(j+1 != ANS[i].size()) {
                    cout << ANS[i][j] << " -";
                }
                else {
                    cout << ANS[i][j];
                }
            }
            cout << endl;
        }
        cout << endl;
    }
    if(sort == "descending") {
        selectionSortDescending();
        if(ANS.size() == 0) {
            cout << "Nothing\n";
        }
        for(int i = 0; i < ANS.size(); i++) {
            for(int j = 0; j < ANS[i].size(); j++) {
                if(j+1 != ANS[i].size()) {
                    cout << ANS[i][j] << " -";
                }
                else {
                    cout << ANS[i][j];
                }
            }
            cout << endl;
        }
        cout << endl;
    }
    if(sort == "none") {
        if(ANS.size() == 0) {
            cout << "Nothing\n";
        }
        for(int i = 0; i < ANS.size(); i++) {
            for(int j = 0; j < ANS[i].size(); j++) {
                if(j+1 != ANS[i].size()) {
                    cout << ANS[i][j] << " -";
                }
                else {
                    cout << ANS[i][j];
                }
            }
            cout << endl;
        }
        cout << endl;
    }
    string sendThis = "";
    int fileDes = open(parentNamedPipe.c_str(), O_WRONLY);
    write(fileDes, sendThis.c_str(), (sendThis.length())+1);
    close(fileDes);
}