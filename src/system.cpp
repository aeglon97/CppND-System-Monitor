#include <cstddef>
#include <string>
#include <vector>
#include <set>

#include <unistd.h>

#include "linux_parser.h"
#include "processor.h"
#include "process.h"
#include "system.h"

using namespace std;

//Set system processes upon initialization
System::System() {
    System::SetProcesses();
}

Processor& System::Cpu() { return cpu_; }

//Update and sort container of system's processes
void System::SetProcesses() {
    for (const int &pid : LinuxParser::Pids()) {
        processes_.emplace_back(pid);
    }
    sort(processes_.begin(), processes_.end());
}

//Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    return processes_;
}

string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

string System::OperatingSystem() { return  LinuxParser::OperatingSystem();}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }