#include "EvtNavigator/NavBuffer.h"
#include "SniperKernel/AlgBase.h"

class OECTestAlg : public AlgBase
{
    public:
        OECTestAlg(const std::string& name);

        bool initialize();
        bool execute();
        bool finalize();

    private:
        int m_count;

        JM::NavBuffer* m_buf;
};
