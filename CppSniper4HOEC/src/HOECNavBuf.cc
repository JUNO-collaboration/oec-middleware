#include "HOECNavBuf.h"
#include "RootIOSvc/IInputStream.h"

using JM::EvtNavigator;

HOECNavBuf::HOECNavBuf(double floor, double ceiling)
    : m_floor(floor),
      m_ceiling(ceiling),
      m_beyond(nullptr)
{
}

HOECNavBuf::~HOECNavBuf()
{
}

#include <iostream>
bool HOECNavBuf::next()
{
    bool stat = false;

    if ( m_beyond != nullptr ) {
        ++m_iCur;
        if ( m_iCur < m_dBuf.size() && timeShift(m_beyond) >= m_ceiling ) {
            trimDated();
            stat = true;
        }
        else {
            --m_iCur;
            m_dBuf.push_back(ElementPtr(m_beyond));
            m_beyond = nullptr;
        }
    }

    return stat;
}

void HOECNavBuf::trimDated()
{
    JM::EvtNavigator* fevt = m_dBuf.front().get();
    while ( fevt != m_dBuf[m_iCur].get() ) {
        if ( m_floor < 0.0 && timeShift(fevt) >= m_floor ) {
            break;
        }
        m_dBuf.pop_front();
        --m_iCur;
        fevt = m_dBuf.front().get();
    }
}

double HOECNavBuf::timeShift(EvtNavigator* nav)
{
    const TTimeStamp& t0 = m_dBuf[m_iCur]->TimeStamp();
    TTimeStamp t = nav->TimeStamp();
    t.Add(TTimeStamp(-t0.GetSec(), -t0.GetNanoSec()));
    return t.AsDouble();
}
