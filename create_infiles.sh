#!/bin/bash

if [ "$2" -lt 0 ];
then
	echo "<Num of files must be zero or positive number!>"
	exit 1
fi

if [ "$3" -lt 0 ];
then
	echo "<Num of directories must be zero or positive number!>"
	exit 1
fi

if [ "$4" -lt 0 ];
then
	echo "<Levels must be zero or positive number!>"
	exit 1
fi

dir=$1
seq_directories=()


if [ ! -d "$dir" ];
then
	mkdir -p "$dir"
	seq_directories=("${seq_directories[@]}" "$dir")
else
	echo Directory "$dir" exists!
fi

folder=$dir
directory=()
files=()

num_of_files=$2
num_of_dirs=$3
levels=$4



for(( i=0; i<"$num_of_dirs"; i++ ))
do 
	random_num=$(( 1 + RANDOM % 8 ))
	string=$(cat /dev/urandom | LC_CTYPE=C tr -dc 'a-zA-Z0-9' | fold -w "$random_num" | head -n 1)
	directory=("${directory[@]}" "$string")
done


for(( i=0; i<"$num_of_files"; i++ ))
do 
	random_num=$(( 1 + RANDOM % 8 ))
	string_file=$(cat /dev/urandom | LC_CTYPE=C tr -dc 'a-zA-Z0-9' | fold -w "$random_num" | head -n 1)
	files=("${files[@]}" "$string_file")
done

i=0

while [ "$i" -lt "${#directory[@]}" ]
do

cur=${directory[$i]}
temp=$dir
j=0

	while [ "$j" -lt "$levels" ]
	do
		cur=${directory[$i]}

		if [[ "$cur" == "" ]];
		then 
			echo -n
		else
			mkdir -p $temp/$cur
		temp=$temp/$cur
			seq_directories=("${seq_directories[@]}" "$temp")
		fi
		let i+=1
		let j+=1
		if [ "$i" -eq "$levels" ];
		then
			break
		fi
	done
done



i=0
j=0

remain=0
while [ "$i" -le "${#seq_directories[@]}" ]
do
	if [ "$i" -eq "${#seq_directories[@]}" ] && [ "$remain" -lt "$num_of_files" ]
	then
		i=0
	fi

	touch "${seq_directories[$i]}"/"${files[$j]}"
	toWrite="${seq_directories[$i]}"/"${files[$j]}"
	
	let i++
	let j++
	let remain++

	r=0
	mb=$(( 1 + RANDOM % 5))
	while [ "$r" -lt "$mb" ]
	do
		random_string=$(cat /dev/urandom | LC_CTYPE=C  tr -dc 'a-zA-Z0-9' | fold -w 256 | head -c 1000)
		echo "$random_string" >> $toWrite
		let r+=1
	done



	if [ "$j" -eq "${#files[@]}" ]
	then
		break
	fi

done










