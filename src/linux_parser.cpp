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
  random_shuffle(pids.begin(), pids.end());
  return pids;
}

//Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, value, kb;
  float memtotal, memfree, memutil;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (memtotal && memfree) {
          memutil = (memtotal - memfree) / memtotal;
          return memutil;
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

//Read and return the system uptime
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
long LinuxParser::IdleJiffies() { return 0 ; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  return {}; 
}

//Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return std::stoi(ParseAndRetrieve(kProcDirectory + kStatFilename, "processes"));
}

//Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return std::stoi(ParseAndRetrieve(kProcDirectory + kStatFilename, "procs_running"));
}

//Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcProcessDirectory + to_string(pid) + kCmdlineFilename); 
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
    }
  return line;
}
//Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string ram_kb = ParseAndRetrieve(kProcProcessDirectory + to_string(pid) + kStatusFilename, "VmSize");

  //convert to megabytes
  float ram_mb = stof(ram_kb) / 1024;
  std::stringstream ram_mb_stream;

  //round to 2 decimal places
  ram_mb_stream << std::fixed << std::setprecision(2) << ram_mb;
  return ram_mb_stream.str();
 }

//Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  const string uid = ParseAndRetrieve(kProcProcessDirectory + to_string(pid) + kStatusFilename, "Uid ");
  return uid;
 }

//Read and return the user associated with a process
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

//Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) {
  long int uptime;
  int index = 0;

  string line;
  std::ifstream stream(kProcProcessDirectory + to_string(pid) + kStatFilename);
  
  if(stream.is_open()) {
    std::getline(stream, line);
    // string token;
    // while(std::getline(stream, token, ' ')) {
    //   index++;
    //   if (index == 22) {
    //     uptime = stol(token);
    //     break;
    //   }
    // }
  }

  std::istringstream buffer(line);
  std::istream_iterator<string> beg(buffer), end;
  vector<string> proc_stat_values(beg, end);

  uptime = stol(proc_stat_values[21]) / sysconf(_SC_CLK_TCK);
  // uptime /= sysconf(_SC_CLK_TCK);
  return uptime;
}
