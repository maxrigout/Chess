#include "TestRunner.h"

int main(int argc, char** argv)
{
	TestRunner::Run({});
	TestRunner::PrintReport();
	return 0;
}