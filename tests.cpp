
#include "tests.h"
#include <algorithm>

RegisterTest::RegisterTest(const char * name, void (* pfn)(bool showDetails)) :
	m_name(name), 
	m_pfn(pfn),
	m_nextTest(nullptr)
{ 
	if (!s_lastTest)
	{
		s_firstTest = this;
		s_lastTest = this;
	}
	else
	{
		s_lastTest->m_nextTest = this;
		s_lastTest = this;
	}
}

RegisterTest * RegisterTest::s_firstTest = nullptr;
RegisterTest * RegisterTest::s_lastTest = nullptr;
