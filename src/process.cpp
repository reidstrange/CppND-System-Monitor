#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor for Process object
Process::Process(int pid) : pid_(pid) {
  Process::user_ = LinuxParser::User(pid);
  Process::command_ = LinuxParser::Command(pid);
  Process::ram_ = LinuxParser::Ram(pid);
  Process::uptime_ = LinuxParser::UpTime(pid);

  // get cpu utilization for this process.
  Process::cpu_utilization_ = Process::CpuUtilization();
}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  float system_time = LinuxParser::Jiffies();     // System Uptime
  float proc_time;
  float utime;                              // Process Time values
  float stime;
  
  // Get active jiffies of process
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    string value;
    for (int i = 1; i < 23; i++) {
      filestream >> value;
      if (i == 14) {
        utime = stof(value);
      }
      if (i == 15) {
        stime = stof(value);
      }
    }
  }
  filestream.close();

  // Total process jiffies is sum of utime and stime from /proc/[pid]/stat
  proc_time = utime + stime;

  // Calculate cpu usage for this process over the last time interval.
  cpu_utilization_ = (proc_time - proc_time_previous_) / (system_time - system_time_previous_);

  // Store current process active time and current total system time to data members
  // for use in next Process::CpuUtilization call.
  proc_time_previous_ = proc_time;
  system_time_previous_ = system_time;

  return cpu_utilization_;
}

// Return the command that generated this process
string Process::Command() { return command_; }

// Return this process's memory utilization
string Process::Ram() { return ram_; }

// Return the user (name) that generated this process
string Process::User() { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return cpu_utilization_ < a.cpu_utilization_;
}

// Compares two unique_ptr<Process> for sorting a vector of unique_ptr<Process>
bool Process::CompareProcess(Process proc1, Process proc2) { 
  return proc1 < proc2; 
}