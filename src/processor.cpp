#include "processor.h"
#include "linux_parser.h"

#include <unistd.h>
#include <string>
#include <map>
#include <assert.h>
#include <iomanip>


using namespace std;
map<string, int> MapUtilizationTimes();
float CalculateUtilization(map<string, int> prev_map, map<string, int> map);

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    //store prev and current values
    map<std::string, int> prev_map = MapUtilizationTimes();
    sleep(1.00);
    map<std::string, int> map = MapUtilizationTimes();

    //calculate CPU usage percentage
    return CalculateUtilization(prev_map, map);
}

//Parse line into a hash table with processor info
map<string, int> MapUtilizationTimes() {
    map<string, int> result;

    string line, cpu;
    int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    if (stream.is_open()) {
        getline(stream, line);
        std::istringstream linestream(line);
        linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    }
    
    result["user"] = user;
    result["nice"] = nice;
    result["system"] = system;
    result["idle"] = idle;
    result["iowait"] = iowait;
    result["irq"] = irq;
    result["softirq"] = softirq;
    result["steal"] = steal;
    result["guest"] = guest;
    result["guest_nice"] = guest_nice;

    return result;
}

//Calculate CPU usage percentage
float CalculateUtilization(map<string, int> prev_map, map<string, int> map) {
    float previdle = prev_map["idle"] + prev_map["iowait"];
    float idle = map["idle"] + map["iowait"];

    float prevnonidle = prev_map["user"] + prev_map["nice"] + prev_map["system"] + prev_map["irq"] + prev_map["softirq"] + prev_map["steal"];
    float nonidle = map["user"] + map["nice"] + map["system"] + map["irq"] + map["softirq"] + map["steal"] + map["guest"] + map["guest_nice"];

    float prevtotal = previdle + prevnonidle;
    float total = idle + nonidle;

    //Change in active time / change in total time
    float activetime = prevnonidle - nonidle;
    float const idletime = prevtotal - total;
    float const totaltime = activetime + idletime;

    return activetime / totaltime;
}