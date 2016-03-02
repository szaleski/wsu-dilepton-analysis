#!/bin/env python

class cosmicEndpointShort() :
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
    
    def __init__(self, infiledir, outfile, maxbias, nBiasBins=1000, nTotalBins=5000, factor=1000, rebins=1) :
        import ROOT as r
        self.infiles = {}
        self.infiles["picky"] = r.TFile(infiledir+"/CosmicHistOut_Picky.root","r")
        self.infiles["dyt"  ] = r.TFile(infiledir+"/CosmicHistOut_DYT.root"  ,"r")
        self.infiles["tunep"] = r.TFile(infiledir+"/CosmicHistOut_TuneP.root","r")
        self.infiles["tpfms"] = r.TFile(infiledir+"/CosmicHistOut_TPFMS.root","r")
        self.outfile   = r.TFile(outfile,"recreate")
        self.outdirs   = {}

        self.maxbias    = maxbias
        self.nBiasBins  = nBiasBins
        self.nTotalBins = nTotalBins
        self.factor     = factor
        self.rebins     = rebins

        self.graphInfo = {}
        self.graphInfo["chi2"] = {"color":r.kBlue,   "marker":r.kFullTriangleUp,
                                  "title":"Calculated #chi^{2}/ndf",         "yaxis":""}
        self.graphInfo["KS"]   = {"color":r.kGreen-2,"marker":r.kFullDiamond,
                                  "title":"Kolmogorov test statistic",       "yaxis":""}
        self.graphInfo["AD"]   = {"color":r.kRed+2,  "marker":r.kFullCross,
                                  "title":"Anderson-Darling test statistic", "yaxis":""}
        self.graphInfo["Chi2"] = {"color":r.kCyan+3, "marker":r.kFullTriangleDown,
                                  "title":"ROOT #chi^{2}/ndf",               "yaxis":""}
        return

    def runMinimization(self, histBaseName, obsName, refName, needsFlip, getResiduals=False):
        import ROOT as r

        self.outfile.cd()
        self.outdirs["obs_%s"%(obsName)] = {} #self.outfile.mkdir("obs_%s"%(obsName))
        self.outdirs["obs_%s"%(obsName)]["results"] = self.outfile.mkdir("obs_%s"%(obsName))
        self.outdirs["obs_%s"%(obsName)]["picky"]   = self.outfile.mkdir("obs_%s/picky"%(obsName))
        self.outdirs["obs_%s"%(obsName)]["dyt"  ]   = self.outfile.mkdir("obs_%s/dyt"  %(obsName))
        self.outdirs["obs_%s"%(obsName)]["tunep"]   = self.outfile.mkdir("obs_%s/tunep"%(obsName))
        self.outdirs["obs_%s"%(obsName)]["tpfms"]   = self.outfile.mkdir("obs_%s/tpfms"%(obsName))
        
        self.outdirs["obs_%s"%(obsName)]["pickyresiduals"] = self.outfile.mkdir("obs_%s/picky/residuals"%(obsName))
        self.outdirs["obs_%s"%(obsName)]["dytresiduals"  ] = self.outfile.mkdir("obs_%s/dyt/residuals"  %(obsName))
        self.outdirs["obs_%s"%(obsName)]["tunepresiduals"] = self.outfile.mkdir("obs_%s/tunep/residuals"%(obsName))
        self.outdirs["obs_%s"%(obsName)]["tpfmsresiduals"] = self.outfile.mkdir("obs_%s/tpfms/residuals"%(obsName))
        
        picky = self.makeGraph(self.infiles["picky"],histBaseName, obsName, refName, "picky",
                               needsFlip, getResiduals)
        dyt   = self.makeGraph(self.infiles["dyt"  ],histBaseName, obsName, refName, "dyt",
                               needsFlip, getResiduals)
        tunep = self.makeGraph(self.infiles["tunep"],histBaseName, obsName, refName, "tunep",
                               needsFlip, getResiduals)
        tpfms = self.makeGraph(self.infiles["tpfms"],histBaseName, obsName, refName, "tpfms",
                               needsFlip, getResiduals)

        self.outfile.cd()
        self.outdirs["obs_%s"%(obsName)]["results"].cd()
        for test in ["chi2","KS","AD","Chi2"]:
            pickyCanvas = r.TCanvas("picky_%s"%(test),"picky_%s"%(test),800,800)
            #pickyCanvas.SetDirectory(self.outdirs["obs_%s"%(obsName)]["results"])
            picky[test].SetName("graph_picky_%s"%(test))
            picky[test].Draw("AP")
            pickyCanvas.Write("picky_%s"%(test))
            picky[test].Write("graph_picky_%s"%(test))

            dytCanvas = r.TCanvas("dyt_%s"%(test),"dyt_%s"%(test),800,800)
            #dytCanvas.SetDirectory(self.outdirs["obs_%s"%(obsName)]["results"])
            dyt[test].SetName("graph_dyt_%s"%(test))
            dyt[test].Draw("AP")
            dytCanvas.Write("dyt_%s"%(test))
            dyt[test].Write("graph_dyt_%s"%(test))

            tunepCanvas = r.TCanvas("tunep_%s"%(test),"tunep_%s"%(test),800,800)
            #tunepCanvas.SetDirectory(self.outdirs["obs_%s"%(obsName)]["results"])
            tunep[test].SetName("graph_tunep_%s"%(test))
            tunep[test].Draw("AP")
            tunepCanvas.Write("tunep_%s"%(test))
            tunep[test].Write("graph_tunep_%s"%(test))

            tpfmsCanvas = r.TCanvas("tpfms_%s"%(test),"tpfms_%s"%(test),800,800)
            #tpfmsCanvas.SetDirectory(self.outdirs["obs_%s"%(obsName)]["results"])
            tpfms[test].SetName("graph_tpfms_%s"%(test))
            tpfms[test].Draw("AP")
            tpfmsCanvas.Write("tpfms_%s"%(test))
            tpfms[test].Write("graph_tpfms_%s"%(test))
            
        #self.outfile.cd()
        #self.outdirs["obs_%s"%(obsName)]["results"].Write()
        #self.outfile.Write()

        for track in ["picky","dyt","tunep","tpfms"]:
            self.outfile.cd()
            self.outdirs["obs_%s"%(obsName)][track].cd()
            CounterCanvas = r.TCanvas("%sCounterCan"%track,"%sCounterCan"%track,800,800)
            #CounterCanvas.SetDirectory(self.outdirs["obs_%s"%(obsName)][track])
            CounterCanvas.cd()
            UpperCounters = self.infiles[track].Get("upperCounters")
            LowerCounters = self.infiles[track].Get("lowerCounters")
            UpperCounters.SetDirectory(self.outdirs["obs_%s"%(obsName)][track])
            LowerCounters.SetDirectory(self.outdirs["obs_%s"%(obsName)][track])
            UpperCounters.SetLineColor(r.kRed)
            UpperCounters.SetLineWidth(2)
            UpperCounters.SetMarkerStyle(r.kFullDiamond)
            LowerCounters.SetLineColor(r.kRed)
            LowerCounters.SetLineWidth(2)
            LowerCounters.SetMarkerStyle(r.kFullCross)
            UpperCounters.Draw("ep0")
            LowerCounters.Draw("ep0sames")
            #CounterCanvas.Write("%sCounterCan"%(track))
            #self.outfile.cd()
            #self.outdirs["obs_%s"%(obsName)][track].WriteObject(CounterCanvas,"%sCounterCan"%(track))

        self.outfile.Write()
        self.outfile.Close()
        return

    def makeGraph(self, f, histBaseName, obsName, refName, trackName, needsFlip, getResiduals):
        import ROOT as r
        import numpy as np
        import sys,os
        
        nBiasBins = self.nBiasBins
        maxBias   = self.maxbias
        factor    = self.factor

        #need two arrays, length = (2*nBiasBins)+1
        xVals = {}
        yVals = {}

        for test in ["chi2","KS","AD","Chi2"]:
            xVals[test] = np.zeros(2*nBiasBins+1,np.dtype('float64'))
            yVals[test] = np.zeros(2*nBiasBins+1,np.dtype('float64'))

        obs = f.Get("%s%sCurve"%(histBaseName,obsName))
        ref = f.Get("%s%sCurve"%(histBaseName,refName))
        obs.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
        ref.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])

        # set bin content to 0 for bins outside the min pT cut
        print "%s: raw: %d"%(trackName,obs.Integral())
        obs = self.setMinPT(obs,self.nTotalBins,200./self.factor,needsFlip)
        print "%s: raw: %d"%(trackName,ref.Integral())
        ref = self.setMinPT(ref,self.nTotalBins,200./self.factor,needsFlip)
        
        print "%s: raw: %d"%(trackName,obs.Integral())
        print "%s: raw: %d"%(trackName,ref.Integral())

        obs = obs.Rebin(self.rebins)
        ref = ref.Rebin(self.rebins)

        myCan1 = r.TCanvas("%s_%s_original"%(histBaseName,trackName),
                           "%s_%s_original"%(histBaseName,trackName),
                           800,800)
        #myCan1.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
        obs.SetLineColor(r.kRed)
        obs.SetLineWidth(2)
        ref.SetLineColor(r.kBlue)
        ref.SetLineWidth(2)

        obs.Draw("ep0")
        ref.Draw("ep0sames")
        self.outdirs["obs_%s"%(obsName)][trackName].cd()
        myCan1.Write("%s_%s_original"%(histBaseName,trackName))
        #self.outfile.cd()
        #self.outdirs["obs_%s"%(obsName)][trackName].Write()

        if (obs.Integral()>0):
            rescale = ref.Integral()/obs.Integral()
            print "Rescaling with factor %f"%(rescale)
            obs.Scale(rescale)
        
        myCan2 = r.TCanvas("%s_%s_scaled"%(histBaseName,trackName),
                           "%s_%s_scaled"%(histBaseName,trackName),
                           800,800)
        #myCan2.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
        obs.SetLineColor(r.kRed)
        obs.SetLineWidth(2)
        ref.SetLineColor(r.kBlue)
        ref.SetLineWidth(2)

        obs.Draw("ep0")
        ref.Draw("ep0sames")
        self.outdirs["obs_%s"%(obsName)][trackName].cd()
        myCan2.Write("%s_%s_scaled"%(histBaseName,trackName))
        #self.outfile.cd()
        #self.outdirs["obs_%s"%(obsName)][trackName].Write()

        # input histogram is signed (plus/minus), to compare we have to take the absolute value of one to
        # mirror it on top of the second, however, at the moment, the outputs don't look good, so
        # requires investigation
        if (needsFlip):
            print "flipping the observed histogram on top of the reference"
            # loop through bins getting the contents of each bin
            # create an array
            # reverse the array
            # call obs.SetContent(array)
            obsValsX = np.zeros(obs.GetNbinsX(),np.dtype('float64'))
            obsValsY = np.zeros(obs.GetNbinsX(),np.dtype('float64'))
            obsErrsY = np.zeros(obs.GetNbinsX(),np.dtype('float64'))
            for b in range(obs.GetNbinsX()):
                obsValsX[b] = b+1
                obsValsY[b] = obs.GetBinContent(b+1)
                obsErrsY[b] = obs.GetBinError(b+1)
                
            obsValsYRev = np.fliplr([obsValsY])[0]
            obsErrsYRev = np.fliplr([obsErrsY])[0]
            # obs.SetContent(obsValsYRev) ## doesn't work for some reason, oh well, hack it
            for b in range(obs.GetNbinsX()):
                obs.SetBinContent(b+1,obsValsYRev[b])
                obs.SetBinError(b+1,obsErrsYRev[b])

        #print obs, ref

        #not now#mode = 4
        #not now#self.calculateChi2(obs,ref,0,False,False)
        #not now#self.calculateChi2(obs,ref,1,False,False)
        #not now#self.calculateChi2(obs,ref,2,False,False)
        #not now#self.calculateChi2(obs,ref,3,False,False)
        #not now#self.calculateChi2(obs,ref,4,False,False)
        #not now## should perhaps return chi2/ndof?
        #not now#xVals["chi2"][nBiasBins] = 0.
        #not now#curvatureChi2 = self.calculateChi2(obs,ref,mode)
        #not now#print "curvatureChi2",curvatureChi2
        #not now#yVals["chi2"][nBiasBins] = curvatureChi2[1]

        # these are seeming to not work...
        xVals["KS"][nBiasBins] = 0.
        yVals["KS"][nBiasBins] = obs.KolmogorovTest(ref,"D")

        xVals["AD"][nBiasBins] = 0.
        yVals["AD"][nBiasBins] = obs.AndersonDarlingTest(ref,"D")

        chi2opts = "P,UU,NORM,CHI2/NDF"
        xVals["Chi2"][nBiasBins] = 0.
        resids = np.zeros(self.nTotalBins,np.dtype('float64')) # pointer argument, one per bin, not quite working
        #yVals["Chi2"][nBiasBins] = obs.Chi2Test(ref,"PCHI2/NDF",resids)
        #yVals["Chi2"][nBiasBins] = obs.Chi2Test(ref,chi2opts,resids)
        if getResiduals:
            print "residuals", resids
            self.outdirs["obs_%s"%(obsName)][trackName+"residuals"].cd()
            residuals0 = r.TH1D("%s_%s_residuals0"%(histBaseName,trackName),
                                "%s_%s_residuals0"%(histBaseName,trackName),
                               100,-5.,5.)
            residuals0.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName+"residuals"])
            for val in resids:
                residuals0.Fill(val)
            residualCanvas = r.TCanvas("%s_%s_residual0"%(histBaseName,trackName),
                                       "%s_%s_residual0"%(histBaseName,trackName),
                                       800,800)
            
            #residualsCanvas.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName+"residuals"])
            residuals0.Draw()
            residuals0.Write("%s_%s_residuals0"%(histBaseName,trackName))
            residualCanvas.Write("%s_%s_residual0"%(histBaseName,trackName))

        ## testing the more complete chi2 function
        chi2Val  = r.Double(0.) # necessary for pass-by-reference in python
        chi2ndf  = r.Long(0)    # necessary for pass-by-reference in python
        igood    = r.Long(0)    # necessary for pass-by-reference in python
        histopts = "UU,NORM" # unweighted/unweighted, normalized
        #histopts = "UUNORM" # unweighted/weighted, normalized
        
        #dummy = obs.Chi2Test(ref,"PCHI2/NDF")       # default options, return chi2/ndf, print summary
        #dummy = obs.Chi2Test(ref,"UWPCHI2/NDF")     # histograms are unweighted/weighted, respectively
        #dummy = obs.Chi2Test(ref,"UUPCHI2/NDF")     # unweighted/unweighted, return chi2/ndf, print summary
        #dummy = obs.Chi2Test(ref,"UUNORMPCHI2/NDF") # one or more histogram is scaled
        #dummy = ref.Chi2Test(obs,"UWNORMPCHI2/NDF") # 
        #dummy = obs.Chi2Test(ref,"UWNORMPCHI2/NDF") # 

        prob = obs.Chi2TestX(ref,chi2Val,chi2ndf,igood,histopts,resids)
        print "Chi2TestX: prob=%f, chi2=%f, chi2ndf=%d, igood=%d"%(prob,chi2Val,chi2ndf,igood)
        if chi2ndf > 0:
            yVals["Chi2"][nBiasBins] = chi2Val/(1.*chi2ndf)
        else:
            print "problem, no degrees of freedom"
            yVals["Chi2"][nBiasBins] = -1
            
        if getResiduals:
            print "residuals", resids
            self.outdirs["obs_%s"%(obsName)][trackName+"residuals"].cd()
            residuals = r.TH1D("%s_%s_residuals"%(histBaseName,trackName),
                               "%s_%s_residuals"%(histBaseName,trackName),
                               100,-5.,5.)
            residuals.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName+"residuals"])
            for val in resids:
                residuals.Fill(val)
            residualCanvas = r.TCanvas("%s_%s_residual"%(histBaseName,trackName),
                                       "%s_%s_residual"%(histBaseName,trackName),
                                       800,800)
            
            #residualsCanvas.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName+"residuals"])
            residuals.Draw()
            residuals.Write("%s_%s_residuals"%(histBaseName,trackName))
            residualCanvas.Write("%s_%s_residual"%(histBaseName,trackName))

        #raw_input("enter to continue")
        self.outdirs["obs_%s"%(obsName)][trackName].cd()
        myCan3 = r.TCanvas("%s_%s_analyzed"%(histBaseName,trackName),
                           "%s_%s_analyzed"%(histBaseName,trackName),
                           800,800)
        #myCan3.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
        obs.SetLineColor(r.kRed)
        obs.SetLineWidth(2)
        ref.SetLineColor(r.kBlue)
        ref.SetLineWidth(2)

        obs.Draw("ep0")
        ref.Draw("ep0sames")
        #self.outdirs["obs_%s"%(obsName)][trackName].cd()
        myCan3.Write("%s_%s_analyzed"%(histBaseName,trackName))
        #self.outfile.cd()
        #self.outdirs["obs_%s"%(obsName)][trackName].Write()

        #if getResiduals:
        #    myCan4 = r.TCanvas("%s_%s_residual"%(histBaseName,trackName),
        #                       "%s_%s_residual"%(histBaseName,trackName),
        #                       800,800)
        #    curvatureChi2[0].Draw("ep0")
        #    self.outdirs["obs_%s"%(obsName)][trackName].cd()
        #    myCan4.Write("%s_%s_residual"%(histBaseName,trackName))
        #    raw_input("enter to continue")

        for i in range(nBiasBins):
            obs_posBias = f.Get("%s%sCurvePlusBias%03d"%( histBaseName,obsName,i+1))
            obs_negBias = f.Get("%s%sCurveMinusBias%03d"%(histBaseName,obsName,i+1))
            obs_posBias.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
            obs_negBias.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
            # set bin content to 0 for bins outside the min pT cut
            print "%s%sCurvePlusBias%03d %d"%( histBaseName,obsName,i+1,obs_posBias.Integral())
            obs_posBias = self.setMinPT(obs_posBias,self.nTotalBins,200./self.factor,needsFlip)
            print "%s%sCurveMinusBias%03d %d"%(histBaseName,obsName,i+1,obs_negBias.Integral())
            obs_negBias = self.setMinPT(obs_negBias,self.nTotalBins,200./self.factor,needsFlip)
            obs_posBias.Rebin(self.rebins)
            obs_negBias.Rebin(self.rebins)
        
            ref_posBias = f.Get("%s%sCurvePlusBias%03d"%( histBaseName,refName,i+1))
            ref_negBias = f.Get("%s%sCurveMinusBias%03d"%(histBaseName,refName,i+1))
            ref_posBias.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
            ref_negBias.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
            print "%s%sCurvePlusBias%03d %d"%( histBaseName,refName,i+1,ref_posBias.Integral())
            ref_posBias = self.setMinPT(ref_posBias,self.nTotalBins,200./self.factor,needsFlip)
            print "%s%sCurveMinusBias%03d %d"%(histBaseName,refName,i+1,ref_negBias.Integral())
            ref_negBias = self.setMinPT(ref_negBias,self.nTotalBins,200./self.factor,needsFlip)
            ref_posBias.Rebin(self.rebins)
            ref_negBias.Rebin(self.rebins)

            if (i%100 == 0):
                self.outdirs["obs_%s"%(obsName)][trackName].cd()
                posBiasCan = r.TCanvas("%s_%s_pos_bias%03d_original"%(histBaseName,trackName,i+1),
                                       "%s_%s_pos_bias%03d_original"%(histBaseName,trackName,i+1),
                                       800,800)
                #posBiasCan.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
                obs_posBias.SetLineColor(r.kRed)
                obs_posBias.SetLineWidth(2)
                ref_posBias.SetLineColor(r.kBlue)
                ref_posBias.SetLineWidth(2)
                
                obs_posBias.Draw("ep0")
                ref_posBias.Draw("ep0sames")
                posBiasCan.Write("%s_%s_pos_bias%03d_original"%(histBaseName,trackName,i+1))

                negBiasCan = r.TCanvas("%s_%s_neg_bias%03d_original"%(histBaseName,trackName,i+1),
                                       "%s_%s_neg_bias%03d_original"%(histBaseName,trackName,i+1),
                                       800,800)
                #negBiasCan.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
                obs_negBias.SetLineColor(r.kRed)
                obs_negBias.SetLineWidth(2)
                ref_negBias.SetLineColor(r.kBlue)
                ref_negBias.SetLineWidth(2)
                
                obs_negBias.Draw("ep0")
                ref_negBias.Draw("ep0sames")
                self.outdirs["obs_%s"%(obsName)][trackName].cd()
                negBiasCan.Write("%s_%s_neg_bias%03d_original"%(histBaseName,trackName,i+1))

                #self.outfile.cd()
                #self.outdirs["obs_%s"%(obsName)][trackName].Write()

            if (obs_posBias.Integral()>0):
                rescale_pos = ref_posBias.Integral()/obs_posBias.Integral()
                rescale_neg = ref_negBias.Integral()/obs_negBias.Integral()
                print "Rescaling with factor %f/%f"%(rescale_pos,rescale_neg)
                obs_posBias.Scale(ref_posBias.Integral()/obs_posBias.Integral())
                obs_negBias.Scale(ref_negBias.Integral()/obs_negBias.Integral())
            
            if (i%100 == 0):
                self.outdirs["obs_%s"%(obsName)][trackName].cd()
                posBiasCan = r.TCanvas("%s_%s_pos_bias%03d_scaled"%(histBaseName,trackName,i+1),
                                       "%s_%s_pos_bias%03d_scaled"%(histBaseName,trackName,i+1),
                                       800,800)
                #posBiasCan.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
                obs_posBias.SetLineColor(r.kRed)
                obs_posBias.SetLineWidth(2)
                ref_posBias.SetLineColor(r.kBlue)
                ref_posBias.SetLineWidth(2)
                
                obs_posBias.Draw("ep0")
                ref_posBias.Draw("ep0sames")
                posBiasCan.Write("%s_%s_pos_bias%03d_scaled"%(histBaseName,trackName,i+1))

                negBiasCan = r.TCanvas("%s_%s_neg_bias%03d_scaled"%(histBaseName,trackName,i+1),
                                       "%s_%s_neg_bias%03d_scaled"%(histBaseName,trackName,i+1),
                                       800,800)
                #negBiasCan.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
                obs_negBias.SetLineColor(r.kRed)
                obs_negBias.SetLineWidth(2)
                ref_negBias.SetLineColor(r.kBlue)
                ref_negBias.SetLineWidth(2)
                
                obs_negBias.Draw("ep0")
                ref_negBias.Draw("ep0sames")
                self.outdirs["obs_%s"%(obsName)][trackName].cd()
                negBiasCan.Write("%s_%s_neg_bias%03d_scaled"%(histBaseName,trackName,i+1))

                #self.outfile.cd()
                #self.outdirs["obs_%s"%(obsName)][trackName].Write()
            
            if (needsFlip):
                obs_posBiasValsX = np.zeros(obs_posBias.GetNbinsX(),np.dtype('float64'))
                obs_negBiasValsX = np.zeros(obs_negBias.GetNbinsX(),np.dtype('float64'))
                obs_posBiasValsY = np.zeros(obs_posBias.GetNbinsX(),np.dtype('float64'))
                obs_negBiasValsY = np.zeros(obs_negBias.GetNbinsX(),np.dtype('float64'))
                obs_posBiasErrsY = np.zeros(obs_posBias.GetNbinsX(),np.dtype('float64'))
                obs_negBiasErrsY = np.zeros(obs_negBias.GetNbinsX(),np.dtype('float64'))
            
                for b in range(obs_posBias.GetNbinsX()):
                    obs_posBiasValsX[b] = b+1
                    obs_negBiasValsX[b] = b+1
                    obs_posBiasValsY[b] = obs_posBias.GetBinContent(b+1)
                    obs_negBiasValsY[b] = obs_negBias.GetBinContent(b+1)
                    obs_posBiasErrsY[b] = obs_posBias.GetBinError(b+1)
                    obs_negBiasErrsY[b] = obs_negBias.GetBinError(b+1)

                obs_posBiasValsYRev = np.fliplr([obs_posBiasValsY])[0]
                obs_negBiasValsYRev = np.fliplr([obs_negBiasValsY])[0]
                obs_posBiasErrsYRev = np.fliplr([obs_posBiasErrsY])[0]
                obs_negBiasErrsYRev = np.fliplr([obs_negBiasErrsY])[0]
                # obs_posBias.SetContent(obs_posBiasValsYRev)
                # obs_negBias.SetContent(obs_negBiasValsYRev)
                for b in range(obs_posBias.GetNbinsX()):
                    obs_posBias.SetBinContent(b+1,obs_posBiasValsYRev[b])
                    obs_negBias.SetBinContent(b+1,obs_negBiasValsYRev[b])
                    obs_posBias.SetBinError(b+1,obs_posBiasErrsYRev[b])
                    obs_negBias.SetBinError(b+1,obs_negBiasErrsYRev[b])
                
            if (i%100 == 0):
                self.outdirs["obs_%s"%(obsName)][trackName].cd()
                posBiasCan = r.TCanvas("%s_%s_pos_bias%03d_analyzed"%(histBaseName,trackName,i+1),
                                       "%s_%s_pos_bias%03d_analyzed"%(histBaseName,trackName,i+1),
                                       800,800)
                #posBiasCan.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
                obs_posBias.SetLineColor(r.kRed)
                obs_posBias.SetLineWidth(2)
                ref_posBias.SetLineColor(r.kBlue)
                ref_posBias.SetLineWidth(2)
                
                obs_posBias.Draw("ep0")
                ref_posBias.Draw("ep0sames")
                posBiasCan.Write("%s_%s_pos_bias%03d_analyzed"%(histBaseName,trackName,i+1))

                negBiasCan = r.TCanvas("%s_%s_neg_bias%03d_analyzed"%(histBaseName,trackName,i+1),
                                       "%s_%s_neg_bias%03d_analyzed"%(histBaseName,trackName,i+1),
                                       800,800)
                #negBiasCan.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName])
                obs_negBias.SetLineColor(r.kRed)
                obs_negBias.SetLineWidth(2)
                ref_negBias.SetLineColor(r.kBlue)
                ref_negBias.SetLineWidth(2)
                
                obs_negBias.Draw("ep0")
                ref_negBias.Draw("ep0sames")
                self.outdirs["obs_%s"%(obsName)][trackName].cd()
                negBiasCan.Write("%s_%s_neg_bias%03d_analyzed"%(histBaseName,trackName,i+1))

                #self.outfile.cd()
                #self.outdirs["obs_%s"%(obsName)][trackName].Write()
            
            biasVal = (i+1)*(factor*maxBias/nBiasBins)
            #not now#xVals["chi2"][nBiasBins+1+i] = biasVal
            #not now#positiveBias = self.calculateChi2(obs_posBias,ref_posBias,mode)
            #not now#yVals["chi2"][nBiasBins+1+i] = positiveBias[1]
            #if getResiduals:
            #    residualCan = r.TCanvas("%s_%s_bin%03d_residual"%(histBaseName,trackName,i+1),
            #                            "%s_%s_bin%03d_residual"%(histBaseName,trackName,i+1),
            #                            800,800)
            #    positiveBias[0].SetLineColor(r.kRed)
            #    positiveBias[0].SetLineWidth(2)
            #    positiveBias[0].SetMarkerColor(r.kRed)
            #    positiveBias[0].SetMarkerStyle(r.kFullDiamond)
            #    positiveBias[0].Draw("ep0")

            xVals["KS"][nBiasBins+1+i] = biasVal
            yVals["KS"][nBiasBins+1+i] = obs_posBias.KolmogorovTest(ref_posBias,"D")
            
            xVals["AD"][nBiasBins+1+i] = biasVal
            yVals["AD"][nBiasBins+1+i] = obs_posBias.AndersonDarlingTest(ref_posBias,"D")
            
            xVals["Chi2"][nBiasBins+1+i] = biasVal
            chi2Val = r.Double(0.) # necessary for pass-by-reference in python
            chi2ndf = r.Long(0)    # necessary for pass-by-reference in python
            igood   = r.Long(0)    # necessary for pass-by-reference in python
            resids  = np.zeros(self.nTotalBins,np.dtype('float64'))
            prob = obs_posBias.Chi2TestX(ref_posBias,chi2Val,chi2ndf,igood,histopts,resids)
            print "Chi2TestX: prob=%f, chi2=%f, chi2ndf=%d, igood=%d"%(prob,chi2Val,chi2ndf,igood)
            if chi2ndf > 0:
                yVals["Chi2"][nBiasBins+1+i] = chi2Val/(1.*chi2ndf)
            else:
                print "problem, no degrees of freedom"
                yVals["Chi2"][nBiasBins+1+i] = -1
            #yVals["Chi2"][nBiasBins+1+i] = obs_posBias.Chi2Test(ref_posBias,chi2opts,resids)

            if getResiduals:
                self.outdirs["obs_%s"%(obsName)][trackName+"residuals"].cd()
                plusResiduals = r.TH1D("%s_%s_plus_bin_%03d_residuals"%(histBaseName,trackName,i+1),
                                       "%s_%s_plus_bin_%03d_residuals"%(histBaseName,trackName,i+1),
                                       100,-5.,5.)
                plusResiduals.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName+"residuals"])
                for val in resids:
                    plusResiduals.Fill(val)
                #residualCanvas = r.TCanvas("%s_%s_residual"%(histBaseName,trackName),
                #                           "%s_%s_residual"%(histBaseName,trackName),
                #                           800,800)

                plusResiduals.Write("%s_%s_plus_bin_%03d_residuals"%(histBaseName,trackName,i+1))
                #plusResidualCan.Write("%s_%s_residual"%(histBaseName,trackName))
                #self.outfile.cd()
                #self.outdirs["obs_%s"%(obsName)][trackName+"residuals"].Write()

            
            ## negative injected bias
            #not nowxVals["chi2"][nBiasBins-(i+1)] = -1.*biasVal
            #not nownegativeBias = self.calculateChi2(obs_negBias,ref_negBias,mode)
            #not nowyVals["chi2"][nBiasBins-(i+1)] = negativeBias[1]
            #if getResiduals:
            #    negativeBias[0].SetLineColor(r.kBlue)
            #    negativeBias[0].SetLineWidth(2)
            #    negativeBias[0].SetMarkerColor(r.kBlue)
            #    negativeBias[0].SetMarkerStyle(r.kFullCross)
            #    negativeBias[0].Draw("ep0sames")

            ## ROOT methods
            xVals["KS"][nBiasBins-(i+1)] = -1.*biasVal
            yVals["KS"][nBiasBins-(i+1)] = obs_negBias.KolmogorovTest(ref_negBias,"D")
            
            xVals["AD"][nBiasBins-(i+1)] = -1.*biasVal
            yVals["AD"][nBiasBins-(i+1)] = obs_negBias.AndersonDarlingTest(ref_negBias,"D")
            
            xVals["Chi2"][nBiasBins-(i+1)] = -1.*biasVal
            chi2Val = r.Double(0.) # necessary for pass-by-reference in python
            chi2ndf = r.Long(0)    # necessary for pass-by-reference in python
            igood   = r.Long(0)    # necessary for pass-by-reference in python
            resids  = np.zeros(self.nTotalBins,np.dtype('float64'))
            prob = obs_negBias.Chi2TestX(ref_negBias,chi2Val,chi2ndf,igood,histopts,resids)
            print "Chi2TestX: prob=%f, chi2=%f, chi2ndf=%d, igood=%d"%(prob,chi2Val,chi2ndf,igood)
            if chi2ndf > 0:
                yVals["Chi2"][nBiasBins-(i+1)] = chi2Val/(1.*chi2ndf)
            else:
                print "problem, no degrees of freedom"
                yVals["Chi2"][nBiasBins-(i+1)] = -1
            #yVals["Chi2"][nBiasBins-(i+1)] = obs_negBias.Chi2Test(ref_negBias,chi2opts)
                
            if getResiduals:
                self.outdirs["obs_%s"%(obsName)][trackName+"residuals"].cd()
                minusResiduals = r.TH1D("%s_%s_minus_bin_%03d_residuals"%(histBaseName,trackName,i+1),
                                        "%s_%s_minus_bin_%03d_residuals"%(histBaseName,trackName,i+1),
                                        100,-5.,5.)
                minusResiduals.SetDirectory(self.outdirs["obs_%s"%(obsName)][trackName+"residuals"])
                for val in resids:
                    minusResiduals.Fill(val)
                #residualCanvas = r.TCanvas("%s_%s_residual"%(histBaseName,trackName),
                #                           "%s_%s_residual"%(histBaseName,trackName),
                #                           800,800)

                minusResiduals.Write("%s_%s_minus_bin_%03d_residuals"%(histBaseName,trackName,i+1))
                #minusResidualCan.Write("%s_%s_residual"%(histBaseName,trackName))
                #self.outfile.cd()
                #self.outdirs["obs_%s"%(obsName)][trackName+"residuals"].Write()

        print "xVals for ",f
        print xVals
        print "yVals for ",f
        print yVals
        graphs = {}
        self.outdirs["obs_%s"%(obsName)]["results"].cd()
        graphs["chi2"] = r.TGraph(xVals["chi2"].size,xVals["chi2"],yVals["chi2"])
        graphs["KS"  ] = r.TGraph(xVals["KS"].size,  xVals["KS"],  yVals["KS"]  )
        graphs["AD"  ] = r.TGraph(xVals["AD"].size,  xVals["AD"],  yVals["AD"]  )
        graphs["Chi2"] = r.TGraph(xVals["Chi2"].size,xVals["Chi2"],yVals["Chi2"])

        graphs["chi2"] = self.prettifyGraph(graphs["chi2"],self.graphInfo["chi2"])
        graphs["KS"  ] = self.prettifyGraph(graphs["KS"  ],self.graphInfo["KS"  ])
        graphs["AD"  ] = self.prettifyGraph(graphs["AD"  ],self.graphInfo["AD"  ])
        graphs["Chi2"] = self.prettifyGraph(graphs["Chi2"],self.graphInfo["Chi2"])
        return graphs

    def prettifyGraph(self, graph, graphParams):
        graph.SetLineColor(graphParams["color"])
        graph.SetLineWidth(4)
        graph.SetMarkerColor(graphParams["color"])
        graph.SetMarkerSize(2)
        graph.SetMarkerStyle(graphParams["marker"])
        graph.SetTitle(graphParams["title"])
        if (self.factor < 1000):
            graph.GetXaxis().SetTitle("#Delta#kappa_{b} [c/GeV]")
        else:
            graph.GetXaxis().SetTitle("#Delta#kappa_{b} [c/TeV]")
        graph.GetYaxis().SetTitle(graphParams["yaxis"])
        return graph

    def calculateChi2(self, hobs, href, mode, useNDF=False, debug=False):
        import math,ROOT as r
        if (hobs.GetNbinsX() != href.GetNbinsX()):
            print "histograms have different number of bins in X:"
            print hobs.GetName(),"has",hobs.GetNbinsX()
            print href.GetName(),"has",href.GetNbinsX()
            return -1.
        
        nBins = hobs.GetNbinsX()
        cChi2 = 0.
        ndf   = 0
        Obs = 0.
        Ref = 0.
        residuals = r.TH1D("residuals","residuals",100,-2.5,2.5)

        bins = []
        bincount = 0
        for b in range(nBins):
            obs = hobs.GetBinContent(b+1);
            ref = href.GetBinContent(b+1);
            
            if (ref > 0 and obs > 0):
                Obs += obs
                Ref += ref
                bincount += 1
                bins.append(b)

        if (debug) :
            print "DEBUG1 Obs = %f, Ref = %f, N=%d"%(Obs,Ref,bincount),bins
        # first loop done, move on to calculate the chi2
        for b in range(nBins):
            obs = hobs.GetBinContent(b+1);
            ref = href.GetBinContent(b+1);
            
            if (ref > 0 and obs > 0):
                obserr = hobs.GetBinError(b+1);
                referr = href.GetBinError(b+1);
                ndf   +=1
                
                if (mode == 0):
                    # my wrong method
                    binChi2 = ((obs-ref)**2)/ref
                
                elif (mode == 1):
                    # nick's method 1
                    uncsq = (obserr**2)+(referr**2)
                    chival = (Ref*obs)-(Obs*ref)
                    binChi2 = (chival**2)/(uncsq)

                elif (mode == 2 or mode == 3):
                    # nick's method 3
                    uncsq = 0
                    if (mode == 2):
                        uncsq = (obserr**2)+(referr**2)+(0.5*obserr*referr)
                    elif (mode == 3):
                        uncsq = 0.5*(obs+ref)
                    chival = obs-ref
                    binChi2 = (chival**2)/uncsq

                elif (mode == 4):
                    # nick's method 3, says same as ROOT
                    chival = (Ref*obs)-(Obs*ref)
                    binChi2 = (chival**2)/(obs+ref)
                
                ## do the sum
                cChi2  += binChi2
                piest = (obs+ref)/(Obs+Ref)

                N_res = (obs-(Obs*piest))
                D_term1 = Obs*piest
                D_term2 = 1-(Obs/(Obs+Ref))
                D_term3 = 1-(obs+ref)/(Obs+Ref)
                D_res = math.sqrt(D_term1*D_term2*D_term3)
                if not D_res or not Obs*Ref:
                    print "N_res(%f-(%f*%f)) = %f, D_term1 = %f, D_term2 = %f, D_term3 = %f"%(obs,Obs,
                                                                                              piest,
                                                                                              N_res,
                                                                                              D_term1,D_term2,D_term3)
                else:
                    residual = N_res/D_res
                    residuals.Fill(residual)
                if (debug) :
                    print "Bin%03d(mode=%d): binChi2 = %12.2f, Chi2 = %12.2f(%12.2f), ndf = %3d, residual = %3.4f"%(b,mode,
                                                                                                                    binChi2,
                                                                                                                    cChi2,
                                                                                                                    cChi2/(Obs*Ref),
                                                                                                                    ndf,
                                                                                                                    residual)

        if not Obs*Ref:
            errorCan = r.TCanvas("errorCan","errorCan",800,800)
            hobs.Draw("ep0")
            href.Draw("ep0sames")
            #raw_input("press enter to continue")
            return -1
                      
        if useNDF:
            cChi2 /= ndf

        if (mode == 0):
            return residuals,cChi2 # my wrong method
        elif (mode == 1):
            return residuals,cChi2/(Obs*Ref) # nick's method 1
        elif (mode == 2 or mode == 3):
            return residuals,cChi2 # nick's method 2
        elif (mode == 4):
            return residuals,cChi2/(Obs*Ref) # nick's method matching ROOT
        else:
            return -1

    def setMinPT(self, hist, nbins, minPt, symmetric=True):
        """Takes an input histogram and sets the bin content to 
        0 if q/pT is outside the range
        """
        print "nBinsX %d"%(hist.GetNbinsX())
        if symmetric:
            thebin = hist.FindBin(-1./minPt)
            while not (hist.GetXaxis().GetBinLowEdge(thebin) < -1./minPt):
                print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
                thebin -= 1
                print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
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
            print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
            thebin += 1
            print thebin, hist.GetXaxis().GetBinLowEdge(thebin), hist.GetXaxis().GetBinUpEdge(thebin)
        print "upper cut off %2.2f, bin %d, integral (first,bin) %d"%(1./minPt,
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
        print "nbins+1 content %d"%(hist.GetBinContent(nbins+1))
        print "nbins+2 content %d"%(hist.GetBinContent(nbins+2))

        return hist
