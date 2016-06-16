import sys,os,re

def checkRequiredArguments(opts, parser):
    """From: http://stackoverflow.com/questions/4407539/python-how-to-make-an-option-to-be-required-in-optparse
    Checks whether the parser should require a given argument or not
    """
    
    missing_options = []
    for option in parser.option_list:
        if re.match(r'^\[REQUIRED\]', option.help) and eval('opts.' + option.dest) == None:
            missing_options.extend(option._long_opts)
        if len(missing_options) > 0:
            parser.error('Missing REQUIRED parameters: ' + str(missing_options))
    return                                                        

def prettifyGraph(graph, graphParams):
    graph.SetLineColor(graphParams["color"])
    graph.SetLineWidth(4)
    graph.SetMarkerColor(graphParams["color"])
    graph.SetMarkerSize(0.5)
    graph.SetMarkerStyle(graphParams["marker"])
    graph.SetTitle(graphParams["title"])
    graph.GetXaxis().SetTitle("#Delta#kappa_{b} [c/TeV]")
    graph.GetYaxis().SetTitle(graphParams["yaxis"])
    return graph
                                                                                            

def flipHist(hist, debug=False):
    import numpy as np
    if (debug):
        print "flipping the observed histogram on top of the reference"
    # loop through bins getting the contents of each bin
    # create an array
    # reverse the array
    # call obs.SetContent(array)
    hist = hist.Clone("flipper")
    obsValsX = np.zeros(hist.GetNbinsX(),np.dtype('float64'))
    obsValsY = np.zeros(hist.GetNbinsX(),np.dtype('float64'))
    obsErrsY = np.zeros(hist.GetNbinsX(),np.dtype('float64'))
    for b in range(hist.GetNbinsX()):
        obsValsX[b] = b+1
        obsValsY[b] = hist.GetBinContent(b+1)
        obsErrsY[b] = hist.GetBinError(b+1)
        
    obsValsYRev = np.fliplr([obsValsY])[0]
    obsErrsYRev = np.fliplr([obsErrsY])[0]
    # hist.SetContent(obsValsYRev) ## doesn't work for some reason, oh well, hack it
    for b in range(hist.GetNbinsX()):
        hist.SetBinContent(b+1,obsValsYRev[b])
        hist.SetBinError(b+1,obsErrsYRev[b])

    return hist
        
def setMinPT(hist, nbins, minPt, symmetric=True, debug=False):
    """Takes an input histogram and sets the bin content to 
    0 if q/pT is outside the range
    """
    if debug:
        print "nBinsX %d"%(hist.GetNbinsX())
    if symmetric:
        thebin = hist.FindBin(-1./minPt)
        while not (hist.GetXaxis().GetBinLowEdge(thebin) < -1./minPt):
            if debug:
                print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
            thebin -= 1
            if debug:
                print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
        if debug:
            print "lower cut off %2.2f, bin %d, integral (first,bin) %d"%(-1./minPt,
                                                                           hist.FindBin(-1./minPt),
                                                                           hist.Integral(hist.GetXaxis().GetFirst(),
                                                                                         thebin))
            print "binlow %f, binup %f, binw %f:"%(hist.GetXaxis().GetBinLowEdge(thebin),
                                                   hist.GetXaxis().GetBinUpEdge( thebin),
                                                   hist.GetXaxis().GetBinWidth(  thebin))
        for binlow in range(0,thebin+1):
            hist.SetBinContent(binlow,0)
            hist.SetBinError(binlow,0)

    thebin = hist.FindBin(1./minPt)
    while not (hist.GetXaxis().GetBinUpEdge(thebin) > 1./minPt):
        if debug:
            print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
        thebin += 1
        if debug:
            print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
    if debug:
        print "upper cut off %2.2f, bin %d, integral (bin,last) %d"%(1./minPt,
                                                                      hist.FindBin(1./minPt),
                                                                      hist.Integral(thebin,
                                                                                    hist.GetXaxis().GetLast()))
        print "binlow %f, binup %f, binw %f:"%(hist.GetXaxis().GetBinLowEdge(thebin),
                                               hist.GetXaxis().GetBinUpEdge( thebin),
                                               hist.GetXaxis().GetBinWidth(  thebin))
        print "nbins+1 content %d"%(hist.GetBinContent(nbins+1))
        print "nbins+2 content %d"%(hist.GetBinContent(nbins+2))
    for binhigh in range(thebin,nbins+2):
        hist.SetBinContent(binhigh,0)
        hist.SetBinError(binhigh,0)
    if debug:
        print "nbins+1 content %d"%(hist.GetBinContent(nbins+1))
        print "nbins+2 content %d"%(hist.GetBinContent(nbins+2))

    return hist

def makeComparisonPlot(refHist, compHist, rebins, minpt, debug=False):
    pass
