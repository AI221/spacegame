#include <iostream>
#include <functional>
class test1
{
	public:
		virtual void callit()= 0;
		bool test;
};

class test2 : public test1
{
	public:
		test2(int x, int y, int z);
		void callit();
		void called();
		std::function< void () > a;
		bool test = true;
};
test2::test2(int x, int y, int z)
{
	x = y;
}
void test2::callit()
{
	std::cout << "callit" << std::endl;
	called();
	if (a)
		a();
}
void test2::called()
{
	std::cout << "WOOT" << std::endl;
}
int main()
{
	test2* a = new test2(1,2,3);

//	std::function< void() > mycalled = called;
	auto /*determine output for me*/ myfunc = [] /*wants no variables*/ () /*wants no input*/ { std::cout << "Hello, World!"; }; 

int test = 0;
	auto myfunc2 = [&] () { std::cout << "Hello, friends" << std::endl; test = 1; };
	
//	a->called = &mycalled;
	a->callit();
	myfunc();

	a->a = myfunc2;
	std::cout << test << std::endl;

	a->callit();

	std::cout << test << std::endl;

	std::cout << "-----------------" <<std::endl;

	std::cout << a->test << std::endl;


	test2* b = new test2(1,2,3);
	std::cout << b->test << std::endl;

}



