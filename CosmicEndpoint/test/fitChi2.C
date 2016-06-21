{
  std::string const& rootFile = "CRAFTvStartup";
  //  std::string const& rootFile = "InterfillvAsym";
  //  std::string const& rootFile = "StartupvAsym";
  //std::string const& rootFile = "CRAFTvInterfill";
  std::string const& outFile = "CRAFTvStartupFit";
  //std::string const& outFile = "InterfillvAsymFit";
  //std::string const& outFile = "StartupvAsym";
  //std::string const& outFile = "CRAFTvInterfill";
  double minFit = -0.5;
  double maxFit = 0.5;
  TFile *file = new TFile(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+rootFile+".root"), "READ");
  TFile *g = new TFile(TString(outFile+".root"),"RECREATE");
  std::cout << "The file reading is" << file << std::endl;
  
  //  std::cout << "\nThe file is open" <<   file->IsOpen()  << std::endl;
  
  //std::cout << "\nThe file is Zombie" <<   file->IsZombie() << std::endl;
  //  TGraph *graph = (TGraph*)file->Get("TunePvCurve;1"); //file->Get("tunep_Chi2");
  TH1F *graph = (TH1F*)file->Get("TunePChi2vCurve;1"); //file->Get("tunep_Chi2");
  
  g->cd();
  
  
  TH1F* dataHist = (TH1F*)file->Get("TunePdataHist_Rebinned"); //Use for DatavMC
  //TH1F* dataHist = (TH1F*)file->Get("dataHist"); //Use for MCvMC and DatavData
  std::cout << "\nGot the first histo\n";
  
  
  double binWidth = dataHist->GetBinWidth(3);
  double stepSize = 0.04;
  
  
  std::cout << "\nThe graph to be read is " << graph << std::endl;
  TF1 *fitPoly2 = new TF1("fitPoly2","pol2", -1,1);
  TF1 *fitPoly6 = new TF1("fitPoly6", "pol6", -1, 1);
  TF1 *fitPoly8 = new TF1("fitPoly8", "pol8", -1, 1);
  
  std::cout << "\nThe fit to be used is is " << fitPoly2 << std::endl;
  fitPoly2->SetParameters(0,0,0);
  fitPoly6->SetParameters(0,0,0,0,0,0,0);
  fitPoly8->SetParameters(0,0,0,0,0,0,0,0,0);
  std::cout << "\nSet Parameters" << std::endl;
  
  
  graph->Fit("fitPoly2", "VEMFR", "", minFit, maxFit);
  graph->Fit("fitPoly6", "VEMFR", "", minFit, maxFit);
  graph->Fit("fitPoly8", "VEMFR", "", minFit, maxFit);
  std::cout << "\nFit the polynomial" << std::endl;
  
  graph->Draw("alp");
  std::cout << "\nDrew graph" << std::endl;
  fitPoly2->Draw("sames");
  fitPoly6->Draw("sames");
  fitPoly8->Draw("sames");
  std::cout << "\nDrew the Fit" << std::endl;
  
  graph->Write();
  fitPoly2->Write();
  fitPoly6->Write();
  fitPoly8->Write();
  
  double minX2 = fitPoly2->GetMinimumX(minFit, maxFit, 1.E-10,100, 0);
  double minY2 = fitPoly2->GetMinimum(minFit,maxFit,1.E-10,100,0);
  std::cout << "\nMinX2 is: " << minX2;
  std::cout << "\nMinY2 is: " << minY2 << std::endl << std::endl;
  double minX6 = fitPoly6->GetMinimumX(minFit, maxFit, 1.E-10,100, 0);
  double minY6 = fitPoly6->GetMinimum(minFit,maxFit,1.E-10,100,0);
  std::cout << "\nMinX6 is: " << minX6;
  std::cout << "\nMinY6 is: " << minY6 << std::endl << std::endl;
  double minX8 = fitPoly8->GetMinimumX(minFit, maxFit, 1.E-10,100, 0);
  double minY8 = fitPoly8->GetMinimum(minFit,maxFit,1.E-10,100,0);
  std::cout << "\nMinX2 is: " << minX8;
  std::cout << "\nMinY2 is: " << minY8 << std::endl << std::endl;
  
  double leftEdge = minFit - 0.02;
  double rightEdge = maxFit +0.02;

  Double_t xLeft2 = 0.0;
  Double_t xRight2 = 0.0;
  
  Double_t xLeft6 = 0.0;
  Double_t xRight6 = 0.0;
  
  Double_t xLeft8 = 0.0;
  Double_t xRight8 = 0.0; 
  xLeft2 = fitPoly2->GetX(minY2+1, 0.00, minX2, 1.E-10, 100, 0);
  xRight2 = fitPoly2->GetX(minY2+1, minX2, 0.15, 1.E-10, 100, 0);
  xLeft6 = fitPoly6->GetX(minY6+1, 0.00, minX6, 1.E-10, 100, 0);
  xRight6 = fitPoly6->GetX(minY6+1, minX6, 0.15, 1.E-10, 100, 0); 
  xLeft8 = fitPoly8->GetX(minY8+1, 0.00, minX8, 1.E-10, 100, 0);
  xRight8 = fitPoly8->GetX(minY8+1, minX8, 0.15, 1.E-10, 100, 0);
  
  
  Double_t minSet2 = minY2 * 0.95;
  Double_t maxSet2 = (minY2 + 1);
  
  Double_t minSet6 = minY6 * 0.95;
  Double_t maxSet6 = (minY6 + 1);
  
  Double_t minSet8 = minY8 * 0.95;
  Double_t maxSet8 = (minY8 + 1);
 
  Double_t lineYmax2 = maxSet2;
  Double_t lineYmin2 = minSet2;
  
  Double_t lineYmax6 = maxSet6;
  Double_t lineYmin6 = minSet6;
  
  Double_t lineYmax8 = maxSet8;
  Double_t lineYmin8 = minSet8; 
  std::cout << "\n\n\nThe left--right value of minY2+1 are: " << xLeft2 << "--" << xRight2 << std::endl;
  std::cout << "\n\n\nThe left--right value of minY6+1 are: " << xLeft6 << "--" << xRight6 << std::endl;
  std::cout << "\n\n\nThe left--right value of minY8+1 are: " << xLeft8 << "--" << xRight8 << std::endl;
  
  
  
  TCanvas *can = new TCanvas("Canvas", "Canvas", 1600, 900);
  TPad *pad = new TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0);
  
  can->cd();
  pad->Draw();
  pad->cd();
  graph->Draw("p0");
  graph->SetMarkerColor(kBlack);
  graph->SetLineColor(kBlack);
  graph->SetMarkerStyle(7);
  graph->SetMarkerSize(100);
  graph->SetTitle("#chi^{2} vs #Delta#kappa_{bias}");
  graph->GetXaxis()->SetTitle("#Delta#kappa [c/TeV]");
  graph->GetXaxis()->SetRangeUser(leftEdge, rightEdge);
  graph->GetYaxis()->SetTitle("#chi^{2}");
  graph->SetStats(0);
  pad->Update();
  can->Update();
  
  
  fitPoly2->Draw("sames");
  fitPoly2->SetLineColor(kGreen+2);
  
  std::cout << "\nDrew quadratic Fit\n";
  
  
  
  fitPoly6->Draw("sames");
  fitPoly6->SetLineColor(kRed+2);
  std::cout << "\nDrew 6th order fit!\n";
  
  fitPoly8->Draw("sames");
  fitPoly8->SetLineColor(kAzure);
  
  pad->Update();
  can->Update();
  
  //TFitResultsPtr p2 = graph->Fit("pol2", "S");
  //  Int_t fitStatus = graphFit("pol2", "S");
  Double_t p2Par0 = fitPoly2->GetParameter(0);
  Double_t p2Par1 = fitPoly2->GetParameter(1);
  Double_t p2Par2 = fitPoly2->GetParameter(2);

  Double_t p6Par0 = fitPoly6->GetParameter(0);
  Double_t p6Par1 = fitPoly6->GetParameter(1);
  Double_t p6Par2 = fitPoly6->GetParameter(2);
  Double_t p6Par3 = fitPoly6->GetParameter(3);
  Double_t p6Par4 = fitPoly6->GetParameter(4);
  Double_t p6Par5 = fitPoly6->GetParameter(5);
  Double_t p6Par6 = fitPoly6->GetParameter(6);

  Double_t p8Par0 = fitPoly8->GetParameter(0);
  Double_t p8Par1 = fitPoly8->GetParameter(1);
  Double_t p8Par2 = fitPoly8->GetParameter(2);
  Double_t p8Par3 = fitPoly8->GetParameter(3);
  Double_t p8Par4 = fitPoly8->GetParameter(4);
  Double_t p8Par5 = fitPoly8->GetParameter(5);
  Double_t p8Par6 = fitPoly8->GetParameter(6);
  Double_t p8Par7 = fitPoly8->GetParameter(7);
  Double_t p8Par8 = fitPoly8->GetParameter(8);

  std::cout << "\n\nThe fit parameters are: " << p2Par0 << "\t" << p2Par1 << "\t" << p2Par2 << std::endl;
  std::cout << "\n\nThe fit parameters are: " << p6Par0 << "\t" << p6Par1 << "\t" << p6Par2 << "\t" << p6Par3 << "\t" << p6Par4 <<
	    "\t" << p6Par5 << "\t" << p6Par6 << std::endl;
  std::cout << "\n\nThe fit parameters are: " << p8Par0 << "\t" << p8Par1 << "\t" << p8Par2 << "\t" << p8Par3 << "\t" << p8Par4 <<
	    "\t" << p8Par5 << "\t" << p8Par6 << "\t" << p8Par7 << "\t" << p8Par8 <<  std::endl;

  
  
  std::stringstream leftSigma;
  std::stringstream rightSigma;
  std::stringstream centralVal;
  std::stringstream legendText1;
  std::stringstream legendText2;
  
  
  leftSigma.str( std::string() );
  leftSigma.clear();
  rightSigma.str( std::string() );
  rightSigma.clear();
  centralVal.str( std::string() );
  centralVal.clear();
  
  legendText1.str( std::string() );
  legendText1.clear();
  legendText1 << stepSize;
  legendText2.str( std::string() );
  legendText2.clear();
  legendText2 << binWidth;
 
  double minusUn2 = minX2 - xLeft2;
  double plusUn2 = xRight2 - minX2;
  double minusUn6 = minX6 - xLeft6;
  double plusUn6 = xRight6 - minX6;
  double minusUn8 = minX8 - xLeft8;
  double plusUn8 = xRight8 - minX8;
  
  

  TLegend* leg = new TLegend(0.2, 0.6, 0.60, 0.9);
  TLegend* legP2 = new TLegend(0.6, 0.6, 0.7, 0.9);
  TLegend* legP6 = new TLegend(0.7, 0.6, 0.8, 0.9);
  TLegend* legP8 = new TLegend(0.8, 0.6, 0.9, 0.9);

  std::stringstream p2P0;
  std::stringstream p2P1;
  std::stringstream p2P2;

  std::stringstream p6P0;
  std::stringstream p6P1;
  std::stringstream p6P2;
  std::stringstream p6P3;
  std::stringstream p6P4;
  std::stringstream p6P5;
  std::stringstream p6P6;

  std::stringstream p8P0;
  std::stringstream p8P1;
  std::stringstream p8P2;
  std::stringstream p8P3;
  std::stringstream p8P4;
  std::stringstream p8P5;
  std::stringstream p8P6;
  std::stringstream p8P7;
  std::stringstream p8P8;

  char poly2P0[15];
  sprintf(poly2P0, "%2.2f", p2Par0);
  p2P0 << "p0: " << poly2P0;

  char poly2P1[15];
  sprintf(poly2P1, "%2.2f", p2Par1);
  p2P1 << "p1: " << poly2P1;

  
  char poly2P2[15];
  sprintf(poly2P2, "%2.2f", p2Par2);
  p2P2 << "p2: " << poly2P2;

  char poly6P0[15];
  sprintf(poly6P0, "%2.2f", p6Par0);
  p6P0 << "p0: " << poly6P0;

  char poly6P1[15];
  sprintf(poly6P1, "%2.2f", p6Par1);
  p6P1 << "p1: " << poly6P1;

  
  char poly6P2[15];
  sprintf(poly6P2, "%2.2f", p6Par2);
  p6P2 << "p2: " << poly6P2;

  char poly6P3[15];
  sprintf(poly6P3, "%2.2f", p6Par3);
  p6P3 << "p3: " << poly6P3;

  char poly6P4[15];
  sprintf(poly6P4, "%2.2f", p6Par4);
  p6P4 << "p4: " << poly6P4;

  
  char poly6P5[15];
  sprintf(poly6P5, "%2.2f", p6Par5);
  p6P5 << "p5: " << poly6P5;

  char poly6P6[15];
  sprintf(poly6P6, "%2.2f", p6Par6);
  p6P6 << "p6: " << poly6P6;


  char poly8P0[15];
  sprintf(poly8P0, "%2.2f", p8Par0);
  p8P0 << "p0: " << poly8P0;

  char poly8P1[15];
  sprintf(poly8P1, "%2.2f", p8Par1);
  p8P1 << "p1: " << poly8P1;

  
  char poly8P2[15];
  sprintf(poly8P2, "%2.2f", p8Par2);
  p8P2 << "p2: " << poly8P2;

  char poly8P3[15];
  sprintf(poly8P3, "%2.2f", p8Par3);
  p8P3 << "p3: " << poly8P3;

  char poly8P4[15];
  sprintf(poly8P4, "%2.2f", p8Par4);
  p8P4 << "p4: " << poly8P4;

  
  char poly8P5[15];
  sprintf(poly8P5, "%2.2f", p8Par5);
  p8P5 << "p5: " << poly8P5;

  char poly8P6[15];
  sprintf(poly8P6, "%2.2f", p8Par6);
  p8P6 << "p6: " << poly8P6;

  char poly8P7[15];
  sprintf(poly8P7, "%2.2f", p8Par7);
  p8P7 << "p7: " << poly8P7;

  char poly8P8[15];
  sprintf(poly8P8, "%2.2f", p8Par8);
  p8P8 << "p8: " << poly8P8;

  legP2->AddEntry((TObject *) 0, "Quadratic Fit", "");
  legP2->AddEntry((TObject *) 0, TString(p2P0.str()), "");
  legP2->AddEntry((TObject *) 0, TString(p2P1.str()), "");
  legP2->AddEntry((TObject *) 0, TString(p2P2.str()), "");
  legP2->SetTextSize(0.020);
  legP2->SetTextColor(kGreen+2);
  legP2->SetFillStyle(3000);
  legP2->Draw();

  legP6->AddEntry((TObject *) 0, "6^{th} degree Fit", "");
  legP6->AddEntry((TObject *) 0, TString(p6P0.str()), "");
  legP6->AddEntry((TObject *) 0, TString(p6P1.str()), "");
  legP6->AddEntry((TObject *) 0, TString(p6P2.str()), "");
  legP6->AddEntry((TObject *) 0, TString(p6P3.str()), "");
  legP6->AddEntry((TObject *) 0, TString(p6P4.str()), "");
  legP6->AddEntry((TObject *) 0, TString(p6P5.str()), "");
  legP6->AddEntry((TObject *) 0, TString(p6P6.str()), "");

  legP6->SetTextSize(0.020);
  legP6->SetTextColor(kRed+2);
  legP6->SetFillStyle(3000);
  legP6->Draw();

  legP8->AddEntry((TObject *) 0, "8^{th} degree Fit", "");
  legP8->AddEntry((TObject *) 0, TString(p8P0.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P1.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P2.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P3.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P4.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P5.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P6.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P7.str()), "");
  legP8->AddEntry((TObject *) 0, TString(p8P8.str()), "");

  legP8->SetTextSize(0.020);
  legP8->SetTextColor(kAzure);
  legP8->SetFillStyle(3000);
  legP8->Draw();

  char minusBias2[10];
  sprintf(minusBias2, "-%2.5f", minusUn2);
  leftSigma << minusBias2;
  
  char plusBias2[10];
  sprintf(plusBias2, "+%2.5f", plusUn2);
  rightSigma << plusBias2;
  
  
  leg->AddEntry((TObject*) 0, TString("Injected bias step size: "+legendText1.str()+" [c/TeV]"), "");
  leg->AddEntry((TObject*) 0, TString("Curvature bin width is: "+legendText2.str())+" [c/TeV]", "");
  
  
  char centralBias2[10];
  sprintf(centralBias2, "%2.5f", minX2);
  centralVal << centralBias2;
  leg->AddEntry(fitPoly2, TString("P2 #Delta#kappa_{min} = "+ centralVal.str() + " _{" + leftSigma.str() + "} ^{" + rightSigma.str() + "} [c/TeV]"), "l");
  
  char minusBias6[10];
  sprintf(minusBias6, "-%2.5f", minusUn6);
  leftSigma.str( std::string() );
  leftSigma.clear();
  leftSigma << minusBias6;
  
  char plusBias6[10];
  sprintf(plusBias6, "+%2.5f", plusUn6);
  rightSigma.str( std::string() );
  rightSigma.clear();
  rightSigma << plusBias6;
  
  char centralBias6[10];
  sprintf(centralBias6, "%2.5f", minX6);
  centralVal.str( std::string() );
  centralVal.clear();
  centralVal << centralBias6;
  leg->AddEntry(fitPoly6, TString("P6 #Delta#kappa_{min} = "+ centralVal.str() + " _{" + leftSigma.str() + "} ^{" + rightSigma.str() + "} [c/TeV]"), "l");

  
  char minusBias8[10];
  sprintf(minusBias8, "-%2.5f", minusUn8);
  leftSigma.str( std::string() );
  leftSigma.clear();
  leftSigma << minusBias8;
  
  char plusBias8[10];
  sprintf(plusBias8, "+%2.5f", plusUn8);
  rightSigma.str( std::string() );
  rightSigma.clear();
  rightSigma << plusBias8;
  
  char centralBias8[10];
  sprintf(centralBias8, "%2.5f", minX8);
  centralVal.str( std::string() );
  centralVal.clear();
  centralVal << centralBias8;
  leg->AddEntry(fitPoly8, TString("P8 #Delta#kappa_{min} = "+ centralVal.str() + " _{" + leftSigma.str() + "} ^{" + rightSigma.str() + "} [c/TeV]"), "l");
  
  
  leg->SetTextSize(0.030);
  leg->SetFillStyle(3000);
  leg->Draw();
  
  
  pad->Update();
  can->Update();
  can->Write();
  can->SaveAs(TString("~/public/html/cosmics/April16/"+outFile+".png"));
  can->SaveAs("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/Chi2Plots/data_MCchi2FitP2.C");
  
  
  g->Write();
  g->Close();
  

}
