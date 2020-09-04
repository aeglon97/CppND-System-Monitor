#include <experimental/filesystem>
#include <exception>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <dirent.h>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::map;

namespace fs = std::experimental::filesystem;

//HELPER FUNCTIONS

//Parse file keys for generic values
template<typename Type> Type ParseValueByKey(const string path, const string myKey) {
  string line, key;
  Type value;

  std::ifstream stream(path);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      while(lineStream >> key >> value) {
        if (key == myKey) {
          stream.close();
          return value;
        }
      }
    }
  }
  return value;
}

float KbToMb(float kb) {
  return kb / 1024;
}

//Split string by whitespace into a vector
vector<string> LinuxParser::SplitLine(const string line) {
  std::istringstream buffer(line);
  std::istream_iterator<string> beg(buffer), end;
  vector<string> tokens(beg, end);

  return tokens;
}

//SYSTEM
string LinuxParser::OperatingSystem() {
  string line, key, value;
  std::ifstream stream(kOSPath); 
    if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::replace(line.begin(), line.end(), ' ', '_');
        std::replace(line.begin(), line.end(), '=', ' ');
        std::replace(line.begin(), line.end(), '"', ' ');
        std::istringstream lineStream(line);
        while (lineStream >> key >> value) {
          if (key == "PRETTY_NAME") {
            std::replace(value.begin(), value.end(), '_', ' ');
            stream.close();
            return value;
        }
      }
    }
  }
  stream.close();
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

//Store pids with std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for (const auto &entry : fs::directory_iterator(kProcDirectory)) {
    //Is this a directory?
    if (fs::is_directory(entry)) {
      string directoryName = fs::path(entry.path()).filename();
      //Is every character of the directory name a digit?
      if (std::all_of(directoryName.begin(), directoryName.end(), isdigit)) {
        try {
          int pid = stoi(directoryName);
          pids.push_back(pid);
        } catch (std::exception& e) {
          std::cerr << e.what() << std::endl;
        }
      }
    }
  }
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string line, key, value, kb;
  float memTotal, memFree, memUtil;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      while (lineStream >> key >> value) {

        //calculate memory utilization
        if (memTotal && memFree) {
          memUtil = (memTotal - memFree) / memTotal;
          stream.close();
          return memUtil;
        }

        if(key == "MemTotal") {
          memTotal = stof(value);
        }

        if(key == "MemFree") {
          memFree = stof(value);
        }
      }
    }
  }
  stream.close();
  return memUtil; 
}

long LinuxParser::UpTime() { 
  long upTime, idleTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> upTime >> idleTime;
    stream.close();
  }
  return upTime;
 }

//Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

//Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

//Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

//Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0 ; }

//Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  return {}; 
}

int LinuxParser::TotalProcesses() {
  int totalProcesses = ParseValueByKey<int>(kProcDirectory + kStatFilename, "processes");
  return totalProcesses;
}

int LinuxParser::RunningProcesses() {
  int runningProcesses = ParseValueByKey<int>(kProcDirectory + kStatFilename, "procs_running");
  return runningProcesses;
}

//PROCESSES 

string LinuxParser::Command(const int pid) { 
  string line;
  std::ifstream stream(kProcProcessDirectory + to_string(pid) + kCmdlineFilename); 
  if (stream.is_open()) {
    std::getline(stream, line);
    stream.close();
  }
  return line;
}

float LinuxParser::Ram(const int pid) {
  float ramKb = ParseValueByKey<float>(kProcProcessDirectory + to_string(pid) + kStatusFilename, "VmSize");
  float ramMb = KbToMb(ramKb);
  return ramMb;
 }

string LinuxParser::Uid(const int pid) { 
  const string uid = ParseValueByKey<string>(kProcProcessDirectory + to_string(pid) + kStatusFilename, "Uid");
  return uid;
 }

string LinuxParser::User(const int pid) {
  string line, username, x, uid;
  const string myuid = Uid(pid);
  std::ifstream stream(kPasswordPath);

  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      while (lineStream >> username >> x >> uid) {
        if (myuid == uid) { 
          stream.close();
          return username; 
        }
      }
    }
  }
  stream.close();
  return username;
}

long int LinuxParser::UpTime(const int pid) {
  string line;
  std::ifstream stream(kProcProcessDirectory + to_string(pid) + kStatFilename);
  
  if(stream.is_open()) {
    std::getline(stream, line);
    stream.close();
  }

  vector<string> procInfo = SplitLine(line);
  long unsigned int startTime;
  try {
    startTime = stol(procInfo[21]);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  //Convert from clock ticks to seconds
  long unsigned int upTime = LinuxParser::UpTime() - (startTime / sysconf(_SC_CLK_TCK));
  return upTime;
}