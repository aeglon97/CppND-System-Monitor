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
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  vector<string> RetrieveCpuValues();
  void SetCpuValues();

  //mutator
  void SetPid(int pid);

  // TODO: Declare any necessary private members
 private:
  int pid_;

  //CPU utilization calculation
  float utime_;
  float stime_;
  float cutime_;
  float cstime_;
  float starttime_;
};

#endif