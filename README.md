# StatisticsServer

The Server gather numbers from file/pipe, report statistics in response to udp packet and dump statistics to stdout in response to SIGUSR1 signal.

The server reads the file / channels one by one, the file names specified on the command line.

```
Usage: statistics-server [-f statistics.txt] [-f statistics.sock] [-d dump-file.txt] [-p udp_port]
```

Input file is tab separated table.

Example:

```
[26-06-15 14:10:27.725094] Statistics gathering started
TIMExxxxxx	EVENT	CALLCNT	FILLCNT	AVGSIZE	MAXSIZE	AVGFULL	MAXFULL	MINFULL	AVGDLL	MAXDLL	AVGTRIP	MAXTRIP	AVGTEAP	MAXTEAP	AVGTSMR	MAXTSMR	MINTSMR
[14:10:27]	ORDER	xxxxxxx	xxxxxxx	xxxxxxx	xxxxxxx	518	xxxxxxx	xxxxxxx	42	xxxxxx	0	xxxxxxx	0	xxxxxxx	476
[14:10:27]	ORDER					323			10		0		0		313
[14:10:27]	ORDER					225			8		0		0		217
```

Server use EVENT column and gets number from AVFTSMR column.

After receive packet with event name, server send statistics like this:
```
ORDER min=169 50%=190 90%=249 99%=271 99.9%=474
```

After receive signal SIGUSR1 server dumps to stdout statistics like this:
```
ORDER min=169 50%=190 90%=249 99%=271 99.9%=474
ExecTime	TransNo	Weight,%%	Percent
165	0	0.00	0.00
170	6	0.00	0.00
175	3863	0.39	0.39
180	50032	5.00	5.39to stdout
185	181139	18.11	23.50
190	230152	23.02	46.52
195	182694	18.27	64.79
200	109069	10.91	75.70
205	47673	4.77	80.46
...
```
