#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include <curses.h>

#include "ncurses_display.h"
#include "format.h"
#include "system.h"

using std::string;
using std::to_string;

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(const float percent) {
  std::string result{"0%"};
  int size{50};
  float bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  string display{to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = " " + to_string(percent * 100).substr(0, 3);
  return result + " " + display + "/100%";
}

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{0};
  mvwprintw(window, ++row, 2, ("OS: " + system.OperatingSystem()).c_str());
  mvwprintw(window, ++row, 2, ("Kernel: " + system.Kernel()).c_str());
  mvwprintw(window, ++row, 2, "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.Cpu().Utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2,
            ("Total Processes: " + to_string(system.TotalProcesses())).c_str());
  mvwprintw(
      window, ++row, 2,
      ("Running Processes: " + to_string(system.RunningProcesses())).c_str());
  mvwprintw(window, ++row, 2,
            ("Up Time: " + Format::ElapsedTime(system.UpTime())).c_str());
  wrefresh(window);
}

void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
                                      WINDOW* window, int n) {
  int row{0};
  int const pidColumn{2};
  int const userColumn{9};
  int const cpuColumn{16};
  int const ramColumn{26};
  int const timeColumn{35};
  int const commandColumn{46};
  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, ++row, pidColumn, "PID");
  mvwprintw(window, row, userColumn, "USER");
  mvwprintw(window, row, cpuColumn, "CPU[%%]");
  mvwprintw(window, row, ramColumn, "RAM[MB]");
  mvwprintw(window, row, timeColumn, "TIME+");
  mvwprintw(window, row, commandColumn, "COMMAND");
  wattroff(window, COLOR_PAIR(2));
  for (int i = 0; i < n; ++i) {
    mvwprintw(window, ++row, pidColumn, to_string(processes[i].Pid()).c_str());
    mvwprintw(window, row, userColumn, processes[i].User().c_str());
    float cpu = processes[i].CpuUtilization() * 100;
    mvwprintw(window, row, cpuColumn, to_string(cpu).substr(0, 4).c_str());
    mvwprintw(window, row, ramColumn, Format::RoundedFloat(processes[i].Ram()).c_str());
    mvwprintw(window, row, timeColumn,
              Format::ElapsedTime(processes[i].UpTime()).c_str());
    mvwprintw(window, row, commandColumn,
              processes[i].Command().substr(0, window->_maxx - 46).c_str());
  }
}

void NCursesDisplay::Display(System& system, int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color

  int xMax{getmaxx(stdscr)};
  WINDOW* systemWindow = newwin(9, xMax - 1, 0, 0);
  WINDOW* processWindow =
      newwin(3 + n, xMax - 1, systemWindow->_maxy + 1, 0);

  while (1) {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(systemWindow, 0, 0);
    box(processWindow, 0, 0);
    DisplaySystem(system, systemWindow);
    DisplayProcesses(system.Processes(), processWindow, n);
    wrefresh(systemWindow);
    wrefresh(processWindow);
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  endwin();
}