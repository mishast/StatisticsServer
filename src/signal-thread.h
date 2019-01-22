#pragma once
#include <vector>
#include <string>
#include <thread>
#include <condition_variable>
#include <sstream>
#include "statistics-db.h"

class SignalThread
{
public:
    SignalThread(StatisticsDB* _statisticsDb);
    void Start(const std::string& _dumpFileName);
    void Stop();
    void DumpStatistics();

private:

    std::mutex              m_mutex;
    std::condition_variable m_condVar;
    std::thread             m_thread;
    bool                    m_stop;
    bool                    m_dump;

    StatisticsDB            *m_statisticsDb;
    std::string             m_dumpFileName;

    void DumpLongStatisticsToStream(std::ostream& stream);
    void MainLoop();
};

