#include "SniperKernel/AlgBase.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"
#include "OecEvtConverter/OecEvtConverter.h"
#include <cassert>

using namespace std;

/*
此算法是一个用于模块测试的算法：测试OecEvtConverter
测试过程：
1, 制造一个EDM结构的OecEvt
2，使用OecEvtConverter将OecEvt写在一段buffer上，再根据buffer上的数据反序列化为OecEvt
3，比较两个OecEvt完全一致
*/
class OecEvtConverterTest: public AlgBase
{
private:
    OecEvtConverter* m_converter;
public:
    OecEvtConverterTest(const string& name);
    ~OecEvtConverterTest();
    virtual bool initialize(){return true;};
    virtual bool execute();
    virtual bool finalize(){return true;};
};

DECLARE_ALGORITHM(OecEvtConverterTest);

OecEvtConverterTest::OecEvtConverterTest(const string& name):
AlgBase(name)
{
    auto* ptr = malloc(1024);
    m_converter = new OecEvtConverter((uint8_t*)ptr);

}

bool OecEvtConverterTest::execute(){
    //制造一个OecEvt
    auto nav1 = new JM::EvtNavigator();
    auto header1 = new JM::OecHeader();
    nav1->addHeader("/Event/Oec", header1);
    auto event1 = new JM::OecEvt();
    header1->setEvent(event1);
    event1->setTime(TTimeStamp(0, 1));
    event1->addTag(2);
    event1->setEnergy(3);
    event1->setVertexX(4);
    event1->setVertexY(5);
    event1->setVertexZ(6);
    event1->setQBF(7);

    event1->setMuID({11, 12, 13, 14, 15});
    event1->setMuInX({16, 17, 18, 19, 20});
    event1->setMuInY({21, 22, 23, 24, 25});
    event1->setMuInZ({41, 42, 43, 44, 45});
    event1->setMuOutX({26, 27, 28, 29, 30});
    event1->setMuOutY({31, 32, 33, 34, 35});
    event1->setMuOutZ({36, 37, 38, 39, 40});

    m_converter->writeOecEvt(nav1, 0);
    auto nav2 = m_converter->getOecEvt();
    auto header2 = dynamic_cast<JM::OecHeader*>(nav2->getHeader("/Event/Oec"));
    auto event2 = (JM::OecEvt*)header2->event("JM::OecEvt");

    assert(event1->getVertexX() == event2->getVertexX());
    assert(event1->getMuInX() == event2->getMuInX());
    assert(event1->getMuOutZ() == event2->getMuOutZ());
    
    LogInfo<<"Test passed"<<std::endl;
    return true;
}

OecEvtConverterTest::~OecEvtConverterTest()
{
}
