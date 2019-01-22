#include <thread>
#include <assert.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "signal-thread.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>

SignalThread::SignalThread(StatisticsDB* _statisticsDb)
{
    m_stop = false;
    m_dump = false;

    m_statisticsDb = _statisticsDb;
}

void SignalThread::Start(const std::string& _dumpFileName)
{
    m_dumpFileName = _dumpFileName;

    m_thread = std::thread([this]{ this->MainLoop(); });
}

void SignalThread::Stop()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
        m_condVar.notify_one();
    }

    if (m_thread.joinable())
        m_thread.join();

    m_stop = false;
}

void SignalThread::DumpStatistics()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_dump = true;
    m_condVar.notify_one();
}

void SignalThread::DumpLongStatisticsToStream(std::ostream& stream)
{
    auto events = m_statisticsDb->GetEvents();

    for (size_t i = 0; i < events.size(); i++)
    {

        auto shortStat = m_statisticsDb->GetDelayStatShort(events[i]);
        auto longStat = m_statisticsDb->GetDelayStatLong(events[i]);

        stream << std::setprecision(2) << std::fixed;

        stream << events[i]
           << " min=" << shortStat.Min
           << " 50%=" << shortStat.Table[0].Delay
           << " 90%=" << shortStat.Table[1].Delay
           << " 99%=" << shortStat.Table[2].Delay
           << " 99.9%=" << shortStat.Table[3].Delay
           << "\n";

        stream << "ExecTime\tTransNo\tWeight,%%\tPercent\n";
        for (size_t r = 0; r < longStat.size(); r++)
        {
            stream << longStat[r].Delay << "\t" << longStat[r].Count << "\t" << (longStat[r].Weight * 100) << "\t" << (longStat[r].Percent * 100) << "\n";
        }
    }
}

void SignalThread::MainLoop()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_dump && !m_stop)
        {
            m_condVar.wait(lock);
        }

        if (m_stop)
        {
            return;
        }

        if (m_dump)
        {
            if (!m_dumpFileName.empty())
            {
                std::ofstream file(m_dumpFileName);
                DumpLongStatisticsToStream(file);
            }
            else
            {
                DumpLongStatisticsToStream(std::cout);

                std::cout << std::flush;
            }

            m_dump = false;
        }
    }
}

