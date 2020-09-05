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
  string key;
  Type value;
  
  std::ifstream stream(path);
  string line;

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
vector<string> LinuxParser::ParsedTokens(const string line) {
  std::istringstream buffer(line);
  std::istream_iterator<string> beg(buffer), end;

  vector<string> tokens(beg, end);
  return tokens;
}

//SYSTEM
string LinuxParser::OperatingSystem() {
  string key;
  string value;

  std::ifstream stream(kOSPath); 
  string line;

    if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::replace(line.begin(), line.end(), ' ', '_');
        std::replace(line.begin(), line.end(), '=', ' ');
        std::replace(line.begin(), line.end(), '"', ' ');
        std::istringstream lineStream(line);
        while (lineStream >> key >> value) {
          if (key == filterOS) {
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
  string os;
  string version;
  string kernel;

  std::ifstream stream(kProcDirectory + kVersionFilename);
  string line;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> os >> version >> kernel;
    stream.close();
  }
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
  string key;
  string value;
  string kb;

  float memTotal;
  float memFree;
  float memUtil;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line;

  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      while (lineStream >> key >> value) {

        //Calculate memory utilization
        if (memTotal && memFree) {
          memUtil = (memTotal - memFree) / memTotal;
          stream.close();
          return memUtil;
        }

        if(key == filterMemTotal) {
          memTotal = stof(value);
        }

        if(key == filterMemFree) {
          memFree = stof(value);
        }
      }
    }
  }
  stream.close();
  return memUtil; 
}

long LinuxParser::UpTime() { 
  long upTime;
  long idleTime;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream lineStream(line);
    lineStream >> upTime >> idleTime;
    stream.close();
  }
  return upTime;
 }

int LinuxParser::TotalProcesses() {
  int totalProcesses = ParseValueByKey<int>(kProcDirectory + kStatFilename, LinuxParser::filterProcesses);
  return totalProcesses;
}

int LinuxParser::RunningProcesses() {
  int runningProcesses = ParseValueByKey<int>(kProcDirectory + kStatFilename, filterProcsRunning);
  return runningProcesses;
}

//PROCESSES 
string LinuxParser::Command(const int pid) { 
  std::ifstream stream(kProcProcessDirectory + to_string(pid) + kCmdlineFilename); 
  string line;
  
  if (stream.is_open()) {
    std::getline(stream, line);
    stream.close();
  }

  //Limit to 40 characters
  line = line.substr(0, 39) += "...";
  return line;
}

//Used VmData instead of VmSize to account for physical RAM (instead of virtual RAM)
float LinuxParser::Ram(const int pid) {
  float ramKb = ParseValueByKey<float>(kProcProcessDirectory + to_string(pid) + kStatusFilename, filterVmData);
  float ramMb = KbToMb(ramKb);
  return ramMb;
 }

string LinuxParser::Uid(const int pid) { 
  const string uid = ParseValueByKey<string>(kProcProcessDirectory + to_string(pid) + kStatusFilename, filterUid);
  return uid;
 }

string LinuxParser::User(const int pid) {
  string username;
  string x;
  string uid;
  const string myuid = Uid(pid);

  std::ifstream stream(kPasswordPath);
  string line;

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
  std::ifstream stream(kProcProcessDirectory + to_string(pid) + kStatFilename);
  string line;

  if(stream.is_open()) {
    std::getline(stream, line);
    stream.close();
  }

  vector<string> procInfo = ParsedTokens(line);
  long int startTime;

  try {
    startTime = stol(procInfo[21]);
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  //Convert from clock ticks to seconds
  startTime /= sysconf(_SC_CLK_TCK);

  long int upTime = LinuxParser::UpTime() - startTime;
  return upTime;
}