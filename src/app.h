#include <vector>
#include <string>
#include "statistics-db.h"
#include "udp-thread.h"
#include "signal-thread.h"

class App
{
public:
    App();
    int Run(const std::vector<std::string>& _files, const std::string& _dumpFileName, uint16_t _port);

protected:
    StatisticsDB    m_statisticsDb;
    UdpThread       m_udpThread;
    SignalThread    m_signalThread;

    std::string     m_dumpFileName;

    void            DumpLongStatistics(std::ostream& stream);

    std::vector<std::string> Split(const std::string& s, char delimiter);
};

