#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>

using namespace std;


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
void printAlgorithmName(int algType);

int runPR_withPREEMPSchedulingAlg(int numOfProcesses, int maxNumOfProcesses, const processInfo *process);

ofstream outf("output.txt");


int main() {

    int algType;
    int numOfProcesses = 0;
    int maxNumOfProcesses;
    struct processInfo *process;
    int const timeQuantam = 4;
    int avg_time = 0;

    int ALG_TYPE[4];


      if(!outf){
          cerr << "Uh oh, output.txt could not be opened for writing!" << endl;
          cout << "error";
          exit(1);
      }


    ifstream inf("input.txt");
    if(!inf){
        cerr <<"input.txt cound not be opened for reading!"<< endl;
        exit(1);
    }


        string strInput;
        stringstream sch_algorithms;
        getline(inf, strInput);
        sch_algorithms << strInput;

        int temp = 0;
        while(sch_algorithms >> strInput){
            if(strInput.compare("SJF") == 0){
                ALG_TYPE[temp] = ::SJF;
            } else if (strInput.compare("RR") == 0){
                ALG_TYPE[temp] = ::RR;
            } else if (strInput.compare("PR_noPREMP") == 0){
                ALG_TYPE[temp] = ::PR_noPREMP;
            } else if(strInput.compare("PR_withPREMP") == 0){
                ALG_TYPE[temp] = ::PR_withPREMP;
            }
            temp++;
        }


    stringstream strStream;



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

    for(int i = 0; i < 4; i++){

        processInfo *copyOfTheOriginalProcesses = new processInfo [maxNumOfProcesses];
        for(int i = 0; i < maxNumOfProcesses; i++){
            copyOfTheOriginalProcesses[i].number        = process[i].number;
            copyOfTheOriginalProcesses[i].timeArrive    = process[i].timeArrive;
            copyOfTheOriginalProcesses[i].cpuBurst      = process[i].cpuBurst;
            copyOfTheOriginalProcesses[i].priority      = process[i].priority;
        }


        printAlgorithmName(ALG_TYPE[i]);
        if(ALG_TYPE[i] == ::SJF){
        runSjfScheduler(numOfProcesses, maxNumOfProcesses, copyOfTheOriginalProcesses);}
        else if(ALG_TYPE[i] == ::RR) {
            runRRScheduler(numOfProcesses, maxNumOfProcesses, copyOfTheOriginalProcesses, timeQuantam); }
        else if (ALG_TYPE[i] == ::PR_noPREMP){
            runPRNoPreempScheduler(numOfProcesses, maxNumOfProcesses, copyOfTheOriginalProcesses);
        } else {
            numOfProcesses = runPR_withPREEMPSchedulingAlg(numOfProcesses, maxNumOfProcesses, process);
        }
    }


    return 0;
}

int runPR_withPREEMPSchedulingAlg(int numOfProcesses, int maxNumOfProcesses, const processInfo *process) {
    numOfProcesses = 0;
    int curTime = 0;
    int total_time = 0;
    int mum_of_iter = 0;

    int prev_process = 0;
    int current_process;
    int *process_wait_time = new int[maxNumOfProcesses+2];
    for(int i = 0; i < maxNumOfProcesses+2; i++){
                process_wait_time[i]  = 0;
            }

    // cout << "Max number of processes is: " << maxNumOfProcesses << endl;
    priority_queue<processInfo*,vector<processInfo*>,function
                    <bool(const processInfo*,const processInfo*)>>
                    prQueuePR([](const processInfo* s1, const processInfo* s2)
                              {return s1->priority > s2->priority;});
//TODO: When does it get preempted, every time or only if a next process' priority is higher?
    while(numOfProcesses < maxNumOfProcesses){
                prQueuePR.push((processInfo*)(&process[numOfProcesses]));
                current_process = ((processInfo*)prQueuePR.top())->number;
                if(numOfProcesses != maxNumOfProcesses - 1) {    // Don't compare the last element
                    processInfo *info = prQueuePR.top();

                    int timeOffset = curTime + info->cpuBurst;
                    if(timeOffset < process[numOfProcesses+1].timeArrive){ // <= ???
                        //cout << curTime << " " << info->number << endl;
                        outf << "\t" << curTime << " " << info->number << endl;

                        if(prev_process != current_process) {
                            total_time += curTime - process_wait_time[current_process];
                        }

                        curTime += info->cpuBurst; //total_time += curTime;
                        prQueuePR.pop();
                        mum_of_iter++;

                        if(prev_process != current_process) {
                            process_wait_time[current_process] =  curTime;
                            prev_process = current_process;
                        }

                    }else {
                        processInfo *info = prQueuePR.top();
                        info->cpuBurst -= process[numOfProcesses + 1].timeArrive - curTime;
                        //cout << curTime << " " << info->number << endl;
                        outf << "\t" << curTime << " " << info->number << endl;

                        curTime += process[numOfProcesses + 1].timeArrive - curTime;
                        mum_of_iter++;

                    }
                }
                numOfProcesses++;
            }
    while(!prQueuePR.empty()){
                processInfo *info = prQueuePR.top();    prQueuePR.pop();
                //cout << curTime << " " << info->number << endl;
                outf << "\t" << curTime << " " << info->number << endl;
                if(prev_process != current_process) {
                    total_time += curTime - process_wait_time[current_process];
                }

                curTime += info->cpuBurst;
                mum_of_iter++;

                if(prev_process != current_process) {
                    process_wait_time[current_process] =  curTime;
                    prev_process = current_process;
                }
            }

    //cout << "AVG Waiting Time: " << (float)total_time / maxNumOfProcesses << endl << endl << endl;
    outf << "AVG Waiting Time: " << (float)total_time / maxNumOfProcesses << endl << endl << endl;
    return numOfProcesses;
}

void printAlgorithmName(int algType) {
             if(algType == SJF){
                outf << "SJF" << endl;
            } else if (algType == RR){
                outf << "RR" << endl;
            } else if (algType == PR_noPREMP){
                outf << "PR_noPREMP" << endl;
            } else if (algType == PR_withPREMP){
                outf << "PR_withPREMP" << endl;
            }
}

void runPRNoPreempScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process) {
    numOfProcesses = 0;
    int curTime = 0;
    int total_time = 0;
   // cout << "Max number of processes is: " << maxNumOfProcesses << endl;
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
                        //cout << curTime << " " << info->number << endl;
                        total_time += curTime;
                        outf << "\t" << curTime << " " << info->number << endl;
                        curTime += info->cpuBurst;
                    }
                }
                numOfProcesses++;
            }
    while(!prQueuePR.empty()){
                processInfo *info = prQueuePR.top();    prQueuePR.pop();
                //cout << curTime << " " << info->number << endl;
                 total_time += curTime;
                outf << "\t"   << curTime << " " << info->number << endl;
                curTime += info->cpuBurst;
            }
    //cout << "AVG Waiting Time: " << (float)total_time /  numOfProcesses << endl << endl << endl;
    outf << "AVG Waiting Time: " << (float)total_time /  numOfProcesses << endl << endl << endl;
}

void runRRScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process, const int timeQuantam) {
    numOfProcesses = 0;
    int curTime = 0;
    int total_time = 0;
    int num_of_iter = 0;
    int prev_process = 0;
    int current_process;
    int *process_wait_time = new int[maxNumOfProcesses+2];
    for(int i = 0; i < maxNumOfProcesses+2; i++){
        process_wait_time[i]  = 0;
    }


   // cout << "Max number of processes is: " << maxNumOfProcesses << endl;
    queue<processInfo*> regQueueRR;
    while(numOfProcesses < maxNumOfProcesses){
                regQueueRR.push((processInfo*)(&process[numOfProcesses]));
                numOfProcesses++;
            }
    while(!regQueueRR.empty()){
                int processBurst = ((processInfo*)regQueueRR.front())->cpuBurst;
                current_process = ((processInfo*)regQueueRR.front())->number;

                if(processBurst > timeQuantam){
                    //do a swap
                    processInfo *info = regQueueRR.front();
                    info->cpuBurst -= timeQuantam;
                    //cout << curTime << " " << info->number << endl;
                    outf << "\t"  << curTime << " " << info->number << endl;
                    //total_time += curTime;

                    if(prev_process != current_process) {
                        total_time += curTime - process_wait_time[current_process];
                        num_of_iter++;
                    }

                    regQueueRR.push(info);
                    regQueueRR.pop();
                    curTime += timeQuantam;

                    if(prev_process != current_process) {
                        process_wait_time[current_process] =  curTime;
                        num_of_iter++;
                        prev_process = current_process;
                    }
                } else {
                    processInfo *info = regQueueRR.front();
                    //cout << curTime << " " << info->number << endl;
                    outf << "\t"  << curTime << " " << info->number << endl;

                    if(prev_process != current_process) {
                        total_time += curTime - process_wait_time[current_process];
                        num_of_iter++;

                    }

                    curTime += info->cpuBurst;
                    regQueueRR.pop();

                    if(prev_process != current_process) {
                        process_wait_time[current_process] =  curTime;
                        num_of_iter++;
                        prev_process = current_process;
                    }



                }
            }

    for(int i = 0; i < numOfProcesses; i++){
       // total_time += process_wait_time[i];
    }

    //cout << "AVG Waiting Time: " << (float)total_time / maxNumOfProcesses << endl << endl << endl;
    outf << "AVG Waiting Time: " << (float)total_time / maxNumOfProcesses << endl << endl << endl;
}

void runSjfScheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process) {
    numOfProcesses = 0;
    int curTime = 0;
    int total_time = 0;


   // cout << "Max number of processes is: " << maxNumOfProcesses << endl;
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
                    //cout << curTime << " " << info->number << endl;
                    outf << "\t"   << curTime << " " << info->number << endl;
                    curTime += info->cpuBurst;

                }
            }
            numOfProcesses++;
        }
    while(!prQueueSJF.empty()){
            processInfo *info = prQueueSJF.top();    prQueueSJF.pop();
            //cout << curTime << " " << info->number << endl;
            total_time += curTime;
            outf << "\t"  << curTime << " " << info->number << endl;
            curTime += info->cpuBurst;

        }

    //cout << "AVG Waiting Time: " << (float)total_time /  numOfProcesses << endl << endl << endl;
    outf << "AVG Waiting Time: " << (float)total_time /  maxNumOfProcesses << endl << endl << endl;


}


