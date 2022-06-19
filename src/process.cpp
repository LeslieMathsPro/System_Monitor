#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int ID) {
  procID = ID;
 
}

// TODO: Return this process's ID
int Process::Pid() { 
  return procID;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
	CPUpcnt = (float)(LinuxParser::ActiveJiffies(procID) / sysconf(_SC_CLK_TCK)) / (float)upTime;
  	return CPUpcnt;
}

// TODO: Return the command that generated this process
string Process::Command() { 
	std::string command = LinuxParser::Command(procID);
   	return command.length() > 40 ? command.substr(0, 40) + "..." : command;
}

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(procID); }

// TODO: Return the user (name) that generated this process
string Process::User() {
	return LinuxParser::User(procID);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
	upTime = LinuxParser::UpTime(procID); 
    
    return upTime;
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  return this->CPUpcnt < a.CPUpcnt;
}