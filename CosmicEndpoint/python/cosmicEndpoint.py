import root as r

class cosmicEndpoint() :
    """
    Run the cosmic endpoint analysis
    Provide a binning (for the pT), a selection requirement, minimum pT cut on the muons
    sample binning = [50,60,75,100,150,200,300,500,1000,5000]
    sample selection = {"pixhits":3,
                        "stripits":3,
                        "matchedstation":3,
                        "normchi2":3,
                        "prob":3,
                        "dxy":3,
                        "dz":3,
                        }
    
    
    """
    
    def __init__(self, infile, outfile, binning, selection, minPt) :
        self.infile    = infile
        self.outfile   = outfile
        self.binning   = binning
        self.selection = selection
        self.minPt     = minPt
        
        self.minushisto = {}
        self.plushisto  = {}
        self.lowerhisto = {}
        self.upperhisto = {}

        for bin in range(length(self.binning)-1):
            self.minushisto["pt%dto%d"%(self.binning[bin],self.binning[bin+1])] = r.TH1D("minuspt%dto%d"%(self.binning[bin],self.binning[bin+1]),
                                                                                    "q/p_{T}",
                                                                                    self.nBins,
                                                                                    0., 1./self.minPt)
            
            self.plushisto["pt%dto%d"%(self.binning[bin],self.binning[bin+1])] = r.TH1D("pluspt%dto%d"%(self.binning[bin],self.binning[bin+1]),
                                                                                   "q/p_{T}",
                                                                                   self.nBins,
                                                                                   0., 1./self.minPt)
            
            self.lowerhisto["pt%dto%d"%(self.binning[bin],self.binning[bin+1])] = r.TH1D("lowerpt%dto%d"%(self.binning[bin],self.binning[bin+1]),
                                                                                    "q/p_{T}",
                                                                                    self.nBins,
                                                                                    0., 1./self.minPt)
            
            self.upperhisto["pt%dto%d"%(self.binning[bin],self.binning[bin+1])] = r.TH1D("upperpt%dto%d"%(self.binning[bin],self.binning[bin+1]),
                                                                                    "q/p_{T}",
                                                                                    self.nBins,
                                                                                    0., 1./self.minPt)
            
            self.tree = infile.Get(treename)
        return
    
    def makeHistograms(tree, binning, selection) :
        """Create the upper/lower positive/negative histograms with the specified binning
        and using the provided selection"""
        
        for event in tree:
            upperbin = findBin(event.upper_P3.pt())
            lowerbin = findBin(event.lower_P3.pt())
            if event.upper_charge > 0:
                self.upperhisto[upperbin]
            
        return

    def findBin(pt):
        binning = self.binning
        if pt > binning[-1]:
            
            return bin

    def runMinimization(f, histBaseName, obsName, refName):
        return

    def makeGraph(f, histBaseName, obsName, refName):
        import numpy as np
        
        nBiasBins = 100
        maxBias = 0.05 #0.005
        #need two arrays, length = (2*nBiasBins)+1
        xVals = np.empty(nBiasBins)
        yVals = np.empty(nBiasBins)

        obs = f.Get("%s%sCurve"%(histBaseName,obsName))
        obs = f.Get("%s%sCurve"%(histBaseName,obsName))
        
        ref = f.Get("%s%sCurve"%(histBaseName,refName))
        ref = f.Get("%s%sCurve"%(histBaseName,refName))
        
        xVals.at(nBiasBins+1) = 0
        yVals.at(nBiasBins+1) = calculateChi2(obs,ref)

        for i in range(nBiasBins):
            obs_posBias = f.Get("%s%sCurvePlusBias%03d"%( histBaseName,obsName,i+1))
            obs_negBias = f.Get("%s%sCurveMinusBias%03d"%(histBaseName,obsName,i+1))

            ref_posBias = f.Get("%s%sCurvePlusBias%03d"%( histBaseName,refName,i+1))
            ref_negBias = f.Get("%s%sCurveMinusBias%03d"%(histBaseName,refName,i+1))

            biasVal = (i+1)*(maxBias/nBiasBins)
            xVals.at(nBiasBins+1+i) = biasVal
            yVals.at(nBiasBins+1+i) = calculateChi2(obs_posBias,ref_posBias)

            xVals.at(i) = -1.*biasVal
            yVals.at(i) = calculateChi2(obs_negBias,ref_negBias)
            
        graph = r.TGraph(histBaseName,histBaseName,xVals,yVals)
        return graph

    def calculateChi2(hobs,href):
        if (hobs.GetNbinsX() != href.GetNbinsX()):
            print "histograms have different number of bins in X:"
            print hobs.GetName(),"has",hobs.GetNbinsX()
            print href.GetName(),"has",href.GetNbinsX()
            return -1.
        
        nBins = hobs.GetNbinsX()
        cChi2 = 0.
        for b in range(nBins):
            obs = hobs.GetBinContent(b+1);
            ref = href.GetBinContent(b+1);
	
            binChi2 = ((obs-ref)**2)/ref
            cChi2 += binChi2
        
        return chi2
