#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>
#include <list>
using namespace std;

//Input File:
// Process number           Arrival Time           CPU burst time            Priority

typedef enum {
    SJF,            //SHORTEST JOB FIRST
    RR,             //ROUND ROBIN
    PR_NO_PREEMP,     //PRIORITY SCHEDULING | NO PREEMPTION
    PR_WITH_PREEMP    //PRIORITY SCHEDULING | PREEMPTION
} SCH_ALGORITHM;

struct ProcessInfo {
    int number;
    int timeArrive;
    int cpuBurst;
    int priority;
};

void run_SJF_scheduler(int seq_num_of_curr_process, int num_of_processes, const ProcessInfo *process);

void
run_RR_scheduler(int num_of_processes, int max_num_of_processes, const ProcessInfo *process, const int time_quantam);

void run_PR_NO_PREEM_scheduler(int seq_num_of_curr_process, int num_of_processes, const ProcessInfo *process);

void printAlgorithmName(int algType);

void run_PR_WITH_PREEM_schedulingAlg(int seq_num_of_curr_process, int num_of_processes, const ProcessInfo *process);

ofstream out_file_stream("output.txt");

int main() {
    int num_proc = 0;
    int num_of_processes;
    struct ProcessInfo *processes;
    int const time_quantam = 4;

    int ALG_TYPE[4];

    if (!out_file_stream) {
        cerr << "output.txt could not be opened for writing!" << endl;
        cout << "error";
        exit(1);
    }

    ifstream input_file_stream("input.txt");
    if (!input_file_stream) {
        cerr << "input.txt could not be opened for reading!" << endl;
        exit(1);
    }

    string one_line_of_input;
    stringstream sch_algorithms;
    getline(input_file_stream, one_line_of_input);
    sch_algorithms << one_line_of_input;

    int counter = 0;
    string local_buffer;
    while (sch_algorithms >> local_buffer) {
        if (local_buffer.compare("SJF") == 0) {
            ALG_TYPE[counter] = ::SJF;
        } else if (local_buffer.compare("RR") == 0) {
            ALG_TYPE[counter] = ::RR;
        } else if (local_buffer.compare("PR_NO_PREEMP") == 0) {
            ALG_TYPE[counter] = ::PR_NO_PREEMP;
        } else if (local_buffer.compare("PR_WITH_PREEMP") == 0) {
            ALG_TYPE[counter] = ::PR_WITH_PREEMP;
        }
        counter++;
    }

    stringstream num_of_proc_stream;

    getline(input_file_stream, one_line_of_input);
    num_of_proc_stream << one_line_of_input;
    num_of_proc_stream >> num_of_processes;
    cout << "The number of processes is: " << num_of_processes << endl;
    processes = (ProcessInfo *) calloc(num_of_processes, sizeof(ProcessInfo));
    num_of_proc_stream.str("");

    stringstream proc_info_string_stream;
    num_proc = 0;
    while (num_proc < num_of_processes) {
        getline(input_file_stream, one_line_of_input);
        proc_info_string_stream << one_line_of_input;
        string cur_value;

        int counter = 0;
        while (proc_info_string_stream >> cur_value) {
            cout << cur_value << " ";
            if (counter == 0) {
                processes[num_proc].number = stoi(cur_value);
            } else if (counter == 1) {
                processes[num_proc].timeArrive = stoi(cur_value);
            } else if (counter == 2) {
                processes[num_proc].cpuBurst = stoi(cur_value);
            } else if (counter == 3) {
                processes[num_proc].priority = stoi(cur_value);
            }
            counter++;
        }
        num_proc++;
        proc_info_string_stream.clear();
        cout << endl;
    }

    for (int i = 0; i < 4; i++) {
        ProcessInfo *processes_deep_copy = new ProcessInfo[num_of_processes];
        for (int i = 0; i < num_of_processes; i++) {
            processes_deep_copy[i].number = processes[i].number;
            processes_deep_copy[i].timeArrive = processes[i].timeArrive;
            processes_deep_copy[i].cpuBurst = processes[i].cpuBurst;
            processes_deep_copy[i].priority = processes[i].priority;
        }

        printAlgorithmName(ALG_TYPE[i]);
        if (ALG_TYPE[i] == ::SJF) {
            run_SJF_scheduler(num_proc, num_of_processes, processes_deep_copy);
        } else if (ALG_TYPE[i] == ::RR) {
            run_RR_scheduler(num_proc, num_of_processes, processes_deep_copy, time_quantam);
        } else if (ALG_TYPE[i] == ::PR_NO_PREEMP) {
            run_PR_NO_PREEM_scheduler(num_proc, num_of_processes, processes_deep_copy);
        } else {
            run_PR_WITH_PREEM_schedulingAlg(num_proc, num_of_processes, processes);
        }
    }
    return 0;
}

void printAlgorithmName(int algType) {
    if (algType == SJF) {
        out_file_stream << "* * * * * * * * * * * * * * * * * * " << endl;
        out_file_stream << "*  Shortest Job First Scheduling  *" << endl;
        out_file_stream << "* * * * * * * * * * * * * * * * * * " << endl << endl;
    } else if (algType == RR) {
        out_file_stream << "* * * * * * * * * * * * * *" << endl;
        out_file_stream << "* Round Robin Scheduling  * " << endl;
        out_file_stream << "* * * * * * * * * * * * * *" << endl << endl;
    } else if (algType == PR_NO_PREEMP) {
        out_file_stream << "* * * * * * * * * * * * * * * * * * * * * * " << endl;
        out_file_stream << "* Priority without Preemption Scheduling  * " << endl;
        out_file_stream << "* * * * * * * * * * * * * * * * * * * * * * " << endl << endl;
    } else if (algType == PR_WITH_PREEMP) {
        out_file_stream << "* * * * * * * * * * * * * * * * * * * * * " << endl;
        out_file_stream << "*  Priority with Preemption Scheduling  *" << endl;
        out_file_stream << "* * * * * * * * * * * * * * * * * * * * * " << endl << endl;
    }
}


void run_PR_WITH_PREEM_schedulingAlg(int seq_num_of_curr_process, int num_of_processes, const ProcessInfo *process) {

    int TIME_QUANTAM = 1;
    seq_num_of_curr_process = 0;
    int seq_num_of_last_process = -1;
    int cur_time = 0;
    int total_waiting_time = 0;

    int *time_it_finished_last_time = new int[num_of_processes + 2];
    for (int i = 0; i < num_of_processes; i++) {
        time_it_finished_last_time[i] = process[i].timeArrive;
    }

    priority_queue<ProcessInfo *, vector<ProcessInfo *>, function
            <bool(const ProcessInfo *, const ProcessInfo *)>>
            prior_queue_of_curr_processes(
            [](const ProcessInfo *s1, const ProcessInfo *s2) { return s1->priority > s2->priority; });

    list<ProcessInfo *> all_processes;
    while (seq_num_of_curr_process < num_of_processes) {
        all_processes.push_back((ProcessInfo *) (&process[seq_num_of_curr_process]));
        seq_num_of_curr_process++;
    }

    list<ProcessInfo *>::iterator iter;
    while (!all_processes.empty()) {
        for (iter = all_processes.begin(); iter != all_processes.end(); ++iter) {
            if ((*iter)->timeArrive <= cur_time) {
                prior_queue_of_curr_processes.push(*iter);
                all_processes.erase(iter);
            }
        }

        ProcessInfo *curr_proc = prior_queue_of_curr_processes.top();
        seq_num_of_curr_process = curr_proc->number;
        if (seq_num_of_curr_process != seq_num_of_last_process) {
            total_waiting_time = cur_time - time_it_finished_last_time[seq_num_of_last_process];
            out_file_stream << "\t" << curr_proc->number << " : " << cur_time << endl;
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
        ProcessInfo *cur_process = prior_queue_of_curr_processes.top();
        seq_num_of_curr_process = cur_process->number;
        prior_queue_of_curr_processes.pop();
        if (seq_num_of_last_process != seq_num_of_curr_process) {
            out_file_stream << "\t" << cur_process->number << " : " << cur_time << endl;
            total_waiting_time += cur_time - time_it_finished_last_time[seq_num_of_curr_process];
        }
        cur_time += cur_process->cpuBurst;
        seq_num_of_last_process = seq_num_of_curr_process;
    }
    out_file_stream << "\nAverage waiting time: " << (float) total_waiting_time / num_of_processes << endl << endl << endl;
}


void run_PR_NO_PREEM_scheduler(int seq_num_of_curr_process, int num_of_processes, const ProcessInfo *process) {
    seq_num_of_curr_process = 0;
    int cur_time = 0;
    int total_waiting_time = 0;

    priority_queue<ProcessInfo *, vector<ProcessInfo *>, function
            <bool(const ProcessInfo *, const ProcessInfo *)>>
            prior_queue_of_curr_processes(
            [](const ProcessInfo *s1, const ProcessInfo *s2) { return s1->priority > s2->priority; });

    list<ProcessInfo *> all_processes;
    while (seq_num_of_curr_process < num_of_processes) {
        all_processes.push_back((ProcessInfo *) (&process[seq_num_of_curr_process]));
        seq_num_of_curr_process++;
    }

    list<ProcessInfo *>::iterator iter;
    while (!all_processes.empty()) {
        for (iter = all_processes.begin(); iter != all_processes.end(); ++iter) {
            if ((*iter)->timeArrive <= cur_time) {
                prior_queue_of_curr_processes.push(*iter);
                all_processes.erase(iter);
            }
        }
        while (!prior_queue_of_curr_processes.empty()) {
            ProcessInfo *cur_process = prior_queue_of_curr_processes.top();
            prior_queue_of_curr_processes.pop();
            out_file_stream << "\t" << cur_process->number << " : " << cur_time << endl;
            total_waiting_time += cur_time;
            cur_time += cur_process->cpuBurst;
        }
    }

    out_file_stream << "\nAverage waiting time: " << (float) total_waiting_time / seq_num_of_curr_process << endl << endl << endl;
}

void run_RR_scheduler(int num_of_processes, int max_num_of_processes, const ProcessInfo *process, const int time_quantam) {
    num_of_processes = 0;
    int curr_time = 0;
    int total_waiting_time = 0;
    int prev_process_seq_number = 0;
    int curr_process_seq_number;
    int *time_it_finished_last_time = new int[max_num_of_processes+1]; // +1 because processes' sequence numbers start from 1
    for (int i = 0; i < max_num_of_processes+1; i++) {
        time_it_finished_last_time[i] = 0;
    }

    queue<ProcessInfo *> rr_queue;
    while (num_of_processes < max_num_of_processes) {
        rr_queue.push((ProcessInfo *) (&process[num_of_processes]));
        num_of_processes++;
    }
    while (!rr_queue.empty()) {
        int curr_process_BURST = ((ProcessInfo *) rr_queue.front())->cpuBurst;
        curr_process_seq_number = ((ProcessInfo *) rr_queue.front())->number;

        if (curr_process_BURST > time_quantam) {
            ProcessInfo *curr_proc = rr_queue.front();
            curr_proc->cpuBurst -= time_quantam;
            out_file_stream << "\t" << curr_proc->number << " : " << curr_time << endl;

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
            ProcessInfo *curr_proc = rr_queue.front();
            out_file_stream << "\t" << curr_proc->number << " : " << curr_time << endl;

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

    out_file_stream << endl << "Average waiting time: " << (float) total_waiting_time / max_num_of_processes << endl << endl
         << endl;
}

void run_SJF_scheduler(int seq_num_of_curr_process, int num_of_processes, const ProcessInfo *process) {
    seq_num_of_curr_process = 0;
    int curr_time = 0;
    int total_time_waiting_time = 0;

    priority_queue<ProcessInfo *, //Define an element
            vector<ProcessInfo *>, //For Internal Use
            function<bool(const ProcessInfo *, const ProcessInfo *)>> //Comparator
            prior_queue_curr_processes(
            [](const ProcessInfo *s1, const ProcessInfo *s2) { return s1->cpuBurst > s2->cpuBurst; });

    //Add all processes to a list
    list<ProcessInfo *> all_processes; //TODO: isn't better to use a vector ???
    while (seq_num_of_curr_process < num_of_processes) {
        all_processes.push_back((ProcessInfo *) (&process[seq_num_of_curr_process]));
        seq_num_of_curr_process++;
    }

    list<ProcessInfo *>::iterator iter;
    while (!all_processes.empty()) {
        for (iter = all_processes.begin(); iter != all_processes.end(); ++iter) {
            if ((*iter)->timeArrive <= curr_time) {
                prior_queue_curr_processes.push(*iter);
                all_processes.erase(iter);
            }
        }
        while (!prior_queue_curr_processes.empty()) {
            ProcessInfo *curr_proc = prior_queue_curr_processes.top();
            prior_queue_curr_processes.pop();
            out_file_stream << "\t" << curr_proc->number << " : " << curr_time << endl;
            total_time_waiting_time += curr_time;
            curr_time += curr_proc->cpuBurst;
        }
    }
    out_file_stream << "\nAverage waiting time: " << (float) total_time_waiting_time / num_of_processes << endl << endl << endl;
}