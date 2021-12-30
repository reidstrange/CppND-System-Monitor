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

// TODO: Return this process's ID
int Process::Pid() { return this->pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  float system_time = LinuxParser::Jiffies();     // System Uptime in seconds
  float proc_time;
  float utime;                              // Process Time values
  float stime;
  
  // Get active jiffies of process
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    string value;
    for (int i = 0; i < 15; i++) {
      filestream >> value;
      if (i == 13) {
        utime = stof(value);
      }
      if (i == 14) {
        stime = stof(value);
      }
    }
  }
  proc_time = utime + stime;

  cpu_utilization_ = (proc_time - proc_time_previous_) / (system_time - system_time_previous_);
  proc_time_previous_ = proc_time;
  system_time_previous_ = system_time;

  return cpu_utilization_;
}

// TODO: Return the command that generated this process
string Process::Command() { return this->command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return this->ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return this->user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return this->uptime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return this->cpu_utilization_ < a.cpu_utilization_;
}