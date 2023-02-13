#ifndef MIDDLEWARE_CONFIG_SVC_H
#define MIDDLEWARE_CONFIG_SVC_H

#include "SniperKernel/SvcBase.h"

class MiddlewareConfigSvc : public SvcBase
{
    public :

        MiddlewareConfigSvc(const std::string& name);
        virtual ~MiddlewareConfigSvc(){};

        virtual bool initialize();
        virtual bool finalize();

        int getThrdNum(){return m_thrdNum;};

    private :
        int m_thrdNum;
};


#endif