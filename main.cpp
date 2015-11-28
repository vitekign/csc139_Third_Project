#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>

using namespace std;

//TODO: Don't forget to add AVG Waiting Time: ______

typedef enum {
    SJF,
    RR,
    PR_noPREMP,
    PR_withPREMP

} SCH_ALGORITHM;

 struct processInfo{
    int number;
    int timeArrive;
    int cpuBurst;
    int priority;
};
void runSjfScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process);
void runRRScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process, const int timeQuantam);

void runPRNoPreempScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process);

int main() {

    int algType;
    int numOfProcesses = 0;
    int maxNumOfProcesses;
    struct processInfo *process;
    int const timeQuantam = 4;


    ifstream inf("input.txt");
    if(!inf){
        cerr <<"input.txt cound not be opened for reading!"<< endl;
        exit(1);
    }
    string strInput;
    while( getline(inf, strInput)){

        stringstream strStream;
        if(strInput.compare("SJF") == 0){
            algType = ::SJF;
        } else if (strInput.compare("RR") == 0){
            algType = ::RR;
        } else if (strInput.compare("PR_noPREMP") == 0){
            algType = ::PR_noPREMP;
        } else if(strInput.compare("PR_withPREMP") == 0){
            algType = ::PR_withPREMP;
        }



        cout << "Algorithm type is: " << algType << endl;
        getline(inf, strInput);
        strStream << strInput;
        strStream >> maxNumOfProcesses;
        cout << "The number of processes is: " << maxNumOfProcesses << endl;


       process = (processInfo*)calloc(maxNumOfProcesses, sizeof(processInfo));
       strStream.str("");

        stringstream prosStream;

        numOfProcesses = 0;

        while(numOfProcesses < maxNumOfProcesses){
            getline(inf, strInput);
            prosStream << strInput;
            string first_value;

            int temp = 0;
            while(prosStream >> first_value) {
                cout << first_value << " ";
                if(temp == 0){
                   process[numOfProcesses].number = stoi(first_value);
                } else if(temp == 1){
                    process[numOfProcesses].timeArrive = stoi(first_value);
                } else if(temp == 2){
                    process[numOfProcesses].cpuBurst = stoi(first_value);
                } else if(temp == 3){
                    process[numOfProcesses].priority = stoi(first_value);
                }
                temp++;
            }
            numOfProcesses++;
            prosStream.clear();
            cout << endl;
         }

        if(algType == ::SJF){
        runSjfScheduler(numOfProcesses, maxNumOfProcesses, process);}
        else if(algType == ::RR) {
            runRRScheduler(numOfProcesses, maxNumOfProcesses, process, timeQuantam); }
        else if (algType == ::PR_noPREMP){
            runPRNoPreempScheduler(numOfProcesses, maxNumOfProcesses, process);
        } else {

            numOfProcesses = 0;
            int curTime = 0;
            cout << "Max number of processes is: " << maxNumOfProcesses << endl;
            priority_queue<processInfo*,vector<processInfo*>,function
                    <bool(const processInfo*,const processInfo*)>>
                    prQueuePR([](const processInfo* s1, const processInfo* s2)
                              {return s1->priority >s2->priority;});

            while(numOfProcesses < maxNumOfProcesses){
                prQueuePR.push((processInfo*)(&process[numOfProcesses]));
                if(numOfProcesses != maxNumOfProcesses - 1) {    // Don't compare the last element
                    int pivot = curTime + process[numOfProcesses].cpuBurst;
                    if (process[numOfProcesses + 1].timeArrive > pivot) {
                        processInfo *info = prQueuePR.top();
                        prQueuePR.pop();
                        cout << curTime << " " << info->number << endl;
                        curTime += info->cpuBurst;
                    } else {
                        processInfo *info = prQueuePR.top();
                        info->cpuBurst - process[numOfProcesses + 1].timeArrive;
                        cout << curTime << " " << info->number << endl;
                        curTime += process[numOfProcesses + 1].timeArrive;
                    }
                }
                numOfProcesses++;
            }
            while(!prQueuePR.empty()){
                processInfo *info = prQueuePR.top();    prQueuePR.pop();
                cout << curTime << " " << info->number << endl;
                curTime += info->cpuBurst;
            }


        }


        delete [] process;
    }






    /**
     * File manipulation

    ofstream outf("output.txt");
    if(!outf){
        cerr << "Uh oh, output.txt could not be opened for writing!" << endl;
        cout << "error";
        exit(1);
    }

    // Write two lines into the file
    outf << "This is line 1" << endl;
    outf << "This is line 2" << endl;

  */
    /**
     * ifstream returns a 0 if we’ve reached the end of the file (EOF)
     */
    /*


  -------------4----------------
  PR_withPREMP
  4
  1     0      8       3
  2     3      1       1

  3     5      2       4
  4     6      2       2

  curTime = 0;
  1     0      8       3
  Give only 3 seconds to the first process by looking in the time queue
  Add the second process after three seconds to the main queue and change
  the first process

  curTime = 3;
  2     3      1       1

  curTime + 1 = 4;

  curTime = 4;
  1     0      5       3
  4 + 5 = 9
  5 - 4 = 1;
  curTime += 1

*/


    return 0;
}

void runPRNoPreempScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process) {
    numOfProcesses = 0;
    int curTime = 0;
    cout << "Max number of processes is: " << maxNumOfProcesses << endl;
    priority_queue<processInfo*,vector<processInfo*>,function
                    <bool(const processInfo*,const processInfo*)>>
                    prQueuePR([](const processInfo* s1, const processInfo* s2)
                               {return s1->priority >s2->priority;});

    while(numOfProcesses < maxNumOfProcesses){
                prQueuePR.push((processInfo*)(&process[numOfProcesses]));
                if(numOfProcesses != maxNumOfProcesses - 1) {    // Don't compare the last element
                    if (process[numOfProcesses + 1].timeArrive > curTime) {
                        processInfo *info = prQueuePR.top();
                        prQueuePR.pop();
                        cout << curTime << " " << info->number << endl;
                        curTime += info->cpuBurst;
                    }
                }
                numOfProcesses++;
            }
    while(!prQueuePR.empty()){
                processInfo *info = prQueuePR.top();    prQueuePR.pop();
                cout << curTime << " " << info->number << endl;
                curTime += info->cpuBurst;
            }
}

void runRRScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process, const int timeQuantam) {
    numOfProcesses = 0;
    int curTime = 0;
    cout << "Max number of processes is: " << maxNumOfProcesses << endl;
    queue<processInfo*> regQueueRR;
    while(numOfProcesses < maxNumOfProcesses){
                regQueueRR.push((processInfo*)(&process[numOfProcesses]));
                numOfProcesses++;
            }
    while(!regQueueRR.empty()){
                int processBurst = ((processInfo*)regQueueRR.front())->cpuBurst;
                if(processBurst > timeQuantam){
                    //do a swap
                    processInfo *info = regQueueRR.front();
                    info->cpuBurst -= timeQuantam;
                    cout << curTime << " " << info->number << endl;
                    regQueueRR.push(info);
                    regQueueRR.pop();
                    curTime += timeQuantam;
                } else {
                    processInfo *info = regQueueRR.front();
                    cout << curTime << " " << info->number << endl;
                    curTime += info->cpuBurst;
                    regQueueRR.pop();

                }
            }
}

void runSjfScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process) {
    numOfProcesses = 0;
    int curTime = 0;
    cout << "Max number of processes is: " << maxNumOfProcesses << endl;
    priority_queue<processInfo*,vector<processInfo*>,function
                <bool(const processInfo*,const processInfo*)>>
                prQueueSJF([](const processInfo* s1, const processInfo* s2)
                   {return s1->cpuBurst>s2->cpuBurst;});


    while(numOfProcesses < maxNumOfProcesses){
            prQueueSJF.push((processInfo*)(&process[numOfProcesses]));
            if(numOfProcesses != maxNumOfProcesses - 1) {    // Don't compare the last element
                if (process[numOfProcesses + 1].timeArrive > curTime) {
                    processInfo *info = prQueueSJF.top();
                    prQueueSJF.pop();
                    cout << curTime << " " << info->number << endl;
                    curTime += info->cpuBurst;
                }
            }
            numOfProcesses++;
        }
    while(!prQueueSJF.empty()){
            processInfo *info = prQueueSJF.top();    prQueueSJF.pop();
            cout << curTime << " " << info->number << endl;
            curTime += info->cpuBurst;
        }
}