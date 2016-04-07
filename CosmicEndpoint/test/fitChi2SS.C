{
  TFile *file = new TFile("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/Data2MCSS.root", "READ");
  TFile *g = new TFile("chi2FitSS.root","RECREATE");
  std::cout << "The file reading is" << file << std::endl;

  std::cout << "\nThe file is open" <<   file->IsOpen()  << std::endl;

  std::cout << "\nThe file is Zombie" <<   file->IsZombie() << std::endl;
  //  TGraph *graph = (TGraph*)file->Get("TunePvCurve;1"); //file->Get("tunep_Chi2");
  TH1F *graph = (TH1F*)file->Get("TunePChi2vCurve;1"); //file->Get("tunep_Chi2");
  
  g->cd();


    TH1F* dataHistRebin = (TH1F*)file->Get("TunePdataHist_Rebinned;2");
    std::cout << "\nGot the first histo\n";

    
    double binWidth = dataHistRebin->GetBinWidth(3);
    double stepSize = 0.0005;


  std::cout << "\nThe graph to be read is " << graph << std::endl;
  TF1 *fitPoly2 = new TF1("fitPoly2","pol2", -1,1);
  TF1 *fitPoly6 = new TF1("fitPoly6", "pol6", -1, 1);
  TF1 *fitPoly8 = new TF1("fitPoly8", "pol8", -1, 1);

  std::cout << "\nThe fit to be used is is " << fitPoly2 << std::endl;
  fitPoly2->SetParameters(0,0,0);
  fitPoly6->SetParameters(0,0,0,0,0,0,0);
  fitPoly8->SetParameters(0,0,0,0,0,0,0,0,0);
  std::cout << "\nSet Parameters" << std::endl;
  
  
  graph->Fit("fitPoly2", "VEMFR", "", -0.04, 0.20);
  graph->Fit("fitPoly6", "VEMFR", "", -0.04, 0.20);
  graph->Fit("fitPoly8", "VEMFR", "", -0.04, 0.20);
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

 double minX2 = fitPoly2->GetMinimumX(- 0.01, 0.1, 1.E-10,100, 0);
 double minY2 = fitPoly2->GetMinimum(-0.01,0.1,1.E-10,100,0);
 std::cout << "\nMinX2 is: " << minX2;
 std::cout << "\nMinY2 is: " << minY2 << std::endl << std::endl;
 double minX6 = fitPoly6->GetMinimumX(- 0.01, 0.1, 1.E-10,100, 0);
 double minY6 = fitPoly6->GetMinimum(-0.01,0.1,1.E-10,100,0);
 std::cout << "\nMinX6 is: " << minX6;
 std::cout << "\nMinY6 is: " << minY6 << std::endl << std::endl;
 double minX8 = fitPoly8->GetMinimumX(- 0.01, 0.1, 1.E-10,100, 0);
 double minY8 = fitPoly8->GetMinimum(-0.01,0.1,1.E-10,100,0);
 std::cout << "\nMinX2 is: " << minX8;
 std::cout << "\nMinY2 is: " << minY8 << std::endl << std::endl;
 

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
 graph->SetTitle("Data_MC_P2_#chi^{2} vs #Delta#kappa");
 graph->GetXaxis()->SetTitle("#Delta#kappa [c/TeV]");
 graph->GetXaxis()->SetRangeUser(xLeft2 - 0.05, xRight2 + 0.05);
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
 


 TLegend* leg = new TLegend(0.2, 0.7, 0.60, 0.9);

 char minusBias2[10];
 sprintf(minusBias2, "-%2.5f", minusUn2);
 leftSigma << minusBias2;

 char plusBias2[10];
 sprintf(plusBias2, "+%2.5f", plusUn2);
 rightSigma << plusBias2;

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


 leg->AddEntry((TObject*) 0, TString("Injected bias step size: "+legendText1.str()+" [c/TeV]"), "");
 leg->AddEntry((TObject*) 0, TString("Curvature bin width is: "+legendText2.str())+" [c/TeV]", "");
 leg->SetTextSize(0.030);
 leg->Draw();


 pad->Update();
 can->Update();
 can->Write();
 can->SaveAs("~/public/html/cosmics/data_MCchi2FitP2SS.png");
 can->SaveAs("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/Chi2Plots/data_MCchi2FitP2SS.C");

 
 g->Write();
 g->Close();
 

}
