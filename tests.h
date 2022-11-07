#pragma once

struct RegisterTest
{
    RegisterTest(const char * name, void (* pfn)(bool showDetails));

    const char * m_name;
    void (* m_pfn)(bool showDetails);
    RegisterTest * m_nextTest;

    static RegisterTest * s_firstTest;
    static RegisterTest * s_lastTest;
};
