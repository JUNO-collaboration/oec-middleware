#include "LOECProcessor.h"
#include "CppSniper/CppSniper4LOEC.h"
#include "OEC_com/oec_com/pack.h"
#include "OEC_com/oec_com/OEC_define.h"
#include <iostream>

LOECProcessor::LOECProcessor()
{
    m_wfRec = new CppSniper4LOEC("LOECWaveformRec");
    m_vtxRec = new CppSniper4LOEC("LOECVertexRec");
}

LOECProcessor::~LOECProcessor()
{
    delete m_wfRec;
    delete m_vtxRec;
}

void LOECProcessor::oec_process(void* event, void* /*nullptr*/ )
{
    oec::EventDepository* data = reinterpret_cast<oec::EventDepository*>(event);
    auto& bufMap = data->getOneEventSimpleBufferMap();

    // waveform reconstruction
    m_wfRec->process(bufMap[STREAM_TAG_WAVE], bufMap[STREAM_TAG_WAVE_TQ]);

    // vertex reconstruction and low level OEC
    m_vtxRec->process(bufMap[STREAM_TAG_WAVE_TQ], bufMap[EVENTS_VERTEX_WAVE]);
}

extern "C" {
    oec::AlgInterface *create_processor()
    {
        return new LOECProcessor;
    }
}
