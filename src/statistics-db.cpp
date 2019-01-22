#include "statistics-db.h"
#include <deque>

ShortStat::ShortStat()
{
    Min = 0;

    std::vector<double> percents = {0.5, 0.9, 0.99, 0.999};

    for (size_t i = 0; i < percents.size(); i++)
    {
        ShortStatRow row;

        row.Percent = 0.5;
        row.Delay = 0;

        Table.push_back(std::move(row));
    }
}

EventStatistic::EventStatistic()
    : Total(0)
{
}

void StatisticsDB::AddDelay(const std::string& _event, const delay_t _delay)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto& eventStatistic = m_eventStatistics[_event];

    eventStatistic.Statistics[_delay]++;
    eventStatistic.Total++;
}

std::vector<std::string>  StatisticsDB::GetEvents()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<std::string> result;

    for(auto& pair: m_eventStatistics)
    {
        result.push_back(pair.first);
    }

    return result;
}

ShortStat StatisticsDB::GetDelayStatShort(const std::string& _event)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    ShortStat result;

    auto eventStatisticIt = m_eventStatistics.find(_event);

    if (eventStatisticIt == m_eventStatistics.end())
    {
        return ShortStat();
    }

    auto& eventStatistic = eventStatisticIt->second;

    if (eventStatistic.Total == 0)
    {
        return ShortStat();
    }

    auto firstElement = eventStatistic.Statistics.begin();
    delay_t minDelay = firstElement->first;

    result.Min = minDelay;

    std::vector<double> percents = {0.5, 0.9, 0.99, 0.999};
    count_t currentCount = 0;
    size_t percentIdx = 0;
    for(auto& delayCountPair: eventStatistic.Statistics)
    {
        currentCount += delayCountPair.second;
        double currentPercent = static_cast<double>(currentCount) / eventStatistic.Total;

        while (percentIdx < percents.size())
        {
            double needPercent = percents[percentIdx];

            if (currentPercent >= needPercent)
            {
                ShortStatRow row;

                row.Percent = needPercent;
                row.Delay = delayCountPair.first;

                result.Table[percentIdx] = std::move(row);
                percentIdx++;

                continue;
            }

            break;
        }
    }

    return result;
}

std::vector<LongStatRow>  StatisticsDB::GetDelayStatLong(const std::string& _event)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<LongStatRow> result;

    auto& eventStatistic = m_eventStatistics[_event];

    if (eventStatistic.Total == 0)
    {
        LongStatRow row;

        row.Count = 0;
        row.Delay = 0;
        row.Weight = 1;
        row.Percent = 1;

        result.push_back(std::move(row));

        return result;
    }

    auto firstElement = eventStatistic.Statistics.begin();
    auto lastElement = eventStatistic.Statistics.rbegin();

    delay_t minDelay = firstElement->first;
    delay_t maxDelay = lastElement->first;
    delay_t minTableElement = (minDelay / 5) * 5;
    delay_t maxTableElement = (((maxDelay + 4) / 5) * 5);

    auto eventStatisticIt = eventStatistic.Statistics.begin();
    count_t stepCount = 0;
    count_t currentCount = 0;

    for (int tableDelay = minTableElement; tableDelay <= maxTableElement; tableDelay += 5)
    {
        stepCount = 0;
        while (eventStatisticIt != eventStatistic.Statistics.end() && eventStatisticIt->first < tableDelay)
        {
            stepCount += eventStatisticIt->second;
            eventStatisticIt++;
        }
        currentCount += stepCount;

        LongStatRow row;

        row.Count = stepCount;
        row.Delay = tableDelay;
        row.Weight = static_cast<double>(stepCount) / eventStatistic.Total;
        row.Percent = static_cast<double>(currentCount) / eventStatistic.Total;

        result.push_back(std::move(row));
    }

    return result;
}
