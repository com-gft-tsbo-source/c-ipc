#!/usr/bin/env bash

trap "echo \"Terminating server at pid '\$PID'.\" ; kill -KILL \$PID ; exit 0" SIGTERM
trap "echo \"Interupting server at pid '\$PID'.\" ; kill -KILL \$PID ; exit 0" SIGINT

topic="$1" ; shift
name="$1"  ; shift

if [[ -z "$topic" ]] ; then topic="$IPC_TOPIC" ; fi
if [[ -z "$name" ]] ; then name="$IPC_NAME" ; fi
if [[ -z "$name" ]] ; then name="$topic" ; fi

topic=$( echo "$topic" | sed 's%^/*%%' )

if [[ -z "$topic" ]] ; then echo "Error: Topic is empty!" ; exit 1 ; fi
if [[ -z "$name" ]]  ; then echo "Error: Name is empty!" ; exit 1 ; fi

BINARY=/opt/server
PIDFILE="/var/run/$name.pid"
PID=0

echo
echo "----------------------------------------------------------------"
echo "Starting server as:"
echo "    '$BINARY' --pidfile '$PIDFILE' --topic '/$topic' --logfile "/var/log/server/$topic.log" $@ '$name'"
"$BINARY" --pidfile "$PIDFILE" --topic "/$topic" --logfile "/var/log/server/$name.$topic.log" $@ "$name" 
status=$?

if [[ $status -ne 0 ]] ; then
	echo "Error: Server failed to start! Status was '$status'."
	exit $status
fi

echo
count=0

while sleep 1 ; do

	STAMP=$( date '+%Y/%m/%d %H:%M:%S')

	if [[ ! -f "$PIDFILE" ]] ; then
		echo "Server terminated, pidfile deleted at $STAMP."
		exit 0
	fi

	PID=$(cat "$PIDFILE")

	if [[ -z "$PID" ]] ; then 
		echo "Error: Pidfile '$PIDFILE' is empty! Terminated at '$STAMP'."
		exit 1
	fi

	kill -0 "$PID" 2>/dev/null
	status=$?

	if [[ $status -ne "0" ]] ; then
		echo "Error: Server at pid '$PID' terminated at $STAMP!"
		rm -f "$PIDFILE"
		exit 1
	fi

	count=$(( count + 1 ))

	if [[ $count -le 10 ]] ; then continue ; fi

	echo "Server still running at pid '$PID' at $STAMP."
	count=0

done
