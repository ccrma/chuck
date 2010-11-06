#!/bin/sh

#timeout of 60 seconds

chuck_bin="../chuck"
while [ `echo $1 | cut -c1` = "-" ]
do
    if [ $1 = "--bin" ]
    then
      shift
      chuck_bin=$1
    fi
    shift
done


timer=$1

while [ $timer -gt 0 ] 
do 
	sleep 3
	#echo "here comes the $2 murderer!"
	chuckproc=`ps -o pid,command | grep "$chuck_bin $2" | grep -v "grep"`;
	#echo "target : $chuckproc"
	chuckid=`echo $chuckproc | cut -f 1 -d" "`
	#echo "id is [$chuckid]"
	if [ $chuckid ] 
	then
		timer=`expr $timer - 3`
	else
		timer=0
	fi
	#echo "killer : timer loop $timer"
done	

if [ $chuckid ] 
then
# echo "killing $chuckid"
 kill -9 $chuckid
fi