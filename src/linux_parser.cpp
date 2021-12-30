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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  float mem_total{0};

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      string key;
      string value;
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        mem_total = stof(value);
      }
      if (key == "MemFree:") {
        return (mem_total - stof(value)) / mem_total;
      }
    }
  }
  return mem_total;
}
 
// Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    filestream >> uptime;
  }
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total_system_jiffies{0};
  vector<string> cpu_values = LinuxParser::CpuUtilization();
  for (string value : cpu_values) {
    total_system_jiffies += stol(value);
  }
  return total_system_jiffies; 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long proc_active_jiffies;
  long starttime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string value;
    for (int i = 0; i < 22; i++) {
      filestream >> value;
    }
    starttime = stol(value);
  }

  proc_active_jiffies = LinuxParser::Jiffies() - starttime;

  return proc_active_jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idle_jiffies{0};
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    string line;
    string value;
    std::getline(filestream, line);
    std::stringstream linestream(line);

    // get sum of "Idle" and "IOwait" cpu values
    int i = 0;
    while (linestream >> value) {
      if (i == kIdle_ || i == kIOwait_) {
        idle_jiffies =+ stol(value);
      }
      i++;
    }
  }
  return idle_jiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_util;
  string line;
  string s;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::getline(filestream, s);
  std::stringstream linestream(s);
  while (linestream >> s) {
    if (s != "cpu") {
      cpu_util.push_back(s);
    }
  }
  return cpu_util;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string val{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  while(std::getline(filestream, line)) {
    std::stringstream linestream(line);
    linestream >> key >> val;
    if (key == "processes") {
      return stoi(val);
    }
  }
  return stoi(val);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string val{"0"};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  while(std::getline(filestream, line)) {
    std::stringstream linestream(line);
    linestream >> key >> val;
    if (key == "procs_running") {
      return stoi(val);
    }
  }
  return stoi(val);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  std::getline(filestream, command);
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  long proc_ram_mb{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string line;
    string key;
    string value;
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        proc_ram_mb = stol(value);
      }
    }
  }
  return to_string(proc_ram_mb / 1000);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string uid{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    string line;
    string key;
    string value;
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        uid = value;
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string user_name{""};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string line;
    string value;
    string x;
    string uid;
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream linestream(line);
      linestream >> value >> x >> uid;
      if (uid == LinuxParser::Uid(pid)) {
        user_name = value;
      }
    }
  }
  return user_name;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long proc_uptime{0};
  long starttime{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string value;
    for (int i = 0; i < 22; i++) {
      filestream >> value;
    }
    starttime = stol(value);
  }
  proc_uptime = (LinuxParser::UpTime() - starttime) / sysconf(_SC_CLK_TCK);
  return proc_uptime;
}
