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
            
        return

    def runMinimization() :
        return

    def calculateChi2() :
        return
