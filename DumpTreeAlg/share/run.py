#!/usr/bin/env python

import Sniper
task = Sniper.Task("task")
task.setLogLevel(3)

# Create Data Buffer Svc
import DataRegistritionSvc
task.createSvc("DataRegistritionSvc")

import BufferMemMgr
bufMgr = task.createSvc("BufferMemMgr")
bufMgr.property("TimeWindow").set([-0.001, 0.001])

# Create PMTCalib Svc
import PMTCalibSvc
calibsvc = task.createSvc("PMTCalibSvc");
calibsvc.property("DirPath").set("/besfs5/users/zoujh/JunoOEC/OfflineDir/data")

#Geometry service
import Geometry
geom = task.createSvc("RecGeomSvc")
geom.property("GeomFile").set("default")
geom.property("FastInit").set(True)

pmtparamsvc = task.createSvc("PMTParamSvc")
pmtparamsvc.property("Path").set("/besfs5/users/zoujh/JunoOEC/OfflineDir/data/pmtdata.txt")

import RootIOSvc, RootWriter
inputSvc = task.createSvc("RootInputSvc/InputSvc")
rWriter = task.createSvc("RootWriter")

#Calib
Sniper.loadDll("libDeconvolution.so")
deconv=task.createAlg("Deconvolution")
#if args.CalibFile:
#    deconv.property("CalibFile").set(args.CalibFile)
deconv.property("TotalPMT").set(17612)
deconv.property("Threshold1").set(0.025)
deconv.property("Threshold2").set(0.025)
deconv.property("Length").set(1250)
deconv.property("Para1").set(110)
deconv.property("Para2").set(160)
deconv.property("Para3").set(15)
deconv.property("Para4").set(80)
deconv.property("Para5").set(130)
deconv.property("Para6").set(15)
deconv.property("Window").set(9)
deconv.property("Calib").set(0)
#if args.Filter:
#    deconv.property("Filter").set(args.Filter)
deconv.property("HitCounting").set(0)
deconv.property("CalibFile").set("/besfs5/users/zoujh/JunoOEC/OfflineDir/data/SPE_v20.root")
deconv.property("Filter").set("/besfs5/users/zoujh/JunoOEC/OfflineDir/data/filter3_m.root")
#Sniper.loadDll("libTimeRec.so");
#timerec=task.createAlg("TimeRec")
#timerec.property("AlgOption").set("fit1")
#timerec.property("enableElecTruth").set(False)
#DumpTree
import DumpTreeAlg
alg = task.createAlg("DumpTreeAlg")

# set input file(s)
inputSvc.property("InputFile").set([
    #"/junofs/production/data-production/Pre-Releases/J20v2r0-Pre0/OEC/elecsim/elecsim-10001.root"
    "/junofs/production/data-production/Pre-Releases/J20v2r0-Pre0/Laser/266nm/photon_22000/266nm_0_0_0/elecsim/root/elecsim-1040.root"
    ])
# set output file
rWriter.property("Output").set({
    "FILE" : "planetree.root"
    })

task.setEvtMax(1)
task.show()
task.run()
