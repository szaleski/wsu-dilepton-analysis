#!/bin/sh

eval `scramv1 runtime -sh`
export PATH=${CMSSW_BASE}/test/${SCRAM_ARCH}:${PATH}
export PYTHONPATH=$PYTHONPATH:$CMSSW_BASE/src/WSUDiLeptons/CosmicEndpoint/python
export PYTHONPATH=$PYTHONPATH:$CMSSW_BASE/src/WSUDiLeptons/MuonAnalyzer/python
