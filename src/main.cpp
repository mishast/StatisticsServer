#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include "app.h"

int main(int argc, char** argv)
{
    std::vector<std::string> files;
    std::string              dumpFileName;
    uint16_t                 port = 0;

    printf("Statistics Server\n");

    bool fileArg = false;
    bool dumpFileNameArg = false;
    bool portArg = false;

    for (int i = 1; i < argc; i++)
    {
        if (fileArg == true)
        {
            files.push_back(argv[i]);
            fileArg = false;
        }
        else if (dumpFileNameArg)
        {
            dumpFileName = argv[i];
            dumpFileNameArg = false;
        }
        else if (portArg)
        {
            port = static_cast<uint16_t>(atoi(argv[i]));
            portArg = false;
        }
        else
        {
            if (strcmp(argv[i], "-f") == 0)
            {
                fileArg = true;
            }
            else if (strcmp(argv[i], "-d") == 0)
            {
                dumpFileNameArg = true;
            }
            else if (strcmp(argv[i], "-p") == 0)
            {
                portArg = true;
            }
        }
    }

    if (fileArg || dumpFileNameArg || portArg || files.size() == 0 || port == 0)
    {
        printf("Usage: statistics-server [-f statistics.txt] [-f statistics.sock] [-d dump-file.txt] [-p udp_port]\n");

        return 1;
    }


    {
        App app;

        return app.Run(files, dumpFileName, port);
    }
}
