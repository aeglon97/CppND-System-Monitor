#ifndef PROCESSOR_H
#define PROCESSOR_H

#include<map>
#include<string>

using namespace std;

class Processor {
 public:
  float Utilization();
  float CpuUtilization(const map<string, int> prevMap, const map<string, int> map);

 private:
};

#endif