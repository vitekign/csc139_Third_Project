#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>
#include <list>

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

void
run_RR_scheduler(int num_of_processes, int max_num_of_processes, const processInfo *process, const int time_quantam);

void run_PR_NO_PREEM_scheduler(int seq_num_of_curr_process, int num_of_processes, const processInfo *process);

void printAlgorithmName(int algType);

void run_PR_WITH_PREEM_schedulingAlg(int seq_num_of_curr_process, int num_of_processes, const processInfo *process);

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
            run_PR_WITH_PREEM_schedulingAlg(numOfProcesses, maxNumOfProcesses, process);
        }
    }
    return 0;
}

void printAlgorithmName(int algType) {
    if (algType == SJF) {
        outf << "* * * * * * * * * * * * * *" << endl;
        outf << "*   Shortest Job First    *" << endl;
        outf << "* * * * * * * * * * * * * *" << endl << endl;
    } else if (algType == RR) {
        outf << "* * * * * * * * * * * * * *" << endl;
        outf << "*       Round Robin       * " << endl;
        outf << "* * * * * * * * * * * * * *" << endl << endl;
    } else if (algType == PR_NO_PREEMP) {
        outf << "* * * * * * * * * * * * * * * * " << endl;
        outf << "* Priority without preemption * " << endl;
        outf << "* * * * * * * * * * * * * * * * " << endl << endl;
    } else if (algType == PR_WITH_PREEMP) {
        outf << "PR_WITH_PREEMP" << endl;
    }
}


void run_PR_WITH_PREEM_schedulingAlg(int seq_num_of_curr_process, int num_of_processes, const processInfo *process) {
    int TIME_QUANTAM = 1;
    seq_num_of_curr_process = 0;
    int seq_num_of_last_process = -1;
    int cur_time = 0;
    int total_waiting_time = 0;

    int *time_it_finished_last_time = new int[num_of_processes + 2];
    for (int i = 0; i < num_of_processes + 2; i++) {
        time_it_finished_last_time[i] = process[i].timeArrive;
    }

    priority_queue<processInfo *, vector<processInfo *>, function
            <bool(const processInfo *, const processInfo *)>>
            prior_queue_of_curr_processes(
            [](const processInfo *s1, const processInfo *s2) { return s1->priority > s2->priority; });

    list<processInfo *> all_processes;
    while (seq_num_of_curr_process < num_of_processes) {
        all_processes.push_back((processInfo *) (&process[seq_num_of_curr_process]));
        seq_num_of_curr_process++;
    }

    list<processInfo *>::iterator iter;
    while (!all_processes.empty()) {
        for (iter = all_processes.begin(); iter != all_processes.end(); ++iter) {
            if ((*iter)->timeArrive <= cur_time) {
                prior_queue_of_curr_processes.push(*iter);
                all_processes.erase(iter);
            }
        }

        processInfo *curr_proc = prior_queue_of_curr_processes.top();
        seq_num_of_curr_process = curr_proc->number;
        if (seq_num_of_curr_process != seq_num_of_last_process) {
            total_waiting_time = cur_time - time_it_finished_last_time[seq_num_of_last_process];
            outf << "\t" << curr_proc->number << " : " << cur_time << endl;
        }

        curr_proc->cpuBurst -= TIME_QUANTAM;

        if (curr_proc->cpuBurst == 0) {
            prior_queue_of_curr_processes.pop();
        }
        seq_num_of_last_process = seq_num_of_curr_process;
        time_it_finished_last_time[seq_num_of_curr_process] = cur_time;
        cur_time++;
    }

    while (!prior_queue_of_curr_processes.empty()) {
        processInfo *cur_process = prior_queue_of_curr_processes.top();
        seq_num_of_curr_process = cur_process->number;
        prior_queue_of_curr_processes.pop();
        if (seq_num_of_last_process != seq_num_of_curr_process) {
            outf << "\t" << cur_process->number << " : " << cur_time << endl;
            total_waiting_time += cur_time - time_it_finished_last_time[seq_num_of_curr_process];
        }
        cur_time += cur_process->cpuBurst;
        seq_num_of_last_process = seq_num_of_curr_process;
    }
    outf << "\nAverage waiting time: " << (float) total_waiting_time / num_of_processes << endl << endl << endl;
}


// Process number           Arrival Time           CPU burst time            Priority
//4
//1     0      8       3
//2     3      1       1
//3     5      2       4
//4     6      2       2
void run_PR_NO_PREEM_scheduler(int seq_num_of_curr_process, int num_of_processes, const processInfo *process) {
    seq_num_of_curr_process = 0;
    int cur_time = 0;
    int total_waiting_time = 0;

    priority_queue<processInfo *, vector<processInfo *>, function
            <bool(const processInfo *, const processInfo *)>>
            prior_queue_of_curr_processes(
            [](const processInfo *s1, const processInfo *s2) { return s1->priority > s2->priority; });

    list<processInfo *> all_processes;
    while (seq_num_of_curr_process < num_of_processes) {
        all_processes.push_back((processInfo *) (&process[seq_num_of_curr_process]));
        seq_num_of_curr_process++;
    }

    list<processInfo *>::iterator iter;
    while (!all_processes.empty()) {
        for (iter = all_processes.begin(); iter != all_processes.end(); ++iter) {
            if ((*iter)->timeArrive <= cur_time) {
                prior_queue_of_curr_processes.push(*iter);
                all_processes.erase(iter);
            }
        }
        while (!prior_queue_of_curr_processes.empty()) {
            processInfo *cur_process = prior_queue_of_curr_processes.top();
            prior_queue_of_curr_processes.pop();
            outf << "\t" << cur_process->number << " : " << cur_time << endl;
            total_waiting_time += cur_time;
            cur_time += cur_process->cpuBurst;
        }
    }

    outf << "\nAverage waiting time: " << (float) total_waiting_time / seq_num_of_curr_process << endl << endl << endl;
}

// Process number           Arrival Time           CPU burst time            Priority
void
run_RR_scheduler(int num_of_processes, int max_num_of_processes, const processInfo *process, const int time_quantam) {
    num_of_processes = 0;
    int curr_time = 0;
    int total_waiting_time = 0;
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
            outf << "\t" << curr_proc->number << " : " << curr_time << endl;

            if (prev_process_seq_number != curr_process_seq_number) {
                total_waiting_time += curr_time - time_it_finished_last_time[curr_process_seq_number];
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
            outf << "\t" << curr_proc->number << " : " << curr_time << endl;

            if (prev_process_seq_number != curr_process_seq_number) {
                total_waiting_time += curr_time - time_it_finished_last_time[curr_process_seq_number];
            }

            curr_time += curr_proc->cpuBurst;
            rr_queue.pop();

            if (prev_process_seq_number != curr_process_seq_number) {
                time_it_finished_last_time[curr_process_seq_number] = curr_time; //looks like this line is redundant
                prev_process_seq_number = curr_process_seq_number;
            }
        }
    }

    outf << endl << "Average waiting time: " << (float) total_waiting_time / max_num_of_processes << endl << endl
         << endl;
}

// Process number           Arrival Time           CPU burst time            Priority
void run_SJF_scheduler(int seq_num_of_curr_process, int num_of_processes, const processInfo *process) {
    seq_num_of_curr_process = 0;
    int curr_time = 0;
    int total_time_waiting_time = 0;

    priority_queue<processInfo *, //Define an element
            vector<processInfo *>, //For Internal Use
            function<bool(const processInfo *, const processInfo *)>> //Comparator
            prior_queue_curr_processes(
            [](const processInfo *s1, const processInfo *s2) { return s1->cpuBurst > s2->cpuBurst; });

    //Add all processes to a list
    list<processInfo *> all_processes; //TODO: isn't better to use a vector ???
    while (seq_num_of_curr_process < num_of_processes) {
        all_processes.push_back((processInfo *) (&process[seq_num_of_curr_process]));
        seq_num_of_curr_process++;
    }

    list<processInfo *>::iterator iter;
    while (!all_processes.empty()) {
        for (iter = all_processes.begin(); iter != all_processes.end(); ++iter) {
            if ((*iter)->timeArrive <= curr_time) {
                prior_queue_curr_processes.push(*iter);
                all_processes.erase(iter);
            }
        }
        while (!prior_queue_curr_processes.empty()) {
            processInfo *curr_proc = prior_queue_curr_processes.top();
            prior_queue_curr_processes.pop();
            outf << "\t" << curr_proc->number << " : " << curr_time << endl;
            total_time_waiting_time += curr_time;
            curr_time += curr_proc->cpuBurst;
        }
    }
    outf << "\nAverage waiting time: " << (float) total_time_waiting_time / num_of_processes << endl << endl << endl;
}