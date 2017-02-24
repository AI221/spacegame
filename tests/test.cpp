#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
using namespace std;

void add1(int* urint)
{
	*urint++;
	cout << "1.5: " << urint << endl;	
}

int main()
{
	int* test ;
	int tests_b;

	//test = &tests_b;

	*test = 10;
	
	cout << "1: " << *test << endl;	

	/*
	add1(test);

	cout << "2: " << test << endl;	
*test++;
	cout << "3: " << test << endl;	*/
	return 1;
}

