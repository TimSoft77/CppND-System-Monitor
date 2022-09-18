#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
    float processJiffies = LinuxParser::ActiveJiffies(pid_);
    float systemJiffiesSinceStart = UpTime() * sysconf(_SC_CLK_TCK);
    return processJiffies / systemJiffiesSinceStart; }

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { 
    int ram = std::stoi(LinuxParser::Ram(pid_));
    ram /= 1000; // Convert from kB to MB
    return to_string(ram);
}

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return a.pid_ < pid_;
}