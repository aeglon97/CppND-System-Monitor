#include <iomanip>
#include <string>
#include <map>

#include <unistd.h>

#include "linux_parser.h"
#include "processor.h"

using namespace std;

//Function definitions
map<string, int> MapProcInfo();

//Return the aggregate CPU utilization
float Processor::Utilization() { 
    map<std::string, int> prevMap = MapProcInfo();
    sleep(1.00);
    map<std::string, int> map = MapProcInfo();

    return Processor::CpuUtilization(prevMap, map);
}

//Parse line into a hash table with processor info
map<string, int> MapProcInfo() {
    map<string, int> result;
    string cpu;
    
    int user;
    int nice;
    int system;
    int idle;
    int ioWait;
    int irq;
    int softIrq;
    int steal;
    int guest;
    int guestNice;

    ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    string line;

    if (stream.is_open()) {
        getline(stream, line);
        std::istringstream linestream(line);
        linestream >> cpu >> user >> nice >> system >> idle >> ioWait >> irq >> softIrq >> steal >> guest >> guestNice;
        stream.close();
    }
    
    result["user"] = user;
    result["nice"] = nice;
    result["system"] = system;
    result["idle"] = idle;
    result["ioWait"] = ioWait;
    result["irq"] = irq;
    result["softIrq"] = softIrq;
    result["steal"] = steal;
    result["guest"] = guest;
    result["guestNice"] = guestNice;

    return result;
}

//Calculate CPU usage percentage
float Processor::CpuUtilization(map<string, int> prevMap, map<string, int> map) {
    float prevIdle = prevMap["idle"] + prevMap["ioWait"];
    float idle = map["idle"] + map["ioWait"];

    float prevNonIdle = prevMap["user"] + prevMap["nice"] + prevMap["system"] + prevMap["irq"] + prevMap["softIrq"] + prevMap["steal"];
    float nonIdle = map["user"] + map["nice"] + map["system"] + map["irq"] + map["softIrq"] + map["steal"] + map["guest"] + map["guestNice"];

    float prevTotal = prevIdle + prevNonIdle;
    float total = idle + nonIdle;

    //Change in active time / change in total time
    float deltaActiveTime = prevNonIdle - nonIdle;
    float const deltaIdleTime = prevTotal - total;
    float const deltaTotalTime = deltaActiveTime + deltaIdleTime;

    return deltaActiveTime / deltaTotalTime;
}