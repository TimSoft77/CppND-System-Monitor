#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU (just a getter function)
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
// TODO: refactor to make this a hashmap of pids and processes and make data persistent
vector<Process>& System::Processes() {
    processes_.clear();
    vector<int> pids = LinuxParser::Pids();
    for (int pid : pids) {
        if (!LinuxParser::Command(pid).empty()) {
            processes_.emplace_back(pid);
        }
    }
    // Thanks stackoverflow:
    // https://stackoverflow.com/questions/45454320/sorting-vector-of-objects-with-operator-overloading-in-c
    std::sort(processes_.begin() , processes_.end() ,[](Process& proc1 , Process& procr) { return proc1 < procr;} );
    return processes_;
}

// Returns the system's kernel identifier (string).
// Saves name on first call and does not recall LinuxParser again.
std::string System::Kernel() {
     if (KernelName.empty()) {
         KernelName = LinuxParser::Kernel();
     }
     return KernelName; 
}

// Return the system's memory utilization
float System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// Return the operating system name
// Saves name on first call and does not recall LinuxParser again.
std::string System::OperatingSystem() {
    if (OperatingSystemName.empty()) {
        OperatingSystemName = LinuxParser::OperatingSystem();
    } 
    return OperatingSystemName; 
}

// Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() {
    return LinuxParser::UpTime();
}
