#!/usr/bin/bash
i=0
j=0
k=$1
if [ -z "$k" ];then
	let k=4509
fi

while [ $i -eq 0 ]
do
	./t05 $k > /dev/null
	let i=$?
	let "j += 1"
if [ $j -gt 99 ];then
	break
fi
done
echo "return: $i loops: $j para: $k"

