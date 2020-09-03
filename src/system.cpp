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

//Update and sort container of system's processes
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

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return  LinuxParser::OperatingSystem();}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }