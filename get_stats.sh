#!/bin/bash


ids=()
bytesRead=()
bytesWritten=()
filesSent=()
filesReceived=()
clients=()


while read -r line 
do 
	words=$(echo $line | tr "\t" "\n")

	id="$(cut -d' ' -f1  <<< "$words")"
	ids=("${ids[@]}" "$id");

	bRead="$(cut -d' ' -f3  <<< "$words")"
	bytesRead=("${bytesRead[@]}" "$bRead");

	bWritten="$(cut -d' ' -f4  <<< "$words")"
	bytesWritten=("${bytesWritten[@]}" "$bWritten");

	fSent="$(cut -d' ' -f5  <<< "$words")"
	filesSent=("${filesSent[@]}" "$fSent");

	fReceived="$(cut -d' ' -f5  <<< "$words")"
	filesReceived=("${filesReceived[@]}" "$fReceived");

	clients="$(cut -d' ' -f6  <<< "$words")"
	clientsLeft=("${clientsLeft[@]}" "$clients");
done


i=0
realids=()

for(( i=0; i<"${#ids[@]}"; i++ ))
do
	if [[ " ${realids[*]} " != *" ${ids[$i]} "* ]]
	then
		realids=("${realids[@]}" "${ids[$i]}")
	fi
done


max=-1
for i in ${ids[@]}
do 
	if [ "$i" -ge "$max" ]
	then
		max="$i"
	fi
done 

min=100000000000
for i in ${ids[@]}
do 
	if [ "$i" -le "$min" ]
	then
		min="$i"
	fi
done 

for i in ${bytesRead[@]}
do
	let sumRead+="$i"
done


for i in ${bytesWritten[@]}
do
	let sumWritten+="$i"
done

for i in ${filesSent[@]}
do
	let sumFilesSent+="$i"
done


for i in ${filesReceived[@]}
do
	let sumFilesReceived+="$i"
done



for i in ${clientsLeft[@]}
do
	let sumClients+="$i"
done

echo Clients ids: "${realids[@]}"
echo Max id: "$max"
echo Min id: "$min"
echo Bytes sent: "$sumWritten"
echo Bytes received: "$sumRead"
echo Files sent: "$sumFilesSent"
echo Files received: "$sumFilesReceived"
echo Clients left: "${#sumClients[@]}"


#echo "${bytesWritten[@]}"
#echo "${filesSent[@]}"
#echo "${filesReceived[@]}"




