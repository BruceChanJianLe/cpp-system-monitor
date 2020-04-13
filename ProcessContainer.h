#include <string>
#include <vector>
#include "Process.h"

using namespace std;


class ProcessContainer
{
    private:
        vector<Process> _list;
    
    public:
        ProcessContainer()
        {
            this->refreshList();
        }
        void refreshList();
        string printList();
        vector<vector<string>> getList();
};

void ProcessContainer::refreshList()
{
    vector<string> pids = ProcessParser::getPidList();
    this->_list.clear();
    for(auto pid : pids)
    {
        Process proc(pid);
        this->_list.push_back(proc);
    }
}

string ProcessContainer::printList()
{
    string result = "";
    for(auto l : _list)
    {
        result += l.getProcess();
    }
    return result;
}

// vector<string> ProcessContainer::getList()
// {
//     vector<string> values;
//     for(int i = (this->_list.size() - 10); i < this->_list.size(); i++)
//     {
//         values.push_back(this->_list[i].getProcess());
//     }
//     return values;
// }

std::vector<std::vector<std::string>> ProcessContainer::getList() {
  std::vector<std::vector<std::string>> values;
  std::vector<std::string> stringifiedList;

  for (int i = 0; i < ProcessContainer::_list.size(); i++) {
    stringifiedList.push_back(ProcessContainer::_list[i].getProcess());
  }

  int lastIndex = 0;

  for (int i = 0; i < stringifiedList.size(); i++) {
    if (i % 10 == 0 && i > 0) {
      std::vector<std::string> sub(&stringifiedList[i - 10],
                                   &stringifiedList[i]);
      values.push_back(sub);
      lastIndex = i;
    }
    if (i == (ProcessContainer::_list.size() - 1) && (i - lastIndex) < 10) {
      std::vector<std::string> sub(&stringifiedList[lastIndex],
                                   &stringifiedList[i + 1]);
      values.push_back(sub);
    }
  }
  return values;
}