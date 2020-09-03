#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>
#include <iterator>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid) : pid_(pid) {
    SetCpuValues();
    cpu_utilization_ = CpuUtilization();
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    float Hertz = sysconf(_SC_CLK_TCK);
    //Determine total time spent
    float total_time = utime_ + stime_;

    //Include children process time
    total_time = total_time + cutime_ + cstime_;

    //Get total elapsed time in seconds
    float seconds = LinuxParser::UpTime()  - (starttime_ / Hertz);

    //Calculate CPU Utilization
    return (total_time / Hertz) / seconds;
 }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return this->cpu_utilization_ > a.cpu_utilization_;
}

//Store /proc/[pid]/stat values in vector
vector<string> Process::RetrieveCpuValues() {
    string line;
    std::ifstream stream(LinuxParser::kProcProcessDirectory + to_string(pid_) + LinuxParser::kStatFilename);

    if(stream.is_open()) {
        std::getline(stream, line);
    }
    //Split string into an array by whitespace
    istringstream buffer(line);
    istream_iterator<string> beg(buffer), end;

    vector<string> cpu_values(beg, end);
    return cpu_values;
}

void Process::SetCpuValues() {
    vector<string> cpu_values = RetrieveCpuValues();
    assert(!cpu_values.empty());
    utime_ = stof(cpu_values[13]);
    stime_ = stof(cpu_values[14]);
    cutime_ = stof(cpu_values[15]);
    cstime_ = stof(cpu_values[16]);
    starttime_ = stof(cpu_values[21]);
}