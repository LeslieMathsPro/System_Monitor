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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
	int total_mem(0), free_mem(0), value;
  	string line, key;
  	std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  	if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
      	std::istringstream linestream(line);
        while (linestream >> key >> value) {
        	if (key == filterMemTotalString)
              total_mem = value;
          	else if (key == filterMemFreeString)
              free_mem = value;
        }
      }
    }
  
  float pcnt = (float)(total_mem - free_mem)/(float)total_mem;
  
  return pcnt;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
	string line, key;
  	long t_system;
  	std::ifstream stream(kProcDirectory + kUptimeFilename);
  	if (stream.is_open()) {
    	std::getline(stream, line);
      	std::istringstream linestream(line);
      	linestream >> key >> t_system;
    }
  return t_system;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
	int index;
  	long long sum;
  
  	std::string line;
  	std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  
  	if(filestream.is_open()) {
    	std::getline(filestream, line);
      	std::stringstream linestream(line);
      	for (string word; std::getline(linestream, word, ' '); ) {
        	if (index == utime_ || index == stime_ ||
                index == cutime_ || index == cstime_ ) {
            	sum += stoll(word);
            }
          index++;
        }
    }
  return sum;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return (AllDigit(jiffies[kUser_]) ? stol(jiffies[kUser_]) : 0) +
         (AllDigit(jiffies[kNice_]) ? stol(jiffies[kNice_]) : 0) +
         (AllDigit(jiffies[kSystem_]) ? stol(jiffies[kSystem_]) : 0) +
         (AllDigit(jiffies[kIRQ_]) ? stol(jiffies[kIRQ_]) : 0) +
         (AllDigit(jiffies[kSoftIRQ_]) ? stol(jiffies[kSoftIRQ_]) : 0) +
         (AllDigit(jiffies[kSteal_]) ? stol(jiffies[kSteal_]) : 0);
}

bool LinuxParser::AllDigit(std::string& str)
{ 
    for (auto it = str.begin(); it != str.end(); it++) {
    if (!std::isdigit(*it)) return false;
    }
    return true;

}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return (AllDigit(jiffies[kIdle_]) ? stol(jiffies[kIdle_]) : 0) +
         (AllDigit(jiffies[kIOwait_]) ? stol(jiffies[kIOwait_]) : 0);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
 string line, key;
  vector<string> jiffies(kGuestNice_ + 1);
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == filterCpu) {
          linestream >> jiffies[kUser_] >> jiffies[kNice_] >>
              jiffies[kSystem_] >> jiffies[kIdle_] >> jiffies[kIOwait_] >>
              jiffies[kIRQ_] >> jiffies[kSoftIRQ_] >> jiffies[kSteal_] >>
              jiffies[kGuest_] >> jiffies[kGuestNice_];
        }
      }
    }
  }
  return jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
	string line;
  	string key;
  	int value = 0;
  	std::ifstream filestream(kProcDirectory + kStatFilename);
  	if (filestream.is_open()) {
    	while(std::getline(filestream, line)) {
        	std::istringstream linestream(line);
          	while (linestream >> key >> value) {
            	if (key == filterProcesses) {
                	return value;
                }
            }
        }
    }
  return value;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
	string line;
  	string key;
  	int value = 0;
  	std::ifstream filestream(kProcDirectory + kStatFilename);
  	if (filestream.is_open()) {
    	while(std::getline(filestream, line)) {
        	std::istringstream linestream(line);
          	while (linestream >> key >> value) {
            	if (key == filterRunningProcesses) {
                	return value;
                }
            }
        }
    }
  return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
	string line, key;
  	std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  	if (filestream.is_open()) std::getline(filestream, line);
  
  	return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value = "";
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterProcMem) {
          if (AllDigit(value))
            value = value.length() > 3
                        ? value.substr(0, value.length() - 3) + "." +
                              value.substr(value.length() - 3, 3)
                        : "0." + value.insert(0, 3 - value.length(), '0');
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value = "";
 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterUID) {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string key;
  string value;
  string userID = LinuxParser::Uid(pid);
   std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (value == userID) return key;
      }
    }
  }
  return value;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
string line, key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int i = 1;
    while (i <= starttime_ && linestream >> value) {
      if (i == starttime_)
        return LinuxParser::UpTime() -
               (AllDigit(value) ? std::stol(value) : 0) / sysconf(_SC_CLK_TCK);
      i++;
    }
  }
  return std::stol(value);
}
