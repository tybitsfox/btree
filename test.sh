#!/usr/bin/bash
#para1:传入程序的参数，生成节点的个数；para2:脚本循环的次数
i=0;j=0;k=$1;
let l=$2-2

if [ -z "$k" ];then
	let k=4509
fi
if [ -z "$l" ];then
	let l=99
fi
if [ $l -le 1 ];then
	./t05 $k
	echo "runtime=$SECONDS return=$?"
	exit
fi
while [ $i -eq 0 ]
do
	let "j += 1"
	if [ $j -gt $l ];then
		break
	fi
	./t05 $k > /dev/null
	let i=$?
done
./t05 $k
let i=$?
let "j += 1"
echo "return: $i loops: $j para: $k runtime: $SECONDS"

