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
    SetCpuValues();
    cpu_utilization_ = CpuUtilization();
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
    float Hertz = sysconf(_SC_CLK_TCK);
    float total_time = utime_ + stime_;

    //Include children process time
    total_time = total_time + cutime_ + cstime_;

    //Get total elapsed time in seconds
    float seconds = LinuxParser::UpTime()  - (starttime_ / Hertz);

    //Calculate CPU Utilization
    return (total_time / Hertz) / seconds;
 }

string Process::Command() { return LinuxParser::Command(pid_); }

float Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

//Sort processes by descending CPU utilization
bool Process::operator<(Process const& a) const { 
    return this->cpu_utilization_ > a.cpu_utilization_;
}

//Store /proc/[pid]/stat values in a vector
vector<string> Process::RetrieveCpuValues() {
    string line;
    std::ifstream stream(LinuxParser::kProcProcessDirectory + to_string(pid_) + LinuxParser::kStatFilename);

    if(stream.is_open()) {
        std::getline(stream, line);
        stream.close();
    }

    vector<string> cpu_values = LinuxParser::SplitLine(line);
    return cpu_values;
}

//Set this process's CPU values upon initialization
void Process::SetCpuValues() {
    vector<string> cpu_values = RetrieveCpuValues();
    utime_ = stof(cpu_values[13]);
    stime_ = stof(cpu_values[14]);
    cutime_ = stof(cpu_values[15]);
    cstime_ = stof(cpu_values[16]);
    starttime_ = stof(cpu_values[21]);
}