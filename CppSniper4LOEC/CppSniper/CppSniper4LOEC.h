#ifndef CPP_SNIPER_4_LOEC_H
#define CPP_SNIPER_4_LOEC_H


#include <vector>
#include <string>



class Task;
class LOECInputSvc;
class LOECOutputSvc;

class CppSniper4LOEC 
{
    public:
        CppSniper4LOEC();
        virtual ~CppSniper4LOEC();
        void initialize();
        virtual void process(void*);

    private:
        Task* m_task;
        LOECInputSvc*  m_input;
        LOECOutputSvc* m_output;

        static int waveTaskNum;
};

#endif
