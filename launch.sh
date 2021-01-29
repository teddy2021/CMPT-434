# CMPT 434
# ASSIGNMENT 1
# KODY MANASTYRSKI
# KOM607
# 11223681
#
#! /bin/bash
#


if [ "$1" = "-h" ];
then
	echo "Usage: './launch.sh [PORT] [IP] [PROTOCOL] [SERVER|PROXY|CLIENT|ALL]'"
	echo "[PORT] is given as a number"
	echo "[IP] is given as a standard IPv4 format value and is unnecessary"
	echo "for running all or server"
	echo "[PROTOCOL] is given as either -T for TCP or -U for UDP, and is always"
	echo "neccessary."
	echo "[SERVER|PROXY|CLIENT|ALL] is given as -S, -P, -C, -A respectively,"
	echo "and will run the corresponding set of programs. For example"
	echo "-S -C will run the server and client"
	exit 0
fi

if [  "$#" -lt 2  -o  "$#" -gt 6  ];
then 
	echo "Incorrect number of arguments given.";
	echo "Usage: './launch.sh [PORT] [PROTOCOL] [SERVER|PROXY|CLIENT|ALL]'"
	echo "Give -h for help text"
	exit 1
fi

port=$1;
shift;
echo "port: $port";
protocol=$1;
echo "proto: $protocol";
IP="";
shift;
while [ $# != 0 ];
do
	echo "Current Arg: $1"
	if [ $1 = "-S" ];
	then
		echo "Launching Server...";
		./server $port $protocol 2>error_log.txt &
		IP=$(lsof -i :$port | grep "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+")
	fi

	shift;
done
		
