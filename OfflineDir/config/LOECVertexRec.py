#!/usr/bin/env python

def GetTask():
    import os
    offlineDir = os.getenv("OFFLINE_DIR")

    import Sniper
    Sniper.setLogLevel(5)
    task = Sniper.TopTask("VtxTask")
    task.setEvtMax(-1)

    Sniper.loadDll("libCppSniper4LOEC.so")
    bufMgr = task.createSvc("LOECEvtMemMgr")
    isvc = task.createSvc("LOECInputSvc/InputSvc")
    isvc.property("InputType").set("QT")
    osvc = task.createSvc("LOECOutputSvc/OutputSvc")
    osvc.property("OutputType").set("Vertex")

    #read xml 
    import OECTagSvc
    lectagsvc = task.createSvc("OECTagSvc")
    lectagsvc.property("OECTagFile").set(offlineDir + "/config/tag.xml")
    import EvtConfig
    evtconfig = task.createSvc("EvtConfigSvc")
    evtconfig.property("seqListFile").set(offlineDir + "/config/seq.xml")
    evtconfig.property("sigListFile").set(offlineDir + "/config/LOEC_sig.xml")

    #store energy and time for tag
    import EvtStore
    task.property("svcs").append("EvtStoreSvc")

    #store result
    import EvtResult
    task.property("svcs").append("EvtResultSvc")

    #for fast reconstruction
    import Geometry
    geosvc=task.createSvc("RecGeomSvc")
    geosvc.property("GeomFile").set(offlineDir + "/data/JunoGeom.root")
    geosvc.property("GeomPathInRoot").set("JunoGeom")
    geosvc.property("FastInit").set(True)
    import JunoTimer
    task.createSvc("JunoTimerSvc")

    import EvtAlgorithms
    import EvtSteering
    import SimpleRecAlg
    import RecCdMuonAlg
    import SpmtMuonRecTool

    #subTask RecCdMuonAlg
    subTask_RecMuon = task.createTask("Task/subTask_RecCdMuonAlg")
    recMuonAlg = RecCdMuonAlg.createAlg(subTask_RecMuon)
    recMuonAlg.property("Use3inchPMT").set(True)
    recMuonAlg.useRecTool("SpmtMuonRecTool")
    task.property("algs").append("StepHandler")

    ## custom settings
    #osvc.setLogLevel(2)

    return task

if __name__ == "__main__":

    task = GetTask()
    task.setEvtMax(10)
    task.setLogLevel(2)
    task.show()
    #task.run()
