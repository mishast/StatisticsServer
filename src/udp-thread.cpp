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
#include "udp-thread.h"
#include <sstream>

UdpThread::UdpThread(StatisticsDB* _statisticsDb)
{
    m_stop = false;

    m_statisticsDb = _statisticsDb;
}

void UdpThread::Start(uint16_t _port)
{
    m_port = _port;

    m_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (m_socket < 0)
    {
            perror("ERROR opening socket");
            return;
    }

    struct sockaddr_in srv;

    srv.sin_family = AF_INET;
    srv.sin_port = htons(m_port);
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(m_socket, (struct sockaddr*) &srv, sizeof(srv)) < 0){
        perror("ERROR on binding");
        close(m_socket);
        exit(1);
    }

    m_thread = std::thread([this]{ this->MainLoop(); });
}

void UdpThread::Stop()
{
    m_stop = true;

    if (m_thread.joinable())
        m_thread.join();

    m_stop = false;
}

#define BUF_LEN 1024

void UdpThread::MainLoop()
{
    fd_set read_fds;
    timeval tv;

    while (!m_stop)
    {
        tv.tv_sec = 1;

        FD_ZERO(&read_fds);
        FD_SET(m_socket, &read_fds);

        int n = select(m_socket + 1, &read_fds, nullptr, nullptr, &tv);

        if (n == 0)
        {
            if (m_stop)
            {
                close(m_socket);
                return;
            }
        }
        else if (n < 0)
        {
            perror("ERROR Server : select()\n");
            close(m_socket);
            return;
        }
        else
        {
            if(FD_ISSET(m_socket, &read_fds))
            {
                char buf[BUF_LEN];
                struct sockaddr_in cli_addr;
                socklen_t clilen = sizeof(cli_addr);

                memset(buf, 0, BUF_LEN);

                ssize_t received = recvfrom(m_socket, buf, BUF_LEN - 1, 0, (struct sockaddr*)&cli_addr, &clilen);
                if (received < 0)
                {
                    perror("ERROR in recvfrom()");
                    close(m_socket);
                    return;
                }

                size_t len = strlen(buf);

                for (size_t i = 0; i < len; i++)
                {
                    if (buf[i] == '\n') buf[i] = '\0';
                }

                std::string event(buf);

                auto statistics = m_statisticsDb->GetDelayStatShort(event);

                std::stringstream ss;

                ss << event
                   << " min=" << statistics.Min
                   << " 50%=" << statistics.Table[0].Delay
                   << " 90%=" << statistics.Table[1].Delay
                   << " 99%=" << statistics.Table[2].Delay
                   << " 99.9%=" << statistics.Table[3].Delay;

                std::string reply = ss.str();

                size_t replysize = reply.size();

                sendto(m_socket, reply.c_str(), replysize, 0, (struct sockaddr *)&cli_addr, clilen);
            }
        }
    }
}

