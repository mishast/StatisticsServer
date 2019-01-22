#pragma once
#include <map>
#include <set>
#include <mutex>
#include <vector>

using delay_t = int32_t;
using percent_t = double;
using count_t = int64_t;

struct ShortStatRow
{
    percent_t   Percent;
    delay_t     Delay;
};

struct ShortStat
{
    ShortStat();

    delay_t                     Min;
    std::vector<ShortStatRow>   Table;
};

struct LongStatRow
{
    delay_t     Delay;
    count_t     Count;
    percent_t   Weight;
    percent_t   Percent;
};

struct EventStatistic
{
    EventStatistic();

    std::map<delay_t, count_t>  Statistics;
    count_t                     Total;
};

class StatisticsDB
{
public:
    void AddDelay(const std::string& _event, const delay_t _delay);

    std::vector<std::string>    GetEvents();
    ShortStat                   GetDelayStatShort(const std::string& _event);
    std::vector<LongStatRow>    GetDelayStatLong(const std::string& _event);

protected:
    std::mutex                      m_mutex;
    std::map<std::string, EventStatistic>  m_eventStatistics;
};
