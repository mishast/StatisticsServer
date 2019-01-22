#pragma once
#include <vector>
#include <string>
#include <thread>
#include "statistics-db.h"

class UdpThread
{
public:
    UdpThread(StatisticsDB* _statisticsDb);
    void Start(uint16_t _port);
    void Stop();
private:

    std::thread     m_thread;
    uint16_t        m_port;
    int             m_socket;
    bool            m_stop;

    StatisticsDB    *m_statisticsDb;

    void MainLoop();
};

