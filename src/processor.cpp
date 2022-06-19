#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  long total = LinuxParser::Jiffies();
  long idle = LinuxParser::IdleJiffies();
  float idleD = float(idle) - float(idleJiffies);
  float totalD = float(total) - float(totalJiffies);
  float pcnt = (totalD - idleD) / totalD;

  idleJiffies = idle;
  totalJiffies = total;

  return pcnt;
}