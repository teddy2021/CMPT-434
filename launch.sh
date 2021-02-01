# CMPT 434
# ASSIGNMENT 1
# KODY MANASTYRSKI
# KOM607
# 11223681
#

#! /bin/bash


if [ "$1" = "-cu" ] || [ "$2" = "-cu" ];
then 
	lsof -i TCP > running.txt; 
	lsof -i UDP >> running.txt;
	output=$(grep -E "(^server|^proxy|^client)" running.txt);
	if [ -z "$output" ];
	then
		echo "No running processes"
	else
		echo "Processes running: "
		echo "$output"
	fi

elif [ "$1" = "-h" ];
then
	echo "Usage: '$0 [PORT] [PROTOCOL] [IP] [PROGRAM]'"
	echo "[PORT] is given as a number"
	echo "for running all or server"
	echo "[PROTOCOL] is given as either -T for TCP or -U for UDP, and is always"
	echo "neccessary."
	echo "[IP] is given as a standard IPv4 format value and is unnecessary if"
	echo "running all of the programs on the same machine, or running only the"
	echo "server"
	echo "[PROGRAM] is given as -S, -P, -C respectively,"
	echo "and will run the corresponding set of programs. For example"
	echo "-S -C will run the server and client."
	echo "Additionally you may give PROGRAM as a combination of the programs"
	echo "You want run, so -SP will run server and proxy. Note that this is not"
	echo "respective to order, so -SP = -PS"
	exit 0


elif [  "$#" -lt 2  -o  "$#" -gt 5  ];
then 
	echo "Incorrect number of arguments given.";
	echo "Usage: './launch.sh [PORT] [PROTOCOL] [IP] [PROGRAM]'"
	echo "Give -h for help text, or -cu to check if processes are up and running"
	exit 1
else
	port=$1;
	protocol=$2;
	SIP="";
	PIP="";
	IP=""
	shift;
	shift;

	while [ $# != 0 ];
	do
		if [ "$1" = "-S" ];
		then
			echo "Starting server..."
			./server $port $protocol > out 2>error_log.txt &
			SIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out | awk '{print $3}');
			echo "SERVER IP: $SIP"

		elif [ "$1" = "-P" ];
		then
			echo "Starting proxy..."
			./proxy $IP $port $protocol >out2 2>error_log.txt &
			PIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out2 | awk '{print $3}');
			echo "Proxy IP: $PIP"
			port=$(( port+1  ));
			echo "CLIENT-PROXY PORT: $port"

		elif [ "$1" = "-C" ];
		then
			if [ -n "$PIP" ];
			then
				IP=$PIP;
			elif [ -n "$SIP" ];
			then
				IP=$SIP;
			fi
			echo "Starting client with args"
			echo "\t[IP]: $IP\t[PORT]: $port\t[PROTOCOL]: $protocol"
			./client $IP $port $protocol

		elif [ "$1" = "-SP" -o "$1" = "-PS" ];
		then
			echo "Starting server..."
			./server $port $protocol >out 2>error_log.txt &
			SIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out | awk '{print $3}');

			echo "Starting proxy..."
			./proxy $IP $port $protocol >out2 2>error_log.txt &
			PIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out2 | awk '{print $3}');
			port=$(( port+1 ));

			echo "SERVER IP: $SIP";
			echo "PROXY IP: $PIP";
			echo "CLIENT-PROXY PORT: $port";

		elif [ "$1" = "-SC" -o "$1" = "-CS" ];
		then
			echo "Starting server..."
			./server $port $protocol >out 2>error_log.txt &
			SIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out | awk '{print $3}');
			echo "SERVER IP: $SIP";

			echo "Starting client..."
			./client $SIP $port $protocol 

		elif [ "$1" = "-PC" -o "$1" = "-CP" ];
		then
			echo "Starting proxy..."
			./proxy $IP $port $protocol >out2 2>error_log.txt &
			PIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out2 | awk '{print $3}');
			port=$(( port+1 ));

			echo "Starting client..."
			./client  $PIP $port $protocol 

		elif [ "$1" = "-SPC" -o "$1" = "-SCP" -o \
			"$1" = "-CSP" -o "$1" = "-CPS" -o \
		   "$1" = "-PSC" -o "$1" = "-PCS" ];
		then
			echo "Starting server with args"
			echo "\t[PORT]: $port\t[PROTOCOL]: $protocol"
			./server $port $protocol >out 2>error_log.txt &
			SIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out | awk '{print $3}');

			echo "Starting proxy with args"
			echo "\t[IP]: $SIP\t[PORT]: $port\t[PROTOCOL]: $protocol"
			./proxy $SIP $port $protocol >out2 2>error_log.txt &
			PIP=$( grep -E "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" out2 | awk '{print $3}');
			port=$(( port+1 ));

			echo "Starting client with args: "
			echo "\t[IP]: $PIP\t[PORT]: $port\t[PROTOCOL]: $protocol"
			./client $PIP $port $protocol 

		elif [ -n "$1" ];
		then
			IP=$1
		else
			echo "Incorrect option for program to launch";
			echo "Usage './launch.sh [PORT] [PROTOCOL] [PROGRAM]'"
			echo "give -h for help text."
		fi
		shift;
	done
fi

if [ "$?" != 0 ];
then
	echo "One or more of the programs closed on error, please check the error"
	echo "log (error_log.txt) for details."
	echo "Most recent error failure: $?"
fi


if [ "$1" = "-k" ] || [ "$2" = "-k" ];
then
	if [ ! -e "running.txt" ];
	then
		lsof -i TCP > running.txt;
		lsof -i UDP >> running.txt;
	fi
	mapfile -t list < <(grep -E "(^client|^proxy|^server)" running.txt | tr -s " " | awk '{print $2}' )
	for proc in "${list[@]}";
	do
		echo "Killing: $proc";
		kill -s SIGTERM $proc;
	done
fi

if [ -e "running.txt" ];
then
	rm -f running.txt;
fi


if [ -e "out.txt" -o -e "out2.txt" ];
then
	rm -f out*
fi
