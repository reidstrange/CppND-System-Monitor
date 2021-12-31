#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
*/
class Process {
 public:
  Process(int pid);
  int Pid();                               // See src/process.cpp
  std::string User();                      // See src/process.cpp
  std::string Command();                   // See src/process.cpp
  float CpuUtilization();                  // See src/process.cpp
  std::string Ram();                       // See src/process.cpp
  long int UpTime();                       // See src/process.cpp
  bool operator<(Process const& a) const;  // See src/process.cpp
  static bool CompareProcessPtr(Process* proc_ptr1, Process* proc_ptr2);

 private:
  int pid_;
  std::string user_;
  std::string command_;
  float cpu_utilization_;
  float proc_time_previous_;
  float system_time_previous_;
  std::string ram_;
  long int uptime_;
};

#endif