#!/usr/bin/env python

def GetTask():
    import os
    offlineDir = os.getenv("OFFLINE_DIR")

    import Sniper
    Sniper.setLogLevel(5)
    task = Sniper.Task("OECTask")

    Sniper.loadDll("libCppSniper4HOEC.so")
    bufMgr = task.createSvc("HOECEvtMemMgr")
    isvc = task.createSvc("HOECInputSvc/InputSvc")
    osvc = task.createSvc("HOECOutputSvc/OutputSvc")

    import OECTagSvc
    hectagsvc = task.createSvc("OECTagSvc")
    hectagsvc.property("OECTagFile").set(offlineDir + "/config/tag.xml")

    import OECConfigSvc
    OECConf = task.createSvc("OECConfigSvc")
    OECConf.property("OECListFile").set(offlineDir + "/config/HOEC_sig.xml")

    import HECAlg
    hecalg = task.createAlg("HECAlg")
    #hecalg.setLogLevel(2)

    ## custom settings
    isvc.setLogLevel(2)
    osvc.setLogLevel(2)
    bufMgr.property("TimeWindow").set([0.0, 0.0])

    return task

if __name__ == "__main__":

    task = GetTask()
    task.setEvtMax(10)
    #task.setLogLevel(2)
    task.show()
    #task.run()
