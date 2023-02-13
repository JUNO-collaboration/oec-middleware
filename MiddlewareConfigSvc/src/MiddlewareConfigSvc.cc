#include "SniperKernel/SvcFactory.h"
#include "MiddlewareConfigSvc/MiddlewareConfigSvc.h"

DECLARE_SERVICE(MiddlewareConfigSvc);

MiddlewareConfigSvc::MiddlewareConfigSvc(const std::string& name):SvcBase(name)
{
    declProp("ThreadNum", m_thrdNum = 1);
}

bool MiddlewareConfigSvc::initialize(){
    return true;
}

bool MiddlewareConfigSvc::finalize(){
    return true;
}

