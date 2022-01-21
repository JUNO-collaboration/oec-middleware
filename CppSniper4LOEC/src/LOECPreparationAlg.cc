#include "SniperKernel/AlgBase.h"
#include "EvtNavigator/NavBuffer.h"
#include "Event/OECEvent.h"
#include "Event/OECHeader.h"

class LOECPreparationAlg : public AlgBase{
public:    
    LOECPreparationAlg(const std::string &name);
    ~LOECPreparationAlg();

    JM::NavBuffer* m_buf;

    virtual bool initialize();
    virtual bool execute();
    virtual bool finalize();
    

};

#include "SniperKernel/AlgFactory.h"
DECLARE_ALGORITHM(LOECPreparationAlg);

LOECPreparationAlg::LOECPreparationAlg(const std::string &name):
AlgBase(name),
m_buf(nullptr)
{
}

LOECPreparationAlg::~LOECPreparationAlg(){

}

bool LOECPreparationAlg::initialize(){
    SniperDataPtr<JM::NavBuffer>  navBuf(getParent(), "/Event");
    if ( navBuf.invalid() ) {
        LogError << "cannot get the NavBuffer @ /Event" << std::endl;
        return false;
    }
    m_buf = dynamic_cast<JM::NavBuffer*>(navBuf.data());
    return true;
}


bool LOECPreparationAlg::execute(){
    JM::NavBuffer::ElementPtr nav = *m_buf->current();
    JM::OECEvent* thisEvent = new JM::OECEvent();
    JM::OECHeader* thisHeader = new JM::OECHeader(); 
    thisEvent->setTime(nav->TimeStamp());
    thisHeader->setEvent(thisEvent);
    nav->addHeader("/Event/OEC", thisHeader);
    return true;
}

bool LOECPreparationAlg::finalize(){
    return true;
}
