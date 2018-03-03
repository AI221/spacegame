#!/bin/bash

read term

for file in $(find src/*)
do
	echo $file
	cat $file | grep $term
done
