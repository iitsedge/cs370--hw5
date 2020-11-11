#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //pid_t
#include <unistd.h> //fork(), execlp()
#include <sys/wait.h> //wait(), WEXITSTATUS()
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iomanip>

using namespace std;

struct Process {
    public:
        int pid;
        int arr_time;
        int burst_time;
        int priority;
        int turnaround_time;
        int wait_time;
        int remaining_time;
        int start_time;
        int end_time;
};

struct sortProcessByArrival{ //can also use normal function and pass func name to sort instead of comparator
    bool operator() (const Process &first, const Process &second) {
        if (first.arr_time == second.arr_time){
            return first.pid < second.pid;
        }
        return first.arr_time < second.arr_time;
    }
};

struct sortProcessByPriority{ 
    bool operator() (const Process &first, const Process &second) {
        if (first.priority == second.priority){
            if (first.arr_time == second.arr_time){
                return first.pid < second.pid;
            }else{
                return first.arr_time < second.arr_time;
            }
        }
        return first.priority < second.priority;
    }
};

struct sortProcessByBurst{ 
    bool operator() (const Process &first, const Process &second) {
        if (first.burst_time == second.burst_time){
            if (first.arr_time == second.arr_time){
                return first.pid < second.pid;
            }else{
                return first.arr_time < second.arr_time;
            }
        }
        return first.burst_time < second.burst_time;
    }
};

bool find(int pid, vector<Process> ready){
    bool found = false;
    for (Process p : ready){
        if (p.pid == pid){
            found = true;
        }
    }
    return found;
}

int findBurst(int pid, vector<Process> processes){
    for (Process p : processes){
        if (p.pid == pid){
            return p.burst_time;
        }
    }
    return 0;
}

void FCFS(vector<Process> processes){
    //cout << processes.at(1).priority << "\n";
    sortProcessByArrival comparator;
    sort(processes.begin(), processes.end(), comparator);
    vector<Process> ready;
    vector<Process> complete;
    int clock = -1;
    //int temp_burst;

    // cout << "Size of Processes: " << processes.size() << "\n";
    // cout << "Size of Ready: " << ready.size() << "\n";
    // cout << "Size of Complete: " << complete.size() << "\n";

    while (processes.size()!=complete.size()){
        //bool idle = true;
        clock++;
        for (unsigned int i =0;i<processes.size();i++){
            if (processes.at(i).arr_time<=clock && !find(processes.at(i).pid, ready) && !find(processes.at(i).pid, complete)){
                ready.push_back(processes.at(i));
                //processes.erase(processes.begin()+i);
                // idle = false;
            }
        }
        // if (ready.empty()){
        //     for (Process p : ready){
        //         p.wait_time++;
        //     }
        // }
        if (!ready.empty()){
            if (ready.at(0).burst_time == 0){
                ready.at(0).turnaround_time = ready.at(0).wait_time + findBurst(ready.at(0).pid, processes);
                //ready.at(0).turnaround_time = clock - ready.at(0).arr_time;
                // cout << "completed proc wait: " << ready.at(0).wait_time<<"\n";
                // cout << "completed proc burst: " << findBurst(ready.at(0).pid, processes)<<"\n";
                // cout << "completed proc turnaround: " << ready.at(0).turnaround_time<<"\n";
                complete.push_back(ready.at(0));
                ready.erase(ready.begin());
            }
            if (!ready.empty()){
                ready.at(0).burst_time--;
            }
            if (ready.size()>1){
                for (unsigned int i =1;i<ready.size();i++){
                    ready.at(i).wait_time++;
                }
            }
        }
         //clock++;
    }
    //cout<< "total clock ticks: "<<clock<<"\n";
    double total_turnaround = 0.0;
    double total_wait = 0.0;
    //double throughput = 0;
    for (Process p : complete){
        total_turnaround += p.turnaround_time;
        //cout << "Turnaround: " << ave_turnaround << "\n";
        total_wait += p.wait_time;
    }
    // cout << "COMPLETE ---\n";
    // for (Process p:complete){
    //     cout << "pid = " << p.pid << "\tArrival_time = " << p.arr_time << "\tBurst_time = " << findBurst(p.pid, processes)
    //      << "\tPriority = " << p.priority << "\tWait: "<<p.wait_time<< "\tTurnaround: "<<p.turnaround_time<<"\n";
    // }
    //cout << "Clock: " << clock << "\n";
    cout << setprecision(3) << fixed;
    cout << "--- FCFS ---\n";
    cout << "Average Turnaround Time: " << total_turnaround / complete.size() << "\n";
    cout << "Average Waiting Time: " << total_wait / complete.size() << "\n";
    cout << "Throughput: " << double(complete.size()) / double(clock) << "\n\n";
    
    // cout << "Size of Processes: " << processes.size() << "\n";
    // cout << "Size of Ready: " << ready.size() << "\n";
    // cout << "Size of Complete: " << complete.size() << "\n";
}

void SJFP (vector<Process> processes){

    sortProcessByBurst comparator;
    sort(processes.begin(), processes.end(), comparator);
    vector<Process> ready;
    vector<Process> complete;
    int clock = -1;

    while (processes.size()!=complete.size()){
        clock++;
        for (unsigned int i =0;i<processes.size();i++){
            if (processes.at(i).arr_time<=clock && !find(processes.at(i).pid, ready) && !find(processes.at(i).pid, complete)){
                ready.push_back(processes.at(i));
            }
        }
        if (!ready.empty()){
            // vector<Process> temp = ready;
            // sort(temp.begin(), temp.end(), comparator);
            // if (temp.at(0).burst_time==ready.at(0).burst_time){//keep current running process on if they have equal burst_times
            //     sort(ready.begin()+1, ready.end(), comparator);
            // }else{
            //     sort(ready.begin(), ready.end(), comparator);
            // }
            //sort(ready.begin(), ready.end(), comparator);
            if (ready.at(0).burst_time == 0){
                ready.at(0).turnaround_time = ready.at(0).wait_time + findBurst(ready.at(0).pid, processes);
                complete.push_back(ready.at(0));
                ready.erase(ready.begin());
            }
            if (!ready.empty()){
                // vector<Process> temp = ready;
                // sort(temp.begin(), temp.end(), comparator);
                // if (temp.at(0).burst_time==ready.at(0).burst_time){//keep current running process on if they have equal burst_times
                //     sort(ready.begin()+1, ready.end(), comparator);
                // }else{
                //     sort(ready.begin(), ready.end(), comparator);
                // }
                sort(ready.begin(), ready.end(), comparator);
                ready.at(0).burst_time--;
            }
            if (ready.size()>1){
                for (unsigned int i =1;i<ready.size();i++){
                    ready.at(i).wait_time++;
                }
            }
        }
         //clock++;
    }
    double total_turnaround = 0.0;
    double total_wait = 0.0;
    for (Process p : complete){
        total_turnaround += p.turnaround_time;
        total_wait += p.wait_time;
    }

    cout << setprecision(3) << fixed;
    cout << "--- SJFP ---\n";
    cout << "Average Turnaround Time: " << total_turnaround / complete.size() << "\n";
    cout << "Average Waiting Time: " << total_wait / complete.size() << "\n";
    cout << "Throughput: " << double(complete.size()) / double(clock) << "\n\n";

}

void Priority (vector<Process> processes){

    sortProcessByPriority comparator;
    sort(processes.begin(), processes.end(), comparator);
    vector<Process> ready;
    vector<Process> complete;
    int clock = -1;

    while (processes.size()!=complete.size()){
        clock++;
        for (unsigned int i =0;i<processes.size();i++){
            if (processes.at(i).arr_time<=clock && !find(processes.at(i).pid, ready) && !find(processes.at(i).pid, complete)){
                ready.push_back(processes.at(i));
            }
        }
        if (!ready.empty()){
            //sort(ready.begin(), ready.end(), comparator);
            if (ready.at(0).burst_time == 0){
                ready.at(0).turnaround_time = ready.at(0).wait_time + findBurst(ready.at(0).pid, processes);
                complete.push_back(ready.at(0));
                ready.erase(ready.begin());
            }
            if (!ready.empty()){
                // vector<Process> temp = ready;
                // sort(temp.begin(), temp.end(), comparator);
                // if (temp.at(0).burst_time==ready.at(0).burst_time){//keep current running process on if they have equal burst_times
                //     sort(ready.begin()+1, ready.end(), comparator);
                // }else{
                //     sort(ready.begin(), ready.end(), comparator);
                // }
                sort(ready.begin(), ready.end(), comparator);
                ready.at(0).burst_time--;
                
            }
            if (ready.size()>1){
                for (unsigned int i =1;i<ready.size();i++){
                    ready.at(i).wait_time++;
                }
            }
        }
         //clock++;
    }
    double total_turnaround = 0.0;
    double total_wait = 0.0;
    for (Process p : complete){
        total_turnaround += p.turnaround_time;
        total_wait += p.wait_time;
    }
    // cout << "Size of Processes: " << processes.size() << "\n";
    // cout << "Size of Ready: " << ready.size() << "\n";
    // cout << "Size of Complete: " << complete.size() << "\n";
    cout << setprecision(3) << fixed;
    cout << "--- Priority ---\n";
    cout << "Average Turnaround Time: " << total_turnaround / complete.size() << "\n";
    cout << "Average Waiting Time: " << total_wait / complete.size() << "\n";
    cout << "Throughput: " << double(complete.size()) / double(clock) << "\n\n";
}

int main (int argc, char *argv[]){

    if (argc != 2){
        cout << "Incorrect Arguments" << std::endl;
        return -1;
    }

    vector<Process> processes;
    string line;
    ifstream input_file(argv[1]);

    if (input_file.is_open()){
        while (getline(input_file, line)){
            istringstream stream(line);
            int index = 0;
            Process p;
            while (stream){
                string value;
                if (!getline(stream, value, ',')) break;
                switch (index){
                    case 0:
                        p.pid = stoi(value);
                        break;
                    case 1:
                        p.arr_time = stoi(value);
                        break;
                    case 2:
                        p.burst_time = stoi(value);
                        break;
                    case 3:
                        p.priority = stoi(value);
                        break;
                }
                ++index;
            }
            //init other struct variables
            p.turnaround_time = 0;
            p.wait_time = 0;
            p.remaining_time = 0;
            p.start_time = 0;
            p.end_time = 0;
            //add process to vector: processes.push_back(p)
            processes.push_back(p);
        }
    }

    // for (Process p : processes){
    //     cout << "pid = " << p.pid << "\tArrival_time = " << p.arr_time << "\tBurst_time = " << p.burst_time
    //     << "\tPriority = " << p.priority << "\n";
    // }
    // sortProcessByArrival comparator;
    // sort(processes.begin(), processes.end(), comparator);
    // cout << "After sorting\n";

    // for (Process p : processes){
    //     cout << "pid = " << p.pid << "\tArrival_time = " << p.arr_time << "\tBurst_time = " << p.burst_time
    //     << "\tPriority = " << p.priority << "\n";
    // }
    FCFS(processes);
    SJFP(processes);
    Priority(processes);
    
}