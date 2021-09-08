#########################################################################
# File Name: restart.sh
# Author: Sakura7301 
# Email: sakuraduck@foxmail.com 
# Github: https://github.com/Sakura7301 
# Created Time: Mon 30 Aug 2021 07:17:56 PM CST
#########################################################################
#!/bin/bash

while true
do
	ps -ef | grep "app-test" | grep -v "grep"
	if [ "$?" -eq 1 ]
	then
		./source/server 
		echo "process has benn restarted!"
	else
		echo "process already started!"

	fi
	sleep 20
done

