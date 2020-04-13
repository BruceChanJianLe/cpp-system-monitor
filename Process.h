#include <string>
#include "ProcessParser.h"

using namespace std;


class Process
{
    private:
        string pid;
        string user;
        string cmd;
        string cpu;
        string mem;
        string upTime;

    public:
        Process(string pid)
        {
            this->pid = pid;
            this->user = ProcessParser::getProcUser(pid);
            this->mem = ProcessParser::getVmSize(pid);
            this->cmd = ProcessParser::getCmd(pid);
            this->upTime = ProcessParser::getProcUpTime(pid);
            this->cpu = ProcessParser::getCpuPercent(pid);
        }
        void setPid(int pid);
        string getPid() const;
        string getUser() const;
        string getCmd() const;
        int getCpu() const;
        int getMem() const;
        string getUpTime() const;
        string getProcess();
        string formatString(string str, int len);
};

void Process::setPid(int pid)
{
    this->pid = pid;
}

string Process::getPid() const
{
    return this->pid;
}

string Process::getUser() const
{
    return this->user;
}

string Process::getCmd() const
{
    return this->getCmd();
}

int Process::getCpu() const
{
    return stoi(this->cpu);
}

int Process::getMem() const
{
    return stoi(this->mem);
}

string Process::getProcess()
{
    if(!ProcessParser::isPidExisting(this->pid))
    {
        return "";
    }
    this->mem = ProcessParser::getVmSize(pid);
    this->upTime = ProcessParser::getProcUpTime(pid);
    this->cpu = ProcessParser::getCpuPercent(pid);

    return (formatString(this->pid, 8) +
            formatString(this->user, 8) +
            formatString(this->cpu.substr(0, 5), 7) +
            formatString(this->mem.substr(0, 5), 7) +
            formatString(this->upTime.substr(0, 5), 7) +
            this->cmd.substr(0, 25) +
            "..."
            );
}

string Process::formatString(string str, int len)
{
    str += "            ";
    return str.substr(0, len) + "  ";
}