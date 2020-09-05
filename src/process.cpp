#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>

#include <unistd.h>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid) : pid_(pid) {
    Process::SetCpuValues();
    cpuUtilization_ = CpuUtilization();
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
    float hertz = sysconf(_SC_CLK_TCK);
    float totalTime = uTime_ + sTime_;

    //Include children process time
    totalTime = totalTime + cuTime_ + csTime_;

    //Get total elapsed time in seconds
    float seconds = LinuxParser::UpTime()  - (startTime_ / hertz);

    //Calculate CPU Utilization
    return (totalTime / hertz) / seconds;
 }

string Process::Command() { return LinuxParser::Command(pid_); }

float Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

//Sort processes by descending CPU utilization
bool Process::operator<(Process const& a) const { 
    return this->cpuUtilization_ > a.cpuUtilization_;
}

//Store /proc/[pid]/stat values in a vector
vector<string> Process::CpuValues() {
    std::ifstream stream(LinuxParser::kProcProcessDirectory + to_string(pid_) + LinuxParser::kStatFilename);
    string line;

    if(stream.is_open()) {
        std::getline(stream, line);
        stream.close();
    }

    vector<string> cpuValues = LinuxParser::ParsedTokens(line);
    return cpuValues;
}

//Set this process's CPU values upon initialization
void Process::SetCpuValues() {
    vector<string> cpuValues = Process::CpuValues();
    try {
        uTime_ = stof(cpuValues[13]);
        sTime_ = stof(cpuValues[14]);
        cuTime_ = stof(cpuValues[15]);
        csTime_ = stof(cpuValues[16]);
        startTime_ = stof(cpuValues[21]);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}