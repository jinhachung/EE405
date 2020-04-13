#!/bin/bash

cd .
#variablename=$(find -name "format-wow.*")
#echo $variablename

read -r one two three four <<<$(find . -type d -name "dir.*")
echo "$one|$two|$three|$four"

echo "cutting strings..."

one=$(echo $one | cut -d'/' -f 2)
two=$(echo $two | cut -d'/' -f 2)
three=$(echo $three | cut -d'/' -f 2)
four=$(echo $four | cut -d'/' -f 2)
str="$one $two $three $four"
#echo $str

### SORT STRING!

newstring=$(for i in $str; do
    echo $i
done | sort)

echo $newstring

read -r ONE TWO THREE FOUR <<<$newstring
