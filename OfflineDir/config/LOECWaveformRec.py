#!/usr/bin/env python

def GetTask():
    import os
    offlineDir = os.getenv("OFFLINE_DIR")

    import Sniper
    Sniper.setLogLevel(5)
    task = Sniper.Task("WfTask")
    task.setEvtMax(-1)

    Sniper.loadDll("libCppSniper4LOEC.so")
    bufMgr = task.createSvc("LOECEvtMemMgr")
    isvc = task.createSvc("LOECInputSvc/InputSvc")
    isvc.property("InputType").set("Waveform")
    osvc = task.createSvc("LOECOutputSvc/OutputSvc")
    osvc.property("OutputType").set("QT")

    ## services
    import Geometry
    PMTParamSvc = task.createSvc("PMTParamSvc")
    #PMTParamSvc.property("DBType").set("File")
    PMTParamSvc.property("Path").set(offlineDir + "/data/pmtdata.txt")

    import PMTCalibSvc
    PMTCalibSvc = task.createSvc("PMTCalibSvc")
    PMTCalibSvc.property("DirPath").set(offlineDir + "/data")

    ## algorithms
    #Sniper.loadDll("libOECTestAlg.so")
    #alg = task.createAlg("OECTestAlg")
    Sniper.loadDll("libDeconvolution.so")
    decon=task.createAlg("Deconvolution")
    #decon.property("TotalPMT").set(17612)
    #decon.property("Threshold").set(0.06)
    #decon.property("Length").set(1250)
    #decon.property("Para1").set(50)
    #decon.property("Para2").set(120)
    #decon.property("Para3").set(30)
    #decon.property("Window").set(3)
    #decon.property("HitCounting").set(0)
    decon.property("CalibFile").set(offlineDir + "/data/SPE_v20.root")
    decon.property("Filter").set(offlineDir + "/data/filter3_m.root")

    ## custom settings
    #isvc.setLogLevel(2)

    return task

if __name__ == "__main__":

    task = GetTask()
    task.setEvtMax(10)
    task.setLogLevel(2)
    task.show()
    #task.run()
