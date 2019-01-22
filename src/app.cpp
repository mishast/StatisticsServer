#include "app.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <signal.h>
#include <string.h>

extern SignalThread *signalThread;

SignalThread *signalThread = nullptr;

void sigusr_handler(int)
{
    signalThread->DumpStatistics();
}

App::App()
    : m_udpThread(&m_statisticsDb), m_signalThread(&m_statisticsDb)
{

}

int App::Run(const std::vector<std::string>& _files, const std::string& _dumpFileName, uint16_t _port)
{
    m_dumpFileName = _dumpFileName;

    m_signalThread.Start(_dumpFileName);
    m_udpThread.Start(_port);

    signalThread = &m_signalThread;
    signal(SIGUSR1, sigusr_handler);
    signal(SIGUSR2, sigusr_handler);

    for (size_t fileIdx = 0; fileIdx < _files.size(); fileIdx++)
    {
        std::ifstream file(_files[fileIdx]);
        std::string line;
        while (std::getline(file, line))
        {
            std::vector<std::string> fields = Split(line, '\t');

            if (fields.size() == 16)
            {
                std::string event = fields[1];
                std::string delayString = fields[15];

                if ( !std::all_of(delayString.begin(), delayString.end(), [](char c){return isdigit(c);}) )
                {
                    continue;
                }

                int delay = atoi(delayString.c_str());

                m_statisticsDb.AddDelay(event, delay);
            }
        }
    }

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(600));
    }

    m_udpThread.Stop();
    m_signalThread.Stop();
}



std::vector<std::string> App::Split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}
