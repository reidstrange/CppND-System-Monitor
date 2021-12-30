#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  float total_cpu = LinuxParser::Jiffies();
  float active_cpu = LinuxParser::ActiveJiffies();
  
  float cpu_percent = (active_cpu - Processor::previous_active_cpu_) / (total_cpu - Processor::prevous_total_cpu_);
  
  Processor::previous_active_cpu_ = active_cpu;
  Processor::prevous_total_cpu_ = total_cpu;

  return cpu_percent;
}