CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(StatisticsServer)

SET(EXECUTABLE_NAME statistics-server)

SET(CMAKE_CXX_STANDARD 14)

SET(SOURCE_FILES
        src/main.cpp
        src/app.cpp
        src/app.h
        src/statistics-db.cpp
        src/statistics-db.h
        src/udp-thread.cpp
        src/udp-thread.h
        src/signal-thread.cpp
        src/signal-thread.h
    )

ADD_EXECUTABLE(${EXECUTABLE_NAME} ${SOURCE_FILES})

ADD_EXECUTABLE(udpclient tools/udpclient.c)

SET(THREADS_PREFER_PTHREAD_FLAG ON)
FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(${EXECUTABLE_NAME} Threads::Threads)
