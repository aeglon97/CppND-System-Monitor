#include <cstddef>
#include <string>
#include <vector>
#include <set>

#include <unistd.h>

#include "linux_parser.h"
#include "processor.h"
#include "process.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

//Set system processes upon initialization
System::System() {
    SetProcesses();
}

Processor& System::Cpu() { return cpu_; }

//Update and sort container of processes
void System::SetProcesses() {
    for (int pid : LinuxParser::Pids()) {
        processes_.push_back(Process(pid));
    }
    std::sort(processes_.begin(), processes_.end());
}

//Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    return processes_;
}

//Return the system's kernel identifier
std::string System::Kernel() { return LinuxParser::Kernel(); }

//Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

//Return the operating system name
std::string System::OperatingSystem() { return  LinuxParser::OperatingSystem();}

//Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

//Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

//Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }