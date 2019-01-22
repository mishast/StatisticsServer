#!/bin/bash

FIFO=/tmp/stats.sock

if [ ! -p "$FIFO" ]; then
    mkfifo $FIFO
fi

(
#    echo "[26-06-15 14:10:27.725094] Statistics gathering started"
#    echo "TIME	EVENT	CALLCNT	FILLCNT	AVGSIZE	MAXSIZE	AVGFULL	MAXFULL	MINFULL	AVGDLL	MAXDLL	AVGTRIP	MAXTRIP	AVGTEAP	MAXTEAP	AVGTSMR	MAXTSMR	MINTSMR"

#    while true
#    do
        echo "[14:10:27]	ORDER					323			10		0		0		150"
#        sleep 1
#    done
sleep 600

) >> $FIFO