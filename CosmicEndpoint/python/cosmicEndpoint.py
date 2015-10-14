
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
    
    def __init__(self, binning, selection, minPt) :

        self.binning   = binning
        self.selection = selection
        self.minPt     = minPt
        
        return
    
    def makeHistograms(binning, selection) :
        """Create the upper/lower positive/negative histograms with the specified binning
        and using the provided selection"""
        
        return

    def runMinimization() :
        return

    def calculateChi2() :
        return
