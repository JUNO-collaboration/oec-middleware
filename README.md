# Introduction

This project implements the interfaces in oec-adptor. There are mainly 3 steps to every event (for each LEC/ps and HEC/es):

1. convert DAQ data blocks to offline EDM objects
2. process the event by invoking oec-prototype & offline algorithms
3. convert the result to DAQ data blocks and return it back to DAQ process

# Dependencies

- external (common libraries for both DAQ and offline, such as boost, ROOT, xerces-c, etc.)
- sniper
- offline (of JUNO)
- oec-prototype
- oec-adaptor

# Build

Modify "setenv.sh", make sure `$JUNOTOP` and `$MyWorkArea` are set to proper directories.

```bash
source setenv.sh
cd OECTestAlg/cmt
cmt br cmt config
cmt br cmt make
```

# Plans

Maybe we can merge oec-adaptor and oec-middleware into 1 project in the future.
