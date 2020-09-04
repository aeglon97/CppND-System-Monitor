#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

using namespace std;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(const int pid);
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  float Ram();
  long int UpTime();
  bool operator<(Process const& a) const; 
  vector<string> CpuValues();

  //mutators
  void SetPid(int pid);
  void SetCpuValues();

 private:
  int pid_;

  //CPU utilization calculation
  float uTime_;
  float sTime_;
  float cuTime_;
  float csTime_;
  float startTime_;
  float cpuUtilization_;
};

#endif