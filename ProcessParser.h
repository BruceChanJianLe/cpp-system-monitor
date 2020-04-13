#ifndef _PP_H_
#define _PP_H_

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "util.h"

using namespace std;


class ProcessParser
{
    private:
        std::ifstream stream;

    public:
        static string getCmd(string pid);
        static vector<string> getPidList();
        static string getVmSize(string pid);
        static string getCpuPercent(string pid);
        static long int getSysUpTime();
        static string getProcUpTime(string pid);
        static string getProcUser(string pid);
        static vector<string> getSysCpuPercent(string coreNumber);
        static float getSysActiveCpuTime(vector<string> values);
        static float getSysIdleCpuTime(vector<string> value);
        static float getSysRamPercent();
        static string getSysKernelVersion();
        static int getNumberOfCores();
        static int getTotalThreads();
        static int getTotalNumberOfProcesses();
        static int getNumberOfRunningProcesses();
        static string getOSName();
        static string PrintCpuStats(vector<string> values1, vector<string> values2);
        static bool isPidExisting(string pid);
};

string ProcessParser::getCmd(string pid)
{
    string line;
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
    getline(stream, line);
    return line;
}

vector<string> ProcessParser::getPidList()
{
    DIR * dir;
    vector<string> list;
    if(!(dir = opendir("/proc")))
    {
        throw std::runtime_error(strerror(errno));
    }

    while(dirent * dirp = readdir(dir))
    {
        if((dirp->d_type != DT_DIR))
        {
            continue;
            // throw runtime_error("Not a directory!");
        }
        if(all_of(dirp->d_name, dirp->d_name + strlen(dirp->d_name), [](const char c){return isdigit(c);}))
        {
            list.push_back(dirp->d_name);
        }
    }
    if(closedir(dir))
    {
        throw runtime_error(strerror(errno));
    }
    return list;
}

string ProcessParser::getVmSize(string pid)
{
    string line;
    string name = "VmData";
    float result;

    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());

    // Loop through each line in the input stream
    while(getline(stream, line))
    {
        // Validate if input line has VmData at the first 6 characters
        if(line.compare(0, name.size(), name) == 0)
        {
            // Input string stream from that line inside buf variable
            // Get the begining and end of input stream iterator (pointer)
            // Pass it to vector of string call values
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            // Convert from KB to GB
            // if you `cat /proc/5307/status | grep VmData`
            // VmData:   550176 kB
            //   [0]       [1]  [2]
            result = (stof(values[1]) / float(1024));
            
            // For Debug
            // for(auto value : values)
            // {
            //     cout << value << "\n";
            // }
            break;
        }
    }
    return to_string(result);
}

string ProcessParser::getCpuPercent(string pid)
{
    string line;
    ifstream stream = Util::getStream(Path::basePath() + pid + "/" + Path::statPath());
    // Since it is just one line from file, we do not need a while loop

    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime / freq);
    float result = 100.0 * ((total_time / freq) / seconds);
    return to_string(result);
}


long int ProcessParser::getSysUpTime()
{
    string line;
    ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    return stoi(values[0]);
}

string ProcessParser::getProcUpTime(string pid)
{
    // To store a line from file stream
    string line;

    // Load input file stream
    ifstream stream = Util::getStream(Path::basePath() + pid + "/" + Path::statPath());

    // Get line from input file stream
    getline(stream, line);

    // Load line to input string stream (nameing it buf)
    istringstream buffallo(line);

    // Input string iterator for beginning and end
    istream_iterator<string> begining(buffallo), end;

    // Pass the input string iterator to a vector of string
    vector<string> values(begining, end);

    return to_string(float(stof(values[13]) / sysconf(_SC_CLK_TCK)));
}


string ProcessParser::getProcUser(string pid)
{
    string line;
    string identifier = "Uid:";
    string id_no;
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    while (getline(stream, line))
    {
        if(line.compare(0, identifier.size(), identifier) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            id_no = values[1];
            break;
        }
    }

    id_no = "x:" + id_no;
    stream = Util::getStream("/etc/passwd");
    while(getline(stream, line))
    {
        if(line.find(id_no) != string::npos)
        {
            return line.substr(0, line.find(":"));
        }
    }

    return "";
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber = "")
{
    string line;
    string name = "cpu" + coreNumber;
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return values;
        }
    }
    return vector<string> {};
}

float ProcessParser::getSysActiveCpuTime(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float ProcessParser::getSysIdleCpuTime(vector<string> values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

float ProcessParser::getSysRamPercent()
{
    string line;
    ifstream stream = Util::getStream(Path::basePath() + Path::memInfoPath());

    // Identifier
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";

    // Store the Identifier values
    float totalMem, freeMem, buffers;
    totalMem = freeMem = buffers = 0;

    while (getline(stream, line))
    {
        if(line.compare(0, name1.size(), name1) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            totalMem = stof(values[1]);
        }
        if(line.compare(0, name2.size(), name2) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            freeMem = stof(values[1]);
        }
        if(line.compare(0, name3.size(), name3) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            buffers = stof(values[1]);
        }
        if(totalMem != 0 && freeMem != 0 && buffers != 0)
        {
            break;
        }
    }
    return float( 100.0 * (1 - (freeMem / (totalMem - buffers))));
}

string ProcessParser::getSysKernelVersion()
{
    string line;
    // string name = "Linux Version ";
    ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());
    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    return values[2];
}

int ProcessParser::getNumberOfCores()
{
    // Obtain the number of cpu cores of the host
    string line;
    string name = "cpu cores";
    ifstream stream = Util::getStream(Path::basePath() + "cpuinfo");
    while (getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return stoi(values[3]);
        }
    }
    return 0;
}

int ProcessParser::getTotalThreads()
{
    vector<string> list = getPidList();
    string line;
    int result = 0;
    string name = "Threads:";

    for(auto l : list)
    {
        ifstream stream = Util::getStream(Path::basePath() + l + Path::statusPath());
        while(getline(stream, line))
        {
            if(line.compare(0, name.size(), name) == 0)
            {
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                result += stoi(values[1]);
                break;
            }
        }
    }
    return result;
}

int ProcessParser::getTotalNumberOfProcesses()
{
    string line;
    string name = "processes";
    int res;
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while (getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            res += stoi(values[1]);
        }
    }
    return res;
}

int ProcessParser::getNumberOfRunningProcesses()
{
    string line;
    int res = 0;
    string name = "procs_running";
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            res += stoi(values[1]);
            break;
        }
    }
    return res;
}

string ProcessParser::getOSName()
{
    string line;
    string name = "PRETTY_NAME=";
    ifstream stream = Util::getStream("/etc/os-release");
    while(getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            auto found = line.find("=");
            line = line.substr(++found);
            line.erase(remove(line.begin(), line.end(), '"'), line.end());
            return line;
        }
    }
    return line = "";
}

string ProcessParser::PrintCpuStats(vector<string> values1, vector<string> values2)
{
    //  CPU stats can only be calculated within an given time interval
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    return to_string(100 * (activeTime / totalTime));
}

bool ProcessParser::isPidExisting(string pid)
{
    vector<string> pidList = ProcessParser::getPidList();

    if(find(pidList.begin(), pidList.end(), pid) != pidList.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}
#endif