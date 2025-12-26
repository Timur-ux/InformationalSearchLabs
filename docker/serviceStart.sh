#!/bin/bash

if [[ -n "$serviceName" ]]; then
	./$serviceName &
	pid=$!
	trap "kill -SIGINT $pid; sleep 5; echo SIGTERM" SIGTERM
	trap "kill -SIGINT $pid; sleep 5; echo SIGINT" SIGINT
	wait
	exit 0
else
	echo "ENV VAR [serviceName] not setup, i don't know what service needs to be started"
fi
