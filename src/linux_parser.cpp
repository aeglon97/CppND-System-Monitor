#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <map>
#include <iomanip>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::map;

//Helper function to parse file for key-value pairs
string ParseAndRetrieve (std::string path, std::string myKey) {
  string line, key, value;
  std::ifstream filestream(path); 
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == myKey) {
            return value;
        }
      }
    }
  }
  return value;
}

//

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line, key, value;
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
  string os, version, kernel;
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
  std::random_shuffle(pids.begin(), pids.end());
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, value, kb;
  float memtotal, memfree, memutil;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (memtotal != 0 && memfree != 0) {
          memutil = (memtotal - memfree) / memtotal;
        }
        if(key == "MemTotal") {
          memtotal = stof(value);
        }
        if(key == "MemFree") {
          memfree = stof(value);
        }
      }
    }
  }
  return memutil; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }
  return stol(uptime);
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return std::stoi(ParseAndRetrieve(kProcDirectory + kStatFilename, "processes"));
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return std::stoi(ParseAndRetrieve(kProcDirectory + kStatFilename, "procs_running"));
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcProcessDirectory + to_string(pid) + kCmdlineFilename); 
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
    }
  return line;
}
// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string ram = ParseAndRetrieve(kProcProcessDirectory + to_string(pid) + kStatusFilename, "VmSize");
  return to_string(stof(ram) / 100);
 }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  const string uid = ParseAndRetrieve(kProcProcessDirectory + to_string(pid) + kStatusFilename, "Uid ");
  return uid;
 }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, username, uid;
  const string myuid = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> uid) {
        if (myuid == uid) { return username; }
      }
    }
  }
  return username;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  map<string, int> mymap;
  string line;
  long uptime{0};
  long counter = 0;
  std::ifstream stream(kProcProcessDirectory + to_string(pid) + kStatFilename);

  if(stream.is_open()) {
    string token;
    while(std::getline(stream, token, ' ')) {
      mymap[token]++;
      counter ++;
      if (counter == 22) {
        uptime = stol(token);
        break;
      }
    }
  }

  assert(!mymap.empty());
  uptime /= sysconf(_SC_CLK_TCK);

  return uptime;
  
}