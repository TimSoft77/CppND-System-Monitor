#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Return total used memory per:
// https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop/41251290#41251290
// converted to a % of memTotal to align with UI.
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float memFree, memTotal;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        memTotal = std::stoi(value);
      } else if (key == "MemFree:") {
        memFree = std::stoi(value);
      }
    }
  }
  return (memTotal - memFree)/memTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
    string line, upTime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }
   return std::stol(upTime); // Truncates floats
}

// Read and return the number of jiffies for the system per:
// https://knowledge.udacity.com/questions/129844
long LinuxParser::Jiffies() {
   return sysconf(_SC_CLK_TCK)*UpTime(); 
}

// Read and return the number of active jiffies for a PID per:
// https://knowledge.udacity.com/questions/129844
long LinuxParser::ActiveJiffies(int pid) {
  long activeJiffies = 0;
  string line, token;
  vector<string> tokens = {};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line); // only one line
    std::istringstream linestream(line);
    while (linestream >> token) { // put all the tokens into a vector
      tokens.emplace_back(token);
    }
    for (int i = 13; i <= 16; i++) { // add up the relevant tokens
      activeJiffies += std::stol(tokens[i]);
    }
  }
  return activeJiffies;
}

// Read and return the number of active jiffies for the system per:
// https://knowledge.udacity.com/questions/129844
long LinuxParser::ActiveJiffies() {
  string token;
  string line;
  long activeJiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line); // data is on the 1st line
    linestream >> token; // throw out the key "cpu";
    while (linestream >> token) {
      activeJiffies += std::stol(token);
    }
  }
  return activeJiffies - IdleJiffies();
}

// Read and return the number of idle jiffies for the system per:
// https://knowledge.udacity.com/questions/129844
long LinuxParser::IdleJiffies() {
  string line, cpu, user, nice, system, idle, iowait;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line); // data is on the 1st line
    linestream >> cpu >> user >> nice >> system >> idle >> iowait;
  }
  return std::stol(idle) + std::stol(iowait);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        break;
      }
    }
  }
  return std::stoi(value);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        break;
      }
    }
  }
  return std::stoi(value);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, value);
  }
  return value;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        break;
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        break;
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, key, x, value;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> x >> value) {
        if (value == uid) {
          return key;
        }
      }
    }
  }
  return string();
}

// Read and return the time elapsed since the process started
long LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 1;
    while (linestream >> value) {
      if (i == 22) {
        // See https://man7.org/linux/man-pages/man5/proc.5.html search for "/proc/[pid]/stat"
        break;
      }
      i++;
    }
  }
  long upTime = UpTime() - std::stol(value) / sysconf(_SC_CLK_TCK);
  // Convert the start time from jiffies to seconds, and subtract from total system UpTime
  return upTime;
}
