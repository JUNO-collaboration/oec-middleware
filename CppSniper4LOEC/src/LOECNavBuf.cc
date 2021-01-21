#include "LOECNavBuf.h"
#include <iostream>

LOECNavBuf::LOECNavBuf()
    : l1id(0xFFFFFFFF)
{
}

LOECNavBuf::~LOECNavBuf()
{
}

void LOECNavBuf::set(JM::EvtNavigator* evt)
{
    m_dBuf.push_back(ElementPtr(evt));
    ++m_iCur;
}

void LOECNavBuf::clear()
{
    m_dBuf.clear();
    m_iCur = -1;
}
