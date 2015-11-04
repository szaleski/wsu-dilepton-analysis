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
    
    def __init__(self, infiledir, outfile, maxbias, nBiasBins=100, factor=1, rebins=1) :
        import ROOT as r
        self.infiles = {}
        self.infiles["picky"] = r.TFile(infiledir+"/CosmicHistOut_Picky.root","r")
        self.infiles["dyt"  ] = r.TFile(infiledir+"/CosmicHistOut_DYT.root"  ,"r")
        self.infiles["tunep"] = r.TFile(infiledir+"/CosmicHistOut_TuneP.root","r")
        self.infiles["tpfms"] = r.TFile(infiledir+"/CosmicHistOut_TPFMS.root","r")
        self.outfile   = r.TFile(outfile,"recreate")
        self.outdirs   = {}
        self.outdirs["picky"]   = self.outfile.mkdir("picky")
        self.outdirs["dyt"  ]   = self.outfile.mkdir("dyt"  )
        self.outdirs["tunep"]   = self.outfile.mkdir("tunep")
        self.outdirs["tpfms"]   = self.outfile.mkdir("tpfms")

        self.maxbias   = maxbias
        self.nBiasBins = nBiasBins
        self.factor    = factor
        self.rebins    = rebins

        self.graphInfo = {}
        self.graphInfo["chi2"] = {"color":r.kBlue,   "marker":r.kFullTriangleUp,
                                  "title":"Calculated #chi^{2}",             "yaxis":""}
        self.graphInfo["KS"]   = {"color":r.kGreen-2,"marker":r.kFullDiamond,
                                  "title":"Kolmogorov test statistic",       "yaxis":""}
        self.graphInfo["AD"]   = {"color":r.kRed+2,  "marker":r.kFullCross,
                                  "title":"Anderson-Darling test statistic", "yaxis":""}
        self.graphInfo["Chi2"] = {"color":r.kCyan+3, "marker":r.kFullTriangleDown,
                                  "title":"ROOT #chi^{2}",                   "yaxis":""}
        return

    def runMinimization(self, histBaseName, obsName, refName, needsFlip):
        import ROOT as r
        self.outfile.cd()
        picky = self.makeGraph(self.infiles["picky"],histBaseName, obsName, refName, "picky", needsFlip)
        dyt   = self.makeGraph(self.infiles["dyt"  ],histBaseName, obsName, refName, "dyt"  , needsFlip)
        tunep = self.makeGraph(self.infiles["tunep"],histBaseName, obsName, refName, "tunep", needsFlip)
        tpfms = self.makeGraph(self.infiles["tpfms"],histBaseName, obsName, refName, "tpfms", needsFlip)

        for test in ["chi2","KS","AD","Chi2"]:
            self.outfile.cd()
            #self.outdirs["picky"].cd()
            pickyCanvas = r.TCanvas("picky_%s"%test,"picky_%s"%test,800,800)
            picky[test].Draw("AP")
            pickyCanvas.Write()
            #self.outfile.cd()
            #self.outdirs["picky"].Write()

            #self.outdirs["dyt"].cd()
            dytCanvas = r.TCanvas("dyt_%s"%test,"dyt_%s"%test,800,800)
            dyt[test].Draw("AP")
            dytCanvas.Write()
            #self.outfile.cd()
            #self.outdirs["dyt"].Write()

            #self.outdirs["tunep"].cd()
            tunepCanvas = r.TCanvas("tunep_%s"%test,"tunep_%s"%test,800,800)
            tunep[test].Draw("AP")
            tunepCanvas.Write()
            #self.outfile.cd()
            #self.outdirs["tunep"].Write()

            #self.outdirs["tpfms"].cd()
            tpfmsCanvas = r.TCanvas("tpfms_%s"%test,"tpfms_%s"%test,800,800)
            tpfms[test].Draw("AP")
            tpfmsCanvas.Write()
            #self.outfile.cd()
            #self.outdirs["tpfms"].Write()
            
            self.outfile.Write()

        for track in ["picky","dyt","tunep","tpfms"]:
            self.outdirs[track].cd()
            CounterCanvas = r.TCanvas("%sCounterCan"%track,"%sCounterCan"%track,800,800)
            CounterCanvas.cd()
            UpperCounters = self.infiles[track].Get("upperCounters")
            LowerCounters = self.infiles[track].Get("lowerCounters")
            UpperCounters.SetLineColor(r.kRed)
            UpperCounters.SetLineWidth(2)
            UpperCounters.SetMarkerStyle(r.kFullDiamond)
            LowerCounters.SetLineColor(r.kRed)
            LowerCounters.SetLineWidth(2)
            LowerCounters.SetMarkerStyle(r.kFullCross)
            UpperCounters.Draw("ep0")
            LowerCounters.Draw("ep0sames")
            CounterCanvas.Write()
            self.outfile.cd()
            self.outdirs[track].Write()

        self.outfile.Write()
        self.outfile.Close()
        return

    def makeGraph(self, f, histBaseName, obsName, refName, trackName, needsFlip):
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
        obs = obs.Rebin(self.rebins)
        ref = ref.Rebin(self.rebins)
        # print obs
        # print ref
        # print f

        myCan1 = r.TCanvas("%s_%s_original"%(histBaseName,trackName),
                           "%s_%s_original"%(histBaseName,trackName),
                           800,800)
        obs.SetLineColor(r.kRed)
        obs.SetLineWidth(2)
        ref.SetLineColor(r.kBlue)
        ref.SetLineWidth(2)

        obs.Draw("ep0")
        ref.Draw("ep0sames")
        self.outdirs[trackName].cd()
        myCan1.Write()

        if (obs.Integral()>0):
            obs.Scale(ref.Integral()/obs.Integral())
        
        myCan2 = r.TCanvas("%s_%s_scaled"%(histBaseName,trackName),
                           "%s_%s_scaled"%(histBaseName,trackName),
                           800,800)
        obs.SetLineColor(r.kRed)
        obs.SetLineWidth(2)
        ref.SetLineColor(r.kBlue)
        ref.SetLineWidth(2)

        obs.Draw("ep0")
        ref.Draw("ep0sames")
        self.outdirs[trackName].cd()
        myCan2.Write()

        if (needsFlip):
            # loop through bins getting the contents of each bin
            # create an array
            # reverse the array
            # call obs.SetContent(array)
            obsValsX = np.zeros(obs.GetNbinsX(),np.dtype('float64'))
            obsValsY = np.zeros(obs.GetNbinsX(),np.dtype('float64'))
            for b in range(obs.GetNbinsX()):
                obsValsX[b] = b+1
                obsValsY[b] = obs.GetBinContent(b+1)
                
            obsValsYRev = np.fliplr([obsValsY])[0]
            # obs.SetContent(obsValsYRev) ## doesn't work for some reason, oh well, hack it
            for b in range(obs.GetNbinsX()):
                obs.SetBinContent(b+1,obsValsYRev[b])

        #print obs, ref

        mode = 0
        #self.calculateChi2(obs,ref,0,False,True)
        #self.calculateChi2(obs,ref,1,False,True)
        #self.calculateChi2(obs,ref,2,False,True)
        #self.calculateChi2(obs,ref,3,False,True)
        # should perhaps return chi2/ndof?
        xVals["chi2"][nBiasBins] = 0.
        curvatureChi2 = self.calculateChi2(obs,ref,mode)
        yVals["chi2"][nBiasBins] = curvatureChi2[1]

        # these are seeming to not work...
        xVals["KS"][nBiasBins] = 0.
        yVals["KS"][nBiasBins] = obs.KolmogorovTest(ref,"D")

        xVals["AD"][nBiasBins] = 0.
        yVals["AD"][nBiasBins] = obs.AndersonDarlingTest(ref,"D")

        xVals["Chi2"][nBiasBins] = 0.
        #th1residuals = []
        yVals["Chi2"][nBiasBins] = obs.Chi2Test(ref,"PCHI2/NDF")
                
        myCan3 = r.TCanvas("%s_%s_analyzed"%(histBaseName,trackName),
                           "%s_%s_analyzed"%(histBaseName,trackName),
                           800,800)
        obs.SetLineColor(r.kRed)
        obs.SetLineWidth(2)
        ref.SetLineColor(r.kBlue)
        ref.SetLineWidth(2)

        obs.Draw("ep0")
        ref.Draw("ep0sames")
        self.outdirs[trackName].cd()
        myCan3.Write()

        myCan4 = r.TCanvas("%s_%s_residual"%(histBaseName,trackName),
                           "%s_%s_residual"%(histBaseName,trackName),
                           800,800)
        curvatureChi2[0].Draw("ep0")
        self.outdirs[trackName].cd()
        myCan4.Write()
        #raw_input("enter to continue")

        for i in range(nBiasBins):
            obs_posBias = f.Get("%s%sCurvePlusBias%03d"%( histBaseName,obsName,i+1))
            obs_negBias = f.Get("%s%sCurveMinusBias%03d"%(histBaseName,obsName,i+1))
            obs_posBias.Rebin(self.rebins)
            obs_negBias.Rebin(self.rebins)
        
            ref_posBias = f.Get("%s%sCurvePlusBias%03d"%( histBaseName,refName,i+1))
            ref_negBias = f.Get("%s%sCurveMinusBias%03d"%(histBaseName,refName,i+1))
            ref_posBias.Rebin(self.rebins)
            ref_negBias.Rebin(self.rebins)

            obs_posBiasValsX = np.zeros(obs_posBias.GetNbinsX(),np.dtype('float64'))
            obs_negBiasValsX = np.zeros(obs_negBias.GetNbinsX(),np.dtype('float64'))
            obs_posBiasValsY = np.zeros(obs_posBias.GetNbinsX(),np.dtype('float64'))
            obs_negBiasValsY = np.zeros(obs_negBias.GetNbinsX(),np.dtype('float64'))
            for b in range(obs_posBias.GetNbinsX()):
                obs_posBiasValsX[b] = b+1
                obs_negBiasValsX[b] = b+1
                obs_posBiasValsY[b] = obs_posBias.GetBinContent(b+1)
                obs_negBiasValsY[b] = obs_negBias.GetBinContent(b+1)

            if (needsFlip):
                obs_posBiasValsYRev = np.fliplr([obs_posBiasValsY])[0]
                obs_negBiasValsYRev = np.fliplr([obs_negBiasValsY])[0]
                # obs_posBias.SetContent(obs_posBiasValsYRev)
                # obs_negBias.SetContent(obs_negBiasValsYRev)
                for b in range(obs_posBias.GetNbinsX()):
                    obs_posBias.SetBinContent(b+1,obs_posBiasValsYRev[b])
                    obs_negBias.SetBinContent(b+1,obs_negBiasValsYRev[b])
                
            if (obs_posBias.Integral()>0):
                obs_posBias.Scale(ref_posBias.Integral()/obs_posBias.Integral())
                obs_negBias.Scale(ref_negBias.Integral()/obs_negBias.Integral())
            
            biasVal = (i+1)*(factor*maxBias/nBiasBins)
            xVals["chi2"][nBiasBins+1+i] = biasVal
            positiveBias = self.calculateChi2(obs_posBias,ref_posBias,mode)
            yVals["chi2"][nBiasBins+1+i] = positiveBias[1]
            residualCan = r.TCanvas("%s_%s_bin%03d_residual"%(histBaseName,trackName,i),
                                    "%s_%s_bin%03d_residual"%(histBaseName,trackName,i),
                                    800,800)
            positiveBias[0].SetLineColor(r.kRed)
            positiveBias[0].SetLineWidth(2)
            positiveBias[0].SetMarkerColor(r.kRed)
            positiveBias[0].SetMarkerStyle(r.kFullDiamond)
            positiveBias[0].Draw("ep0")

            xVals["KS"][nBiasBins+1+i] = biasVal
            yVals["KS"][nBiasBins+1+i] = obs_posBias.KolmogorovTest(ref_posBias,"D")
            
            xVals["AD"][nBiasBins+1+i] = biasVal
            yVals["AD"][nBiasBins+1+i] = obs_posBias.AndersonDarlingTest(ref_posBias,"D")
            
            xVals["Chi2"][nBiasBins+1+i] = biasVal
            yVals["Chi2"][nBiasBins+1+i] = obs_posBias.Chi2Test(ref_posBias,"PCHI2/NDF")
        
            xVals["chi2"][nBiasBins-(i+1)] = -1.*biasVal
            negativeBias = self.calculateChi2(obs_negBias,ref_negBias,mode)
            yVals["chi2"][nBiasBins-(i+1)] = negativeBias[1]
            negativeBias[0].SetLineColor(r.kBlue)
            negativeBias[0].SetLineWidth(2)
            negativeBias[0].SetMarkerColor(r.kBlue)
            negativeBias[0].SetMarkerStyle(r.kFullCross)
            negativeBias[0].Draw("ep0sames")

            self.outdirs[trackName].cd()
            #residualCan.Write()
            
            xVals["KS"][nBiasBins-(i+1)] = -1.*biasVal
            yVals["KS"][nBiasBins-(i+1)] = obs_negBias.KolmogorovTest(ref_negBias,"D")
            
            xVals["AD"][nBiasBins-(i+1)] = -1.*biasVal
            yVals["AD"][nBiasBins-(i+1)] = obs_negBias.AndersonDarlingTest(ref_negBias,"D")
            
            xVals["Chi2"][nBiasBins-(i+1)] = -1.*biasVal
            yVals["Chi2"][nBiasBins-(i+1)] = obs_negBias.Chi2Test(ref_negBias,"PCHI2/NDF")

        print "xVals for ",f
        print xVals
        print "yVals for ",f
        print yVals
        graphs = {}
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
                if not D_res:
                    print "N_res(%f-(%f*%f)) = %f, D_term1 = %f, D_term2 = %f, D_term3 = %f"%(obs,Obs,piest,N_res,D_term1,D_term2,D_term3)
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
