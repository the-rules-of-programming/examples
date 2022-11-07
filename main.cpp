
#include "tests.h"
#include <stdio.h>

int main(int argc, char ** argv)
{
	bool showDetails = false;

	for (const RegisterTest * test = RegisterTest::s_firstTest;
		 test;
		 test = test->m_nextTest)
	{
		printf("### %s ###\n", test->m_name);
		test->m_pfn(showDetails);
	}

	printf("### DONE ###\n");
}
