#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "util.h"
#include "SysInfo.h"
#include "ProcessContainer.h"

using namespace std;

char * getCString(string str)
{
    char * cstr = new char[str.length() + 1]();
    strcpy(cstr, str.c_str());
    return cstr;
}

void writeSysInfoToConsole(SysInfo sys, WINDOW * sys_win)
{
    sys.setAttributes();

    mvwprintw(sys_win, 2, 2, getCString(("OS: " + sys.getOsName())));
    mvwprintw(sys_win, 3, 2, getCString(("Kernel version: " + sys.getKernelVersion())));
    mvwprintw(sys_win, 5, 2, getCString(("CPU:    ")));
    wattron(sys_win, COLOR_PAIR(2));
    wprintw(sys_win, getCString(Util::getProgressBar(sys.getCpuPercent())));
    wattron(sys_win, COLOR_PAIR(2));
    mvwprintw(sys_win, 6, 2, getCString(("Other cores:")));
    wattron(sys_win, COLOR_PAIR(2));
    vector<string> val = sys.getCoresStats();

    for(int i = 0; i < val.size(); i++)
    {
        mvwprintw(sys_win, (7 + i), 2, getCString(val[i]));
    }

    wattron(sys_win, COLOR_PAIR(2));
    mvwprintw(sys_win, 11 + 6, 2, getCString(("Memory: ")));
    wattron(sys_win, COLOR_PAIR(2));
    wprintw(sys_win, getCString(Util::getProgressBar(sys.getMemPercent())));
    wattron(sys_win, COLOR_PAIR(2));
    mvwprintw(sys_win, 12 + 6, 2, getCString(("Total Processes: " + sys.getTotalProc())));
    mvwprintw(sys_win, 13 + 6, 2, getCString(("Runing Processes: " + sys.getRunningProc())));
    mvwprintw(sys_win, 14 + 6, 2, getCString(("Up Time: " + Util::convertToTime(sys.getUpTime()))));
    wrefresh(sys_win);
}

void getProcessListToConsole(std::vector<string> processes, WINDOW *win) {
  wattron(win, COLOR_PAIR(2));
  mvwprintw(win, 1, 2, "PID:");
  mvwprintw(win, 1, 12, "User:");
  mvwprintw(win, 1, 22, "CPU[%%]:  ");
  mvwprintw(win, 1, 31, "RAM[MB]:");
  mvwprintw(win, 1, 40, "Uptime:");
  mvwprintw(win, 1, 49, "CMD:");
  wattroff(win, COLOR_PAIR(2));
  
  for (int i = 0; i < processes.size(); i++) {
    mvwprintw(win, 2 + i, 2, getCString(processes[i]));
  }
}

void printMain(SysInfo sys, ProcessContainer procs)
{
    // Start curses mode
    initscr();
    // Not printing input values
    noecho();
    // Terminating on classic ctrl + c
    cbreak();
    // Enabling color change of text
    start_color();
    int yMax, xMax;
    // Get size of window measured in lines and columns (column one char length)
    getmaxyx(stdscr, yMax, xMax);

    WINDOW * sys_win = newwin(25, xMax - 1, 0, 0);
    WINDOW * proc_win = newwin(15, xMax - 1, 26, 0);

    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    int counter = 0;

    while(1)
    {
        try
        {
            box(sys_win, 0, 0);
            box(proc_win, 0, 0);
            procs.refreshList();
            vector<vector<string>> processes = procs.getList();
            writeSysInfoToConsole(sys, sys_win);
            getProcessListToConsole(processes[counter], proc_win);
            wrefresh(sys_win);
            wrefresh(proc_win);
            refresh();
            sleep(1);
            if(counter == (processes.size() - 1))
            {
                counter = 0;
            }
            else
            {
                counter++;
            }
        }
        catch (...) {};
    }
    endwin();
}


int main(int argc, char ** argv)
{
    // Object which contains list of current processes, Container for Process Class
    ProcessContainer procs;
    // Object which contains relevant methods and attributes regarding system details
    SysInfo sys;
    // Display the information
    printMain(sys, procs);
    return 0;
}


// Below are for debugging

    // string pid = "5509";

    // std::cout << "\033[0;32mTest getVmSize\033[0m\n";
    // std::cout << ProcessParser::getVmSize(pid) << "\n";

    // std::cout << "\033[0;32mTest getCpuPercent\033[0m\n";
    // std::cout << ProcessParser::getCpuPercent(pid) << "\n";

    // std::cout << "\033[0;32mTest getProcUser\033[0m\n";
    // std::cout << ProcessParser::getProcUser(pid) << "\n";

    // std::cout << "\033[0;32mTest getCmd\033[0m\n";
    // std::cout << ProcessParser::getCmd(pid) << "\n";

    // std::cout << "\033[0;32mTest getNumberOfCores\033[0m\n";
    // std::cout << ProcessParser::getNumberOfCores() << "\n";

    // std::cout << "\033[0;32mTest getSysRamPercent\033[0m\n";
    // std::cout << ProcessParser::getSysRamPercent() << "\n";

    // std::cout << "\033[0;32mTest getOSName\033[0m\n";
    // std::cout << ProcessParser::getOSName() << "\n";

    // std::cout << "\033[0;32mTest getTotalThreads\033[0m\n";
    // std::cout << ProcessParser::getTotalThreads() << "\n";

    // std::cout << "\033[0;32mTest getTotalNumberOfProcesses\033[0m\n";
    // std::cout << ProcessParser::getTotalNumberOfProcesses() << "\n";

    // // std::cout << "Test getPidList\n";
    // // unsigned int counter = 0;
    // // for(auto l : ProcessParser::getPidList())
    // // {
    // //     std::cout << l << " ";
    // //     if(counter == 10)
    // //     {
    // //         std:cout << "\n";
    // //         counter = 0;
    // //     }
    // //     counter++;
    // // }

    // std::cout << "Works!" << std::endl;
