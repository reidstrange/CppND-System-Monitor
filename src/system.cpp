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

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes  
vector<Process*>& System::Processes() {
  // Create vector of all current pids
  vector<int> pids = LinuxParser::Pids();

  // For each process in processes_, check if its pid is still an active
  // pid.  If not, delete reference and remove from processes_.
  int processes_size = processes_.size();
  for (int i = 0; i < processes_size; i++) {
    bool proc_pid_in_pids = std::find(pids.begin(), pids.end(), (processes_[i])->Pid()) != pids.end();
    if (!proc_pid_in_pids) {
      delete (processes_[i]);
      processes_.erase(processes_.begin() + i);
    }
  }

  // For each active pid:
  for (int pid : pids) {
    // Check if pid was already active on last Processes() call.
    // If pid was not already active, add to processes_
    bool pid_in_prev_pids = std::find(prev_pids_.begin(), prev_pids_.end(), pid) != prev_pids_.end();
    if (!pid_in_prev_pids) {
      Process* new_process = new Process(pid);
      System::processes_.push_back(new_process);
    }
  }

  // clear prev_pids_ and store curren pids vector in 
  // prev_pids_ for next Processes() call.
  prev_pids_.clear();
  prev_pids_= pids;

  // Sort all Process pointers in processes_ by their cpu_utilization values
  std::sort(processes_.begin(), processes_.end(), Process::CompareProcessPtr);
  std::reverse(processes_.begin(), processes_.end());   // necessary to reverse???
  return System::processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
