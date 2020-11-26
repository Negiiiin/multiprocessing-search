#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iomanip>

using namespace std;

#define ARRAY_SIZE 500

vector<string> removeDupWord(char str[]) 
{
    vector<string> words;
    char *token = strtok(str, " "); 
    while (token != NULL) 
    { 
        words.push_back(token);
        token = strtok(NULL, " "); 
    } 
    return words;
} 

int findNumberOfFiles(string dir){
    int fileCount = 1;
    bool stillExist = true;
    while(stillExist){
        string classifierAddress = dir + "/dataset"  + to_string(fileCount); 
        ifstream classifierFile;
        classifierFile.open(classifierAddress);
        if(classifierFile.is_open()){
            fileCount ++;
            classifierFile.close();
        }
        else{
            stillExist = false;
        }
    }
    return fileCount;
}


int main(int argc1, char const *argv1[]){
    while(true) {
        vector<string> argv;
        int argc;
        string input;
        getline(cin,input);
        if(input == "quit") {
            break;
        }
        char char_array[ARRAY_SIZE];
        strcpy(char_array, input.c_str()); 
        argv = removeDupWord(char_array);
        argc = argv.size();
        int index;
        string dir = argv[argc - 1];
        string sort = argv[argc - 9];
        int processes = atoi(argv[argc - 5].c_str());
        char s[ARRAY_SIZE];
        char b[ARRAY_SIZE];
        if(sort.compare("ascending") == 0 || sort.compare("descending") == 0) {
            index = argc - 11;
            strcpy(s, argv[argc - 9].c_str());
            strcpy(b, argv[argc - 11].c_str());
        }
        else {
            index = argc - 7;
            string str = "none";
            strcpy(s, str.c_str());
            strcpy(b, str.c_str());
        }
        string msgToSend = "";
        string in = "";
        for(int i = 0; i < index; i++) {
            in += argv[i];
            in += " ";
        }
        in.pop_back();
        string temp = "";
        int flag = 0;
        for(int i = 0; i < in.size(); i++) {
            if(in[i] == '=' || in[i] =='-') {
                temp.pop_back();
                msgToSend += temp;
                msgToSend += "&";
                temp = "";
                flag = 1;
                continue;
            }
            if(flag == 1) {
                flag = 0;
                continue;
            }
            else{
                temp += in[i];
            }
        }
        // NUMBER OF FILES
        int fileCount = findNumberOfFiles(dir);
        // MAKING NEEDED UNNAMED PIPES OF SYSTEM
        for(int i = 0; i < processes; i++){
            string namedPipeAddress = "./unnamedPipe_" + to_string(i);
            mkfifo(namedPipeAddress.c_str(), 0666);
        }  
        string parentNamedPipe = "./LoadBalancerNamedPipe";
        mkfifo(parentNamedPipe.c_str(), 0666);
        // FORK
        int unNamedPipe[processes][2];
        for(int i = 0; i < processes; i++){
            if(pipe(unNamedPipe[i]) < 0){
                cout<<"FAILED...creating pipe failed!\n";
                exit(1);
            }
            int forkStatus = fork();
            if(forkStatus < 0){
                cout << "FAILED...fork failed!\n";
                exit(1);
            }
            else if(forkStatus == 0){//child process
                close(unNamedPipe[i][1]);
                char readMsg[ARRAY_SIZE];
                read(unNamedPipe[i][0], readMsg, ARRAY_SIZE);
                close(unNamedPipe[i][0]);
                char num[ARRAY_SIZE];
                char directory[ARRAY_SIZE];
                strcpy(directory, argv[argc - 1].c_str());
                strcpy(num, to_string(fileCount).c_str());
                char num2[ARRAY_SIZE];
                strcpy(num2, to_string(i).c_str());
                char num3[ARRAY_SIZE];
                strcpy(num2, to_string(processes).c_str());
                strcpy(num3, to_string(i).c_str());
                char* arguments[] = {(char* )"./Filter", num, directory, readMsg, num2, num3, NULL};
                execv("./Filter", arguments);
            }
            else{//parent process
                write(unNamedPipe[i][1], msgToSend.c_str(), (msgToSend.length())+1);
                close(unNamedPipe[i][1]);
            }
        }
        // PRESENTER FORK
        int presenterStatus = fork();
        if(presenterStatus == 0){//Presenter process
            char num[ARRAY_SIZE];
            strcpy(num, to_string(processes).c_str());
            char num2[ARRAY_SIZE];
            strcpy(num2, to_string(fileCount).c_str());
            char directory[200];
            strcpy(directory, argv[argc - 1].c_str());
            char* arguments[] = {(char* )"./Presenter", num, s, directory, num2, b, NULL};
            execv("./Presenter", arguments); 
        }
        int fileDes = open(parentNamedPipe.c_str(), O_RDONLY);
        char msg[ARRAY_SIZE];
        read(fileDes, msg, ARRAY_SIZE);
        for(int i = 0; i < processes + 1; i++){
            wait(NULL);
        }
        for(int i = 0; i < fileCount; i++){
            unlink(("./unnamedPipe_" + to_string(i)).c_str());
        }
        unlink((char* )"./LoadBalancerNamedPipe");
    }
}