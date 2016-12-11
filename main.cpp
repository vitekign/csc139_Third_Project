#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>

using namespace std;

// Process number           Arrival Time           CPU burst time            Priority

typedef enum {
    SJF,            //SHORTEST JOB FIRST
    RR,             //ROUND ROBIN
    PR_NO_PREEMP,     //PRIORITY SCHEDULING | NO PREEMPTION
    PR_WITH_PREEMP    //PRIORITY SCHEDULING | PREEMPTION
} SCH_ALGORITHM;

struct processInfo {
    int number;
    int timeArrive;
    int cpuBurst;
    int priority;
};

void run_SJF_scheduler(int seq_num_of_curr_process, int num_of_processes, const processInfo *process);
void run_RR_scheduler(int num_of_processes, int max_num_of_processes, const processInfo *process, const int time_quantam);
void run_PR_NO_PREEM_scheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process);
void printAlgorithmName(int algType);
int run_PR_WITH_PREEM_schedulingAlg(int numOfProcesses, int maxNumOfProcesses, const processInfo *process);
ofstream outf("output.txt");

int main() {

    int numOfProcesses = 0;
    int maxNumOfProcesses;
    struct processInfo *process;
    int const timeQuantam = 4;

    int ALG_TYPE[4];

    if (!outf) {
        cerr << "output.txt could not be opened for writing!" << endl;
        cout << "error";
        exit(1);
    }

    ifstream inf("input.txt");
    if (!inf) {
        cerr << "input.txt cound not be opened for reading!" << endl;
        exit(1);
    }

    string strInput;
    stringstream sch_algorithms;
    getline(inf, strInput);
    sch_algorithms << strInput;

    int temp = 0;
    while (sch_algorithms >> strInput) {
        if (strInput.compare("SJF") == 0) {
            ALG_TYPE[temp] = ::SJF;
        } else if (strInput.compare("RR") == 0) {
            ALG_TYPE[temp] = ::RR;
        } else if (strInput.compare("PR_NO_PREEMP") == 0) {
            ALG_TYPE[temp] = ::PR_NO_PREEMP;
        } else if (strInput.compare("PR_WITH_PREEMP") == 0) {
            ALG_TYPE[temp] = ::PR_WITH_PREEMP;
        }
        temp++;
    }

    stringstream strStream;

    getline(inf, strInput);
    strStream << strInput;
    strStream >> maxNumOfProcesses;
    cout << "The number of processes is: " << maxNumOfProcesses << endl;


    process = (processInfo *) calloc(maxNumOfProcesses, sizeof(processInfo));
    strStream.str("");

    stringstream prosStream;

    numOfProcesses = 0;

    while (numOfProcesses < maxNumOfProcesses) {
        getline(inf, strInput);
        prosStream << strInput;
        string first_value;

        int temp = 0;
        while (prosStream >> first_value) {
            cout << first_value << " ";
            if (temp == 0) {
                process[numOfProcesses].number = stoi(first_value);
            } else if (temp == 1) {
                process[numOfProcesses].timeArrive = stoi(first_value);
            } else if (temp == 2) {
                process[numOfProcesses].cpuBurst = stoi(first_value);
            } else if (temp == 3) {
                process[numOfProcesses].priority = stoi(first_value);
            }
            temp++;
        }
        numOfProcesses++;
        prosStream.clear();
        cout << endl;
    }

    for (int i = 0; i < 4; i++) {

        processInfo *copyOfTheOriginalProcesses = new processInfo[maxNumOfProcesses];
        for (int i = 0; i < maxNumOfProcesses; i++) {
            copyOfTheOriginalProcesses[i].number = process[i].number;
            copyOfTheOriginalProcesses[i].timeArrive = process[i].timeArrive;
            copyOfTheOriginalProcesses[i].cpuBurst = process[i].cpuBurst;
            copyOfTheOriginalProcesses[i].priority = process[i].priority;
        }


        printAlgorithmName(ALG_TYPE[i]);
        if (ALG_TYPE[i] == ::SJF) {
            run_SJF_scheduler(numOfProcesses, maxNumOfProcesses, copyOfTheOriginalProcesses);
        } else if (ALG_TYPE[i] == ::RR) {
            run_RR_scheduler(numOfProcesses, maxNumOfProcesses, copyOfTheOriginalProcesses, timeQuantam);
        } else if (ALG_TYPE[i] == ::PR_NO_PREEMP) {
            run_PR_NO_PREEM_scheduler(numOfProcesses, maxNumOfProcesses, copyOfTheOriginalProcesses);
        } else {
            numOfProcesses = run_PR_WITH_PREEM_schedulingAlg(numOfProcesses, maxNumOfProcesses, process);
        }
    }


    return 0;
}

int run_PR_WITH_PREEM_schedulingAlg(int numOfProcesses, int maxNumOfProcesses, const processInfo *process) {
    numOfProcesses = 0;
    int curTime = 0;
    int total_time = 0;
    int mum_of_iter = 0;

    int prev_process = 0;
    int current_process;
    int *process_wait_time = new int[maxNumOfProcesses + 2];
    for (int i = 0; i < maxNumOfProcesses + 2; i++) {
        process_wait_time[i] = 0;
    }

    // cout << "Max number of processes is: " << maxNumOfProcesses << endl;
    priority_queue<processInfo *, vector<processInfo *>, function
            <bool(const processInfo *, const processInfo *)>>
            prQueuePR([](const processInfo *s1, const processInfo *s2) { return s1->priority > s2->priority; });
//TODO: When does it get preempted, every time or only if a next process' priority is higher?
    while (numOfProcesses < maxNumOfProcesses) {
        prQueuePR.push((processInfo *) (&process[numOfProcesses]));
        current_process = ((processInfo *) prQueuePR.top())->number;
        if (numOfProcesses != maxNumOfProcesses - 1) {    // Don't compare the last element
            processInfo *info = prQueuePR.top();

            int timeOffset = curTime + info->cpuBurst;
            if (timeOffset < process[numOfProcesses + 1].timeArrive) { // <= ???
                //cout << curTime << " " << info->number << endl;
                outf << "\t" << curTime << " " << info->number << endl;

                if (prev_process != current_process) {
                    total_time += curTime - process_wait_time[current_process];
                }

                curTime += info->cpuBurst; //total_time += curTime;
                prQueuePR.pop();
                mum_of_iter++;


                if (prev_process != current_process) {
                    process_wait_time[current_process] = curTime;
                    prev_process = current_process;
                }

            } else {
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
    while (!prQueuePR.empty()) {
        processInfo *info = prQueuePR.top();
        prQueuePR.pop();
        //cout << curTime << " " << info->number << endl;
        outf << "\t" << curTime << " " << info->number << endl;
        if (prev_process != current_process) {
            total_time += curTime - process_wait_time[current_process];
        }

        curTime += info->cpuBurst;
        mum_of_iter++;

        if (prev_process != current_process) {
            process_wait_time[current_process] = curTime;
            prev_process = current_process;
        }
    }

    //cout << "AVG Waiting Time: " << (float)total_time / maxNumOfProcesses << endl << endl << endl;
    outf << "AVG Waiting Time: " << (float) total_time / maxNumOfProcesses << endl << endl << endl;
    return numOfProcesses;
}

void printAlgorithmName(int algType) {
    if (algType == SJF) {
        outf << "SJF" << endl;
    } else if (algType == RR) {
        outf << "* * * * * * * * * * * * * *" << endl;
        outf << "*       Round Robin       * " << endl;
        outf << "* * * * * * * * * * * * * *" << endl << endl;
    } else if (algType == PR_NO_PREEMP) {
        outf << "PR_NO_PREEMP" << endl;
    } else if (algType == PR_WITH_PREEMP) {
        outf << "PR_WITH_PREEMP" << endl;
    }
}

void run_PR_NO_PREEM_scheduler(int numOfProcesses, int maxNumOfProcesses, const processInfo *process) {
    numOfProcesses = 0;
    int curTime = 0;
    int total_time = 0;
    // cout << "Max number of processes is: " << maxNumOfProcesses << endl;
    priority_queue<processInfo *, vector<processInfo *>, function
            <bool(const processInfo *, const processInfo *)>>
            prQueuePR([](const processInfo *s1, const processInfo *s2) { return s1->priority > s2->priority; });

    while (numOfProcesses < maxNumOfProcesses) {
        prQueuePR.push((processInfo *) (&process[numOfProcesses]));
        if (numOfProcesses != maxNumOfProcesses - 1) {    // Don't compare the last element
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
    while (!prQueuePR.empty()) {
        processInfo *info = prQueuePR.top();
        prQueuePR.pop();
        //cout << curTime << " " << info->number << endl;
        total_time += curTime;
        outf << "\t" << curTime << " " << info->number << endl;
        curTime += info->cpuBurst;
    }
    //cout << "AVG Waiting Time: " << (float)total_time /  numOfProcesses << endl << endl << endl;
    outf << "AVG Waiting Time: " << (float) total_time / numOfProcesses << endl << endl << endl;
}

// Process number           Arrival Time           CPU burst time            Priority
void run_RR_scheduler(int num_of_processes, int max_num_of_processes, const processInfo *process, const int time_quantam) {
    num_of_processes = 0;
    int curr_time = 0;
    int total_time_waiting_time = 0;
    int prev_process_seq_number = 0;
    int curr_process_seq_number;
    int *time_it_finished_last_time = new int[max_num_of_processes + 2];
    for (int i = 0; i < max_num_of_processes + 2; i++) {
        time_it_finished_last_time[i] = 0;
    }

    queue<processInfo *> rr_queue;
    while (num_of_processes < max_num_of_processes) {
        rr_queue.push((processInfo *) (&process[num_of_processes]));
        num_of_processes++;
    }
    while (!rr_queue.empty()) {
        int curr_process_BURST = ((processInfo *) rr_queue.front())->cpuBurst;
        curr_process_seq_number = ((processInfo *) rr_queue.front())->number;

        if (curr_process_BURST > time_quantam) {
            processInfo *curr_proc = rr_queue.front();
            curr_proc->cpuBurst -= time_quantam;
            outf << "\t" << curr_proc->number  << " : " <<  curr_time<< endl;

            if (prev_process_seq_number!= curr_process_seq_number) {
                total_time_waiting_time += curr_time - time_it_finished_last_time[curr_process_seq_number];
            }

            rr_queue.push(curr_proc);
            rr_queue.pop();
            curr_time += time_quantam;

            if (prev_process_seq_number != curr_process_seq_number) {
                time_it_finished_last_time[curr_process_seq_number] = curr_time;
                prev_process_seq_number = curr_process_seq_number;
            }
        } else { //If Current Process' Burst is LESS OR EQUAL to the TIME QUANTUM
            processInfo *curr_proc = rr_queue.front();
            outf << "\t" << curr_proc->number << " : " <<  curr_time<< endl;

            if (prev_process_seq_number != curr_process_seq_number) {
                total_time_waiting_time += curr_time - time_it_finished_last_time[curr_process_seq_number];
            }

            curr_time += curr_proc->cpuBurst;
            rr_queue.pop();

            if (prev_process_seq_number != curr_process_seq_number) {
                time_it_finished_last_time[curr_process_seq_number] = curr_time; //looks like this line is redundant
                prev_process_seq_number = curr_process_seq_number;
            }
        }
    }

    outf << endl <<  "Average waiting time: " << (float) total_time_waiting_time / max_num_of_processes << endl << endl << endl;
}

// Process number           Arrival Time           CPU burst time            Priority
void run_SJF_scheduler(int seq_num_of_curr_process, int num_of_processes, const processInfo *process) {
    seq_num_of_curr_process = 0;
    int curr_time = 0;
    int total_time = 0;

    priority_queue<processInfo *, //Define an element
            vector<processInfo *>, //For Internal Use
            function<bool(const processInfo *, const processInfo *)>> //Comparator
            prior_queue([](const processInfo *s1, const processInfo *s2) { return s1->cpuBurst > s2->cpuBurst; });


    while (seq_num_of_curr_process < num_of_processes) {
        prior_queue.push((processInfo *) (&process[seq_num_of_curr_process]));
            if (process[seq_num_of_curr_process + 1].timeArrive > curr_time) {
                processInfo *info = prior_queue.top();
                prior_queue.pop();
                outf << "\t" << curr_time << " " << info->number << endl;
                curr_time += info->cpuBurst;
        }
        seq_num_of_curr_process++;
    }
    while (!prior_queue.empty()) {
        processInfo *info = prior_queue.top();
        prior_queue.pop();
        //cout << curr_time << " " << info->number << endl;
        total_time += curr_time;
        outf << "\t" << curr_time << " " << info->number << endl;
        curr_time += info->cpuBurst;

    }

    outf << "Average waiting time: " << (float) total_time / num_of_processes << endl << endl << endl;
}