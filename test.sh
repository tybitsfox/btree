#!/usr/bin/bash
#para1:传入程序的参数，生成节点的个数；para2:脚本循环的次数
i=0;j=0;k=$1;l=$2

if [ -z "$k" ];then
	let k=4509
fi
if [ -z "$l" ];then
	let l=99
fi

while [ $i -eq 0 ]
do
	./t05 $k > /dev/null
	let i=$?
	let "j += 1"
if [ $j -gt $l ];then
	break
fi
done
echo "return: $i loops: $j para: $k"

