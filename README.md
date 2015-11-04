# wsu-dilepton-analysis
Code for WSU dilepton analysis
## Overview

### Setup the code
In order to start using the code, you will need to check out a CMSSW release area (preferably 74X or higher, as no development has been tested on older releases)
From your CMS SW aware directory you can execute the following commands:
```bash
export SCRAM_ARCH=slc6_amd64_gcc472 # or another architecture of your choice
scram l CMSSW # to see which CMSSW releases are available for your chosen architecture
cmsrel CMSSW_7_Y_X
cd CMSSW_7_Y_X/src
cmsenv
git cms-init
git clone git@github.com:jsturdy/wsu-dilepton-analysis.git WSUAnalysis
## or, if you've forked the code to your own github
git clone git@github.com:<username>/wsu-dilepton-analysis.git WSUAnalysis
scram b -j8
```

By this point, you should have the code checked out and compiled

### Using the code
There are several different tools provided in this package doing several different things
* ```WSUAnalysis/MuonAnalyzer``` provides a simple nTupler for any ```reco::Muon``` collection and will split the muons into upper/lower, depending on their positions in the CMS detector.  It is currently being used for several studies using cosmic ray muons where the single muon is split into upper and lower legs by the CMS reconstruction.
* ```WSUAnalysis/CosmicEndpoint``` provides several tools to run a study looking at the "endpoint" in the curvature (q/pT) distribution.  It is being used to provide a limit on the momentum scale uncertainty for high-pT muons, and has been adapted from code and studies done by previous groups at CMS (N. Kypryos UF, J. Tucker UCLA (now Cornell))

#### Tools
