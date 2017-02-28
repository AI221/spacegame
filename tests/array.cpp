#include <iostream>
#include <vector>


std::vector<std::vector<int> > test = {{}};

std::vector<int> simtest = {};

int main()
{
	simtest.push_back(200);
	simtest[1] = 27;

	std::cout << simtest[1] << std::endl;
	
	test.push_back(std::vector<int>(2));
	test[0].push_back(2);
	test[1].push_back(2);
	test[0][0] = 270;
	test[1][0] = 271;

	test[1][1] = 272;

	std::cout << test[0][0] << std::endl;
	std::cout << test[1][0] << std::endl;
	std::cout << test[1][1] << std::endl;

	//test[1][2] = 27;
	
	//std::cout << test[1][2] << std::endl;
	//std::cout << test[2][2] << std::endl;
	//std::cout << test[1][1] << std::endl;

	return 0;
}
