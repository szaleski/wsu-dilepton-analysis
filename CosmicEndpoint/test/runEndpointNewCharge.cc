#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"

#include "TLorentzVector.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TTreeReaderValue.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TString.h"
#include "TChain.h"
#include "TLegend.h"
#include "TDirectory.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>

void loopBias(std::string const& inDir1, std::string const& inDir2, TFile *oFile, int nBiasBins, int rebins, std::string const& histBase, TH1F *chi2Hist, std::string const& algo, std::string const& comp);//, TDirectory *dir);
void chargeLoopBias(std::string const& inDir1, std::string const& inDir2, TFile *oFile, int nBiasBins, int rebins, std::string const& histBase, TH1F *chi2Hist, std::string const& algo, std::string const& comp);


void runEndpointNewCharge(std::string const& inDir1, std::string const& inDir2, std::string const& outFile, int nBiasBins, int rebins, std::string const& histBase, std::string const& comp){


  //Create output .root file to write to
  TFile *oFile;
  TFile *oFile2;
  oFile = new TFile(TString(outFile+"Added.root"), "RECREATE");
  oFile2 = new TFile(TString(outFile+"Qsep.root"), "RECREATE");
  
  std::string algo;

  //Start to loop over for each track algorithm
  //1:TrackerOnly
  //2:TPMS
  //3:DYT
  //4:Picky
  //5:TuneP
  //Then create a histogram for each algorithm.
  /*
  for(int z = 1; z < 6; z++){
    if(z == 1){
      algo = "TrackerOnly";
      //TDirectory *dir = oFile->mkdir(TString(algo));
    }
    else if(z == 2){
      algo = "TPFMS";
      //TDirectory *dir = oFile->mkdir(TString(algo));
    }
    else if(z == 3){
      algo = "DYT";
      //TDirectory *dir = oFile->mkdir(TString(algo));
    }
    else if(z == 4){
      algo = "Picky";
      //TDirectory *dir = oFile->mkdir(TString(algo));
    }
    else if(z == 5){
      algo = "TuneP";
      //TDirectory *dir = oFile->mkdir(TString(algo));
    }
    
  */
  algo = "TuneP";
    TH1F *chi2Hist = new TH1F(TString("Chi2_"+algo), TString("Chi2_"+algo),100 , 0., 150);
    //    loopBias(inDir1, inDir2, oFile, nBiasBins, rebins, histBase, chi2Hist, algo, comp);//, dir);
    chargeLoopBias(inDir1, inDir2, oFile2, nBiasBins, rebins, histBase, chi2Hist, algo, comp);//, dir);
    //  }

  //Write the file and close.
  oFile->Write();
  oFile->Close();
  oFile2->Write();
  oFile2->Close();

 

  


  return;  //End program.
  
}

void loopBias(std::string const& inDir1, std::string const& inDir2, TFile *oFile, int nBiasBins, int rebins, std::string const& histBase, TH1F *chi2Hist, std::string const& algo, std::string const& comp){//, TDirectory dir){


  //FIX FILE OPEN NAME
    //Open the files. If errors return.
  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inDir1+"/CosmicHistOut_"+algo+".root"));
  if(f == 0){
    std::cout << "Error: cannot open file1! \nMake sure that you specified directory without '/' !\n";
    return;
  }
  
  std::cout << "Successfully opened file 1!\n";
  //FIX FILE OPEN NAME
  TFile *g = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inDir2+"/CosmicHistOut_"+algo+".root"));
  
  if(g == 0){
      std::cout << "Error: cannot open file1!\nMake sure that you specified directory without '/'!\n";
      return;
    }
  
    std::cout << "\nSuccessfully opened file 2!\n";

    
    Double_t binWidth = 0.0;
    int iNegStart = (-1) * nBiasBins;
    int iPosEnd = nBiasBins;
    int isPos = 0;
    int bias = 0;
    int numBins = ((2*nBiasBins) + 1);
    int bin = 0;
    Double_t chi2[numBins+1]; //passed by reference for chi2testx
    for(int k = 0; k < numBins; k++){
      chi2[k] = -1000;
    }
    Int_t ndof[numBins+1]; //passes by reference for chi2testx
    for(int k = 0; k < numBins; k++){
      ndof[k] = -1000;
    }
    Double_t ks[numBins+1];
    for(int k = 0; k < numBins; k++){
      ks[k] = -1000;
    }


    Double_t pVal = 0.0;

    


    Int_t numPlus = 0;
    Int_t numMinus = 0;
    Double_t numDataEntries = 0.0;

    Double_t numMC = 0.0;
    Double_t scale = 0.0;
    Double_t scaledEntries = 0.0;
    Double_t newMC = 0.0;
    Double_t oldMC = 0.0;
    Double_t dataInt = 0.0;
    Double_t mcInt = 0.0;
    Int_t igood;
    Double_t minChi2 = 0.0;

    TH1F *mcHistPlus;
    TH1F *mcHistMinus;
    
    std::stringstream ss;

    double step = 0.001;
    double maxBias = 0.5;
    double size = maxBias*step;

    std::stringstream numRebin;
    std::stringstream legendText1;
    std::stringstream legendText2;


    TH1F *chi2vCurveHist = new TH1F(TString(algo+"Chi2vCurve"), TString(algo+"#chi^{2}vs#Delta#kappa_"), numBins, -maxBias, maxBias);
    TH1F *ksvCurveHist = new TH1F(TString(algo+"Kolomogorov_Plot"), TString(algo+"Kolomogorov_Plot"), numBins, -maxBias, maxBias);
    TH1F *rchi2vCurveHist = new TH1F(TString(algo+"ReducedChi2_Plot"), TString(algo+"ReducedChi2_Plot"), 2001, 0., 2001);
    oFile->cd();
    TH1F *ndofvCurveHist = new TH1F(TString(algo+"Ndof_Plot"), TString(algo+"Ndof_Plot"), numBins, -maxBias, maxBias);


    //Get data Curvature histograms (CRAFT or Interfill).
    TH1F *refMinusHist = (TH1F*)g->Get(TString(histBase+"MinusCurve"));
    TH1F *refPlusHist = (TH1F*)g->Get(TString(histBase+"PlusCurve"));
    refMinusHist->SetName(TString(algo+"CraftMinus"));
    refPlusHist->SetName(TString(algo+"CraftPlus"));
    refMinusHist->Write();
    refPlusHist->Write();
    std::cout << "\n\nThe names of the data histograms are:\ndataMinus: " << refMinusHist->GetName() << "\ndataPlus: " << refPlusHist->GetName() << std::endl << std::endl;
    TH1F *dataHist = (TH1F*)refMinusHist->Clone("dataHist");


    


    //Add the positive and negative data histograms together then rebin them.
    refMinusHist->Sumw2();
    refPlusHist->Sumw2();
    dataHist->Sumw2();
    dataHist->Add(refPlusHist);
    dataHist->SetName(TString(algo+"CraftBoth"));
    dataInt = dataHist->Integral();
    dataHist->Write();

    std::cout << "\n\n\nThe number of Data Entries are: " << numDataEntries << "\n\n\n";

    Int_t dataBins = 0;
    dataBins = dataHist->GetNbinsX();
    //    int cutEnd = dataBins/15 +1; // where cut -7.5 to end.
    //int cutStart = dataBins - (dataBins/15) + 1; //Where cut starts until +7.5
    std::cout << "\n\nNumber of bins in dataHist are: " << dataBins << std::endl;
    for(int j = 1; j <= dataBins; j++){
      if((j < 251) || (j > 1250)){
	dataHist->SetBinContent(j, 0 );
	dataHist->SetBinError(j, 0);
      }
    }  
    dataHist->SetName(TString(algo+"dataHist_AfterCut"));
    dataHist->Write();

    numDataEntries = dataHist->GetEntries();    

    dataHist->Rebin(rebins);
    binWidth = dataHist->GetBinWidth(2);
    dataHist->SetName(TString(algo+"dataHist_Rebinned"));
    dataHist->Write();

    //Initialize variables for chi2 loops

    std::cout << "\n\nDone creating variables. Getting ready to loop over biases!\n";
    //Loop over negative injected bias.
    for(int i = iNegStart; i <= iPosEnd; i++){


      //      Double_t *res[numBins + 1]; //pass by pointer for chi2testx
      //for(int x = 0; x < numBins; x++){
      //*res[x] = -1000;
      //}
      bias = abs(i);
      if(bias == 1000){
	std::cout << "\nTook abs of negative bias\n";
	ss.str( std::string() );
	ss.clear();
	ss << std::setw(4) << bias;
	std::cout << "\ncreated new stringstream object: " << ss.str() << std::endl;
      }
      else{
	ss.str( std::string() );
	ss.clear();
	ss << std::setw(3) << std::setfill('0') <<  bias;
	std::cout << "\ncreated new stringstream object: " << ss.str() << std::endl;	
      }

      if( i < 0){
	mcHistPlus = (TH1F*)f->Get(TString(histBase+"PlusCurveMinusBias"+ss.str()));

      
	//	if(debugLevel > 0) std::cout << "\nmcHistPlus" << std::hex << mcHistPlus << std::dec << std::endl;
	
	mcHistMinus = (TH1F*)f->Get(TString(histBase+"MinusCurveMinusBias"+ss.str()));
	
	//	if(debugLevel > 0) std::cout << "\nmcHistMinus" << std::hex << mcHistMinus << std::dec << std::endl;
	std::cout << "\n\nThe histgram names are:\nmcPlus: " << mcHistPlus->GetName() << "\nmcMinus: " << mcHistMinus->GetName() << std::endl << std::endl;
	std::cout << "\ndataHist: " << dataHist->GetName() << std::endl << std::endl;
	
	//handle Minus bias cases here 
	
      }
      else if(i == 0){
    //Handle zero bias case here

    //Implement scaleChi2 for zero bias case before proceeding!

	mcHistPlus = (TH1F*)f->Get(TString(histBase+"PlusCurve"));

	//	if(debugLevel > 0) std::cout << "\nmcHistPlus" << std::hex << mcHistPlus << std::dec << std::endl;
	
	mcHistMinus = (TH1F*)f->Get(TString(histBase+"MinusCurve"));
	
	
	//	if(debugLevel > 0) std::cout << "\nmcHistMinus" << std::hex << mcHistMinus << std::dec << std::endl;
	std::cout << "\n\nThe histgram names are:\nmcPlus: " << mcHistPlus->GetName() << "\nmcMinus: " << mcHistMinus->GetName() << std::endl << std::endl;
	std::cout << "\ndataHist: " << dataHist->GetName() << std::endl << std::endl;

    /*    chi2[1000] = getChi2(f, oFile, dataHist, 0, rebins, 0, histBase, numDataEntries);
    chi2Hist->Fill(chi2[1000]);
    */
    //Loop over positive injected bias.
      }

      else if(i > 0){
	mcHistPlus = (TH1F*)f->Get(TString(histBase+"PlusCurvePlusBias"+ss.str()));
	


	//	if(debugLevel > 0) std::cout << "\nmcHistPlus" << std::hex << mcHistPlus << std::dec << std::endl;
	
	mcHistMinus = (TH1F*)f->Get(TString(histBase+"MinusCurvePlusBias"+ss.str()));
	
	
	//	if(debugLevel > 0) std::cout << "\nmcHistMinus" << std::hex << mcHistMinus << std::dec << std::endl;
	std::cout << "\n\nThe histgram names are:\nmcPlus: " << mcHistPlus->GetName() << "\nmcMinus: " << mcHistMinus->GetName() << std::endl << std::endl;
	std::cout << "\ndataHist: " << dataHist->GetName() << std::endl << std::endl;
	
      }
      
      //handle Plus bias cases here 

      //Get number of bins in each MC histogram.
      numPlus = mcHistPlus->GetNbinsX();
      numMinus = mcHistMinus->GetNbinsX();
      
      
      //Check to see if the number of bins in the histograms are the same.
      if(numPlus == numMinus){
	std::cout << "\nCombined histograms have the same size! Continuing" << std::endl;
	
	//If yes, combine them and rebin.
	oFile->cd();
	mcHistMinus->Sumw2();
	mcHistPlus->Sumw2();
	if(i < 0){
	  mcHistMinus->SetName(TString(algo+"MC_Curve_Minus_MinusBias"+ss.str()));
	  mcHistPlus->SetName(TString(algo+"MC_Curve_Plus_MinusBias"+ss.str()));
	    }
	else if(i == 0){
	  mcHistMinus->SetName(TString(algo+"MC_Curve_Minus_NoBias"));
	  mcHistPlus->SetName(TString(algo+"MC_Curve_Plus_NoBias"));
	}
	else if(i > 0){
	  mcHistMinus->SetName(TString(algo+"MC_Curve_Minus_PlusBias"+ss.str()));
	  mcHistPlus->SetName(TString(algo+"MC_Curve_Plus_PlusBias"+ss.str()));
	}
	mcHistMinus->Write();
	mcHistPlus->Write();
	//    mcHistMinus->Write();
	//mcHistPlus->Write();
	mcHistPlus->Add(mcHistMinus);
	if(i < 0) mcHistPlus->SetName(TString(algo+"MC_Curve_Added_MinusBias"+ss.str()));
	else if(i == 0) mcHistPlus->SetName(TString(algo+"MC_Curve_Added_NoBias"));
	else if(i > 0) mcHistPlus->SetName(TString(algo+"MC_Curve_Added_PlusBias"+ss.str()));
	mcHistPlus->Write();
	Double_t numMCEntries = 0.0;
	numMCEntries = mcHistPlus->GetEntries();
	std::cout << "\n\n\nThe number of MC entries are: " << numMCEntries << "\n\n\n";
	mcHistPlus->Sumw2();
	//    numData = dataHist->GetEntries();
	numMC = mcHistPlus->GetEntries();
	mcInt = mcHistPlus->Integral();

	Double_t numData = numDataEntries;	
	scale = dataInt/mcInt;
	std::cout << "\n\n\nThe dataEntries-mcEntries-scale is: " << numData << "-" << numMC << "-" << scale << std::endl;
	oldMC = mcHistPlus->Integral();
	

	std::cout << "\n\nThe integral data-mc-scale is:" << dataInt << "-" << mcInt << "-" << scale;
	mcHistPlus->Scale(scale);
	newMC = mcHistPlus->Integral();
	
	scaledEntries = mcHistPlus->GetEntries();
	std::cout << "\n\nThe number of MC entries after scaling is: " << scaledEntries << std::endl << std::endl;
	std::cout << "\n\n\nThe integral(mcHistPlus) before-after scaling returns value: " << oldMC << "-" << newMC << std::endl << std::endl;
	if(i < 0) mcHistPlus->SetName(TString(algo+"MC_Curvature_Scaled_MinusBias"+ss.str()));
	else if (i == 0) mcHistPlus->SetName(TString(algo+"MCCurvature_Scaled_NoBias"));
	else if (i > 0) mcHistPlus->SetName(TString(algo+"MCCurvature_Scaled_PlusBias"+ss.str()));
	mcHistPlus->Write();

	Int_t mcBins = 0;
	
	mcBins = mcHistPlus->GetNbinsX();
	std::cout << "\n\nNumber of bins in dataHist are: " << mcBins << std::endl;
	for(int j = 1; j <= mcBins; j++){
	  if((j < 251) || (j > 1250)){
	    mcHistPlus->SetBinContent(j, 0);
	    mcHistPlus->SetBinError(j, 0);
	  }
	}  
	if(i < 0) mcHistPlus->SetName(TString(algo+"MC_Curve_AfterCut_MinusBias"+ss.str()));
	else if(i == 0) mcHistPlus->SetName(TString(algo+"MC_Curve_AfterCut_NoBias"));
	else if(i > 0) mcHistPlus->SetName(TString(algo+"MC_Curve_AfterCut_PlusBias"+ss.str()));
	mcHistPlus->Write();
	

	mcHistPlus->Sumw2();
	mcHistPlus->Rebin(rebins);

	if(i < 0) mcHistPlus->SetName(TString(algo+"MC_Curve_Rebinned_MinusBias"+ss.str()));
	else if(i == 0) mcHistPlus->SetName(TString(algo+"MC_Curve_Rebinned_NoBias"));
	else if(i > 0) mcHistPlus->SetName(TString(algo+"MC_Curve_Rebinned_PlusBias"+ss.str()));

	mcHistPlus->Write();

	Int_t numRebinned = dataHist->GetNbinsX();
	Double_t *res = new Double_t[numRebinned];

	pVal = dataHist->Chi2TestX(mcHistPlus, chi2[i + 1000], ndof[i + 1000], igood, "UWNORM", res);
	ks[i + 1000] = dataHist->KolmogorovTest(mcHistPlus, "D");

	oFile->cd();
	std::cout << "\n\n";


	if(i == 999){
	  TH1F *resHist = new TH1F(TString(algo+"Residual_PlotMinusBias"+ss.str()),TString(algo+"Residual_PlotMinusBias"+ss.str()) , numRebinned, -7.5, 7.5);

	for(int y = 0; y < numRebinned; y++){
	  
	  resHist->SetBinContent(y+1, *(res+y));
	  resHist->SetBinError(y+1, 0);
	  std::cout << *(res + y) << "\t";
	  if(y % 10 == 0) std::cout << std::endl << std::endl;

	}
	
	resHist->Write();
	res = 0;
	delete res;
	std::cout << "\nresHist = " << std::hex << resHist << std::dec << std::endl;
	}
      }
      
      //If not return nonsense value.
      else{ std::cout << "\nCombined histograms do NOT have the same size! WHY NOT?!" << std::endl;

	return;
      }

      

      
    }

    //    pVal = dataHist->Chi2TestX(mcHistPlus, chi2 [2000], ndof[2000], igood, "UWNORM", &res[2000]);
    for(int h = 0; h < numBins; h++) std::cout << chi2[h] << "\t";
    for(int l = 0; l < numBins; l++){
      chi2vCurveHist->SetBinContent(l+1, chi2[l] );
      minChi2 = chi2vCurveHist->GetBinContent(chi2vCurveHist->GetMinimumBin());
      ksvCurveHist->SetBinContent(l+1, ks[l]);
      ndofvCurveHist->SetBinContent(l+1, ndof[l]);
      //resHist->SetBinContent(l+1, res[l]);
    }
    //Write chi2 histogram for this track algorithm to output file.
    oFile->cd();
    chi2Hist->Write();
    chi2vCurveHist->Write();
    if(algo == "TuneP"){
      double width = binWidth;
      numRebin << rebins;
      
      TCanvas *can = new TCanvas("can","can", 1000, 1000);
      can->cd();
      chi2vCurveHist->Draw("p0");
      chi2vCurveHist->SetLineColor(2);
      chi2vCurveHist->SetMarkerColor(2);
      chi2vCurveHist->SetMarkerSize(8);
      chi2vCurveHist->SetMinimum(minChi2 - 5);
      chi2vCurveHist->SetMaximum(minChi2 + 25);
      chi2vCurveHist->SetStats(0);
      chi2vCurveHist->GetXaxis()->SetTitle("#Delta#kappa[c/TeV]");
      chi2vCurveHist->GetYaxis()->SetTitle("#Chi^{2}");
      chi2vCurveHist->SetTitle("#chi^{2}vs#Delta#kappa");
      can->SetGridx(1);
      can->SetGridy(1);
      can->Update();
      TLegend* leg = new TLegend(0.3, 0.8, 0.7, 0.9);
      legendText1.str( std::string() );
      legendText1.clear();
      legendText1 << "Bias step: " << size << " [c/TeV] ";
      legendText2.str( std::string() );
      legendText2.clear();
      legendText2 << "Curvature Hist bin width:" << width << " [c/TeV] ";
      leg->AddEntry((TObject*) 0, TString(legendText1.str()), "");
      leg->AddEntry((TObject*) 0, TString(legendText2.str()), "");
      leg->SetTextSize(0.020);
      leg->Draw();
      can->Update();
      can->SaveAs(TString("~/public/html/cosmics/"+comp+"AddedChi2rebin"+numRebin.str()+".png"));
    }

    ksvCurveHist->Write();
    ndofvCurveHist->Write();
    return;
}

void chargeLoopBias(std::string const& inDir1, std::string const& inDir2, TFile *oFile, int nBiasBins, int rebins, std::string const& histBase, TH1F *chi2Hist, std::string const& algo, std::string const& comp){//, TDirectory dir){


  //FIX FILE OPEN NAME
    //Open the files. If errors return.
  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inDir1+"/CosmicHistOut_"+algo+".root"));
  if(f == 0){
    std::cout << "Error: cannot open file1! \nMake sure that you specified directory without '/' !\n";
    return;
  }
  
  std::cout << "Successfully opened file 1!\n";
  //FIX FILE OPEN NAME
  TFile *g = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inDir2+"/CosmicHistOut_"+algo+".root"));
  
  if(g == 0){
      std::cout << "Error: cannot open file1!\nMake sure that you specified directory without '/'!\n";
      return;
    }
  
    std::cout << "\nSuccessfully opened file 2!\n";

    
    Double_t binWidth = 0.0;
    int iNegStart = (-1) * nBiasBins;
    int iPosEnd = nBiasBins;
    int isPos = 0;
    int bias = 0;
    int numBins = ((2*nBiasBins) + 1);
    int bin = 0;
    Double_t chi2Plus[numBins+1]; //passed by reference for chi2testx
    Double_t chi2Minus[numBins+1]; //passed by reference for chi2testx
    for(int k = 0; k < numBins; k++){
      chi2Plus[k] = -1000;
      chi2Minus[k] = -1000;
    }
    Int_t ndofPlus[numBins+1]; //passes by reference for chi2testx
    Int_t ndofMinus[numBins+1]; //passes by reference for chi2testx
    for(int k = 0; k < numBins; k++){
      ndofPlus[k] = -1000;
      ndofMinus[k] = -1000;
    }
    Double_t ksPlus[numBins+1];
    Double_t ksMinus[numBins+1];
    for(int k = 0; k < numBins; k++){
      ksPlus[k] = -1000;
      ksMinus[k] = -1000;
    }


    Double_t pValPlus = 0.0;
    Double_t pValMinus = 0.0;
    


    Int_t numPlus = 0;
    Int_t numMinus = 0;
    Double_t numDataEntries = 0.0;

    Double_t numMC = 0.0;
    Double_t scalePlus = 0.0;
    Double_t scaleMinus = 0.0;
    Double_t scaledEntries = 0.0;
    Double_t newMC = 0.0;
    Double_t oldMC = 0.0;
    Double_t refPlusInt = 0.0;
    Double_t refMinusInt = 0.0;
    Double_t mcPlusInt = 0.0;
    Double_t mcMinusInt = 0.0;
    Int_t igood;
    Double_t minChi2Plus = 0.0;
    Double_t minChi2Minus = 0.0;

    TH1F *mcHistPlus;
    TH1F *mcHistMinus;
    
    std::stringstream ss;

    double step = 0.001;
    double maxBias = 0.5;
    double size = maxBias*step;

    std::stringstream numRebin;
    std::stringstream legendText1;
    std::stringstream legendText2;


    TH1F *chi2vCurveMinusHist = new TH1F(TString(algo+"Chi2vCurveMinus"), TString(algo+"#chi^{2}vs#Delta#kappa_"), numBins, -maxBias, maxBias);
    TH1F *chi2vCurvePlusHist = new TH1F(TString(algo+"Chi2vCurvePlus"), TString(algo+"#chi^{2}vs#Delta#kappa_"), numBins, -maxBias, maxBias);
    TH1F *ksvCurveMinusHist = new TH1F(TString(algo+"KolomogorovMinus_Plot"), TString(algo+"KolomogorovMinus_Plot"), numBins, -maxBias, maxBias);
    TH1F *ksvCurvePlusHist = new TH1F(TString(algo+"KolomogorovPlus_Plot"), TString(algo+"KolomogorovPlus_Plot"), numBins, -maxBias, maxBias);
    TH1F *rchi2vCurveHist = new TH1F(TString(algo+"ReducedChi2_Plot"), TString(algo+"ReducedChi2_Plot"), 2001, 0., 2001);
    oFile->cd();
    TH1F *ndofvCurveMinusHist = new TH1F(TString(algo+"NdofMinus_Plot"), TString(algo+"NdofMinus_Plot"), numBins, -maxBias, maxBias);
    TH1F *ndofvCurvePlusHist = new TH1F(TString(algo+"NdofPlus_Plot"), TString(algo+"NdofPlus_Plot"), numBins, -maxBias, maxBias);


    //Get data Curvature histograms (CRAFT or Interfill).
    TH1F *refMinusHist = (TH1F*)g->Get(TString(histBase+"MinusCurve"));
    TH1F *refPlusHist = (TH1F*)g->Get(TString(histBase+"PlusCurve"));
    refMinusHist->SetName(TString(algo+"CraftMinus"));
    refPlusHist->SetName(TString(algo+"CraftPlus"));
    refMinusHist->Write();
    refPlusHist->Write();
    std::cout << "\n\nThe names of the data histograms are:\ndataMinus: " << refMinusHist->GetName() << "\ndataPlus: " << refPlusHist->GetName() << std::endl << std::endl;

    //Add the positive and negative data histograms together then rebin them.
    refMinusHist->Sumw2();
    refPlusHist->Sumw2();
    refMinusHist->SetName(TString(algo+"CraftMinus"));
    refPlusHist->SetName(TString(algo+"CraftPlus"));
    refMinusInt = refMinusHist->Integral();
    refPlusInt = refPlusHist->Integral();
    refMinusHist->Write();
    refPlusHist->Write();

    std::cout << "\n\n\nThe number of Data Entries are: " << numDataEntries << "\n\n\n";

    Int_t dataBins = 0;
    dataBins = refMinusHist->GetNbinsX();
    //    int cutEnd = dataBins/15 +1; // where cut -7.5 to end.
    //int cutStart = dataBins - (dataBins/15) + 1; //Where cut starts until +7.5
    std::cout << "\n\nNumber of bins in dataHist are: " << dataBins << std::endl;
    for(int j = 1; j <= dataBins; j++){
      if(j < 251){
	refMinusHist->SetBinContent(j, 0 );
	refMinusHist->SetBinError(j, 0);
      }

      else if(j > 1250){

	refPlusHist->SetBinContent(j, 0);
	refPlusHist->SetBinError(j, 0);
      }
    }  
    refMinusHist->SetName(TString(algo+"CraftMinus_AfterCut"));
    refPlusHist->SetName(TString(algo+"CraftPlus_AfterCut"));
    refMinusHist->Write();
    refPlusHist->Write();

    refMinusHist->Rebin(rebins);
    refPlusHist->Rebin(rebins);
    binWidth = refMinusHist->GetBinWidth(2);
    refMinusHist->SetName(TString(algo+"CraftMinus_Rebinned"));
    refPlusHist->SetName(TString(algo+"CraftPlus_Rebinned"));
    refMinusHist->Write();
    refPlusHist->Write();

    //Initialize variables for chi2 loops

    std::cout << "\n\nDone creating variables. Getting ready to loop over biases!\n";
    //Loop over negative injected bias.
    for(int i = iNegStart; i <= iPosEnd; i++){


      //      Double_t *res[numBins + 1]; //pass by pointer for chi2testx
      //for(int x = 0; x < numBins; x++){
      //*res[x] = -1000;
      //}
      bias = abs(i);
      if(bias == 1000){
	std::cout << "\nTook abs of negative bias\n";
	ss.str( std::string() );
	ss.clear();
	ss << std::setw(4) << bias;
	std::cout << "\ncreated new stringstream object: " << ss.str() << std::endl;
      }
      else{
	ss.str( std::string() );
	ss.clear();
	ss << std::setw(3) << std::setfill('0') <<  bias;
	std::cout << "\ncreated new stringstream object: " << ss.str() << std::endl;	
      }

      if( i < 0){
	mcHistPlus = (TH1F*)f->Get(TString(histBase+"PlusCurveMinusBias"+ss.str()));

      
	//	if(debugLevel > 0) std::cout << "\nmcHistPlus" << std::hex << mcHistPlus << std::dec << std::endl;
	
	mcHistMinus = (TH1F*)f->Get(TString(histBase+"MinusCurveMinusBias"+ss.str()));
	
	//	if(debugLevel > 0) std::cout << "\nmcHistMinus" << std::hex << mcHistMinus << std::dec << std::endl;
	std::cout << "\n\nThe histgram names are:\nmcPlus: " << mcHistPlus->GetName() << "\nmcMinus: " << mcHistMinus->GetName() << std::endl << std::endl;
	
	//handle Minus bias cases here 
	
      }
      else if(i == 0){
    //Handle zero bias case here

    //Implement scaleChi2 for zero bias case before proceeding!

	mcHistPlus = (TH1F*)f->Get(TString(histBase+"PlusCurve"));

	//	if(debugLevel > 0) std::cout << "\nmcHistPlus" << std::hex << mcHistPlus << std::dec << std::endl;
	
	mcHistMinus = (TH1F*)f->Get(TString(histBase+"MinusCurve"));
	
	
	//	if(debugLevel > 0) std::cout << "\nmcHistMinus" << std::hex << mcHistMinus << std::dec << std::endl;
	std::cout << "\n\nThe histgram names are:\nmcPlus: " << mcHistPlus->GetName() << "\nmcMinus: " << mcHistMinus->GetName() << std::endl << std::endl;

    //Loop over positive injected bias.
      }

      else if(i > 0){
	mcHistPlus = (TH1F*)f->Get(TString(histBase+"PlusCurvePlusBias"+ss.str()));
	


	//	if(debugLevel > 0) std::cout << "\nmcHistPlus" << std::hex << mcHistPlus << std::dec << std::endl;
	
	mcHistMinus = (TH1F*)f->Get(TString(histBase+"MinusCurvePlusBias"+ss.str()));
	
	
	//	if(debugLevel > 0) std::cout << "\nmcHistMinus" << std::hex << mcHistMinus << std::dec << std::endl;
	std::cout << "\n\nThe histgram names are:\nmcPlus: " << mcHistPlus->GetName() << "\nmcMinus: " << mcHistMinus->GetName() << std::endl << std::endl;
	
      }
      
      //handle Plus bias cases here 

      //Get number of bins in each MC histogram.
      numPlus = mcHistPlus->GetNbinsX();
      numMinus = mcHistMinus->GetNbinsX();
      
      
      //Check to see if the number of bins in the histograms are the same.
      if(numPlus == numMinus){
	std::cout << "\nCombined histograms have the same size! Continuing" << std::endl;
	
	//If yes, combine them and rebin.
	oFile->cd();
	mcHistMinus->Sumw2();
	mcHistPlus->Sumw2();
	if(i < 0){
	  mcHistMinus->SetName(TString(algo+"MC_Curve_Minus_MinusBias"+ss.str()));
	  mcHistPlus->SetName(TString(algo+"MC_Curve_Plus_MinusBias"+ss.str()));
	    }
	else if(i == 0){
	  mcHistMinus->SetName(TString(algo+"MC_Curve_Minus_NoBias"));
	  mcHistPlus->SetName(TString(algo+"MC_Curve_Plus_NoBias"));
	}
	else if(i > 0){
	  mcHistMinus->SetName(TString(algo+"MC_Curve_Minus_PlusBias"+ss.str()));
	  mcHistPlus->SetName(TString(algo+"MC_Curve_Plus_PlusBias"+ss.str()));
	}
	mcHistMinus->Write();
	mcHistPlus->Write();

	mcHistPlus->Sumw2();

	mcMinusInt = mcHistMinus->Integral();
	mcPlusInt = mcHistPlus->Integral();





	scalePlus = refPlusInt/mcPlusInt;
	scaleMinus = refMinusInt/mcMinusInt;

	std::cout << "\n\n\nThe MINUS data-mc-scale is: " << refMinusInt << "-" << mcMinusInt << "-" << scaleMinus << std::endl;

	std::cout << "\n\n\nThe PLUS data-mc-scale is: " << refPlusInt << "-" << mcPlusInt << "-" << scalePlus << std::endl;	
	mcHistPlus->Scale(scalePlus);
	mcHistMinus->Scale(scaleMinus);
	
	if(i < 0){
	  mcHistPlus->SetName(TString(algo+"MCPlus_Curvature_Scaled_MinusBias"+ss.str()));
	  mcHistMinus->SetName(TString(algo+"MCMinus_Curvature_Scaled_MinusBias"+ss.str()));
	}
	else if (i == 0){
	  mcHistPlus->SetName(TString(algo+"MCPlusCurvature_Scaled_NoBias"));
	  mcHistMinus->SetName(TString(algo+"MCMinusCurvature_Scaled_NoBias"));
	}	
	else if (i > 0){
	  mcHistPlus->SetName(TString(algo+"MCPlusCurvature_Scaled_PlusBias"+ss.str()));
	  mcHistMinus->SetName(TString(algo+"MCMinusCurvature_Scaled_PlusBias"+ss.str()));
	}
	  mcHistPlus->Write();
	  mcHistMinus->Write();

	Int_t mcBins = 0;
	
	mcBins = mcHistPlus->GetNbinsX();
	std::cout << "\n\nNumber of bins in dataHist are: " << mcBins << std::endl;
	for(int j = 1; j <= mcBins; j++){
	  if(j < 251){
	    mcHistMinus->SetBinContent(j, 0);
	    mcHistMinus->SetBinError(j, 0);
	  }

	  else if(j > 1250){
	  mcHistPlus->SetBinContent(j, 0);
	  mcHistPlus->SetBinError(j, 0);
	  }
	}  
      if(i < 0){
	mcHistPlus->SetName(TString(algo+"MCplus_Curve_AfterCut_MinusBias"+ss.str()));
	mcHistMinus->SetName(TString(algo+"MCMinus_Curve_AfterCut_MinusBias"+ss.str()));
      }	
      else if(i == 0){
	mcHistPlus->SetName(TString(algo+"MCPlus_Curve_AfterCut_NoBias"));
	mcHistMinus->SetName(TString(algo+"MCMinus_Curve_AfterCut_NoBias"));
      }
      else if(i > 0){
	mcHistPlus->SetName(TString(algo+"MCPlus_Curve_AfterCut_PlusBias"+ss.str()));
	mcHistMinus->SetName(TString(algo+"MCMinus_Curve_AfterCut_PlusBias"+ss.str()));
      }
	mcHistPlus->Write();
	mcHistMinus->Write();	

	mcHistPlus->Sumw2();
	mcHistMinus->Sumw2();
	mcHistPlus->Rebin(rebins);
	mcHistMinus->Rebin(rebins);

	if(i < 0){
	  mcHistPlus->SetName(TString(algo+"MCPlus_Curve_Rebinned_MinusBias"+ss.str()));
	  mcHistMinus->SetName(TString(algo+"MCMinus_Curve_Rebinned_MinusBias"+ss.str()));
	}
	else if(i == 0){
	  mcHistPlus->SetName(TString(algo+"MCPlus_Curve_Rebinned_NoBias"));
	  mcHistMinus->SetName(TString(algo+"MCMinus_Curve_Rebinned_NoBias"));
	}
	else if(i > 0){
	  mcHistPlus->SetName(TString(algo+"MCPlus_Curve_Rebinned_PlusBias"+ss.str()));
	  mcHistMinus->SetName(TString(algo+"MCMinus_Curve_Rebinned_PlusBias"+ss.str()));
	}

	mcHistPlus->Write();
	mcHistMinus->Write();

	Int_t numRebinned = refMinusHist->GetNbinsX();
	Double_t *res = new Double_t[numRebinned];

	pValPlus = refPlusHist->Chi2TestX(mcHistPlus, chi2Plus[i + 1000], ndofPlus[i + 1000], igood, "UWNORM", res);
	pValMinus = refMinusHist->Chi2TestX(mcHistMinus, chi2Minus[i + 1000], ndofMinus[i + 1000], igood, "UWNORM", res);
	ksPlus[i + 1000] = refPlusHist->KolmogorovTest(mcHistPlus, "D");
	ksMinus[i + 1000] = refMinusHist->KolmogorovTest(mcHistMinus, "D");

	oFile->cd();
	std::cout << "\n\n";


	if(i == 999){
	  TH1F *resHist = new TH1F(TString(algo+"Residual_PlotMinusBias"+ss.str()),TString(algo+"Residual_PlotMinusBias"+ss.str()) , numRebinned, -7.5, 7.5);

	for(int y = 0; y < numRebinned; y++){
	  
	  resHist->SetBinContent(y+1, *(res+y));
	  resHist->SetBinError(y+1, 0);
	  std::cout << *(res + y) << "\t";
	  if(y % 10 == 0) std::cout << std::endl << std::endl;

	}
	
	resHist->Write();
	res = 0;
	delete res;
	std::cout << "\nresHist = " << std::hex << resHist << std::dec << std::endl;
	}
      }
      
      //If not return nonsense value.
      else{ std::cout << "\nCombined histograms do NOT have the same size! WHY NOT?!" << std::endl;

	return;
      }

      

      
    }

    //    pVal = dataHist->Chi2TestX(mcHistPlus, chi2 [2000], ndof[2000], igood, "UWNORM", &res[2000]);
    for(int h = 0; h < numBins; h++) std::cout << chi2Plus[h] << "\t";
    for(int h = 0; h < numBins; h++) std::cout << chi2Minus[h] << "\t";
    for(int l = 0; l < numBins; l++){
      chi2vCurvePlusHist->SetBinContent(l+1, chi2Plus[l] );
      chi2vCurveMinusHist->SetBinContent(l+1, chi2Minus[l] );
      ksvCurvePlusHist->SetBinContent(l+1, ksPlus[l]);
      ksvCurveMinusHist->SetBinContent(l+1, ksMinus[l]);
      ndofvCurvePlusHist->SetBinContent(l+1, ndofPlus[l]);
      ndofvCurveMinusHist->SetBinContent(l+1, ndofMinus[l]);
      //resHist->SetBinContent(l+1, res[l]);
    }

      minChi2Plus = chi2vCurvePlusHist->GetBinContent(chi2vCurvePlusHist->GetMinimumBin());
      minChi2Minus = chi2vCurveMinusHist->GetBinContent(chi2vCurveMinusHist->GetMinimumBin());

      double minChi2 =0.0;
      if(minChi2Plus < minChi2Minus) minChi2 = minChi2Plus;
      else minChi2 = minChi2Minus;
    //Write chi2 histogram for this track algorithm to output file.
    oFile->cd();
    chi2Hist->Write();
    chi2vCurvePlusHist->Write();
    chi2vCurveMinusHist->Write();
    if(algo == "TuneP"){
      double width = binWidth;
      numRebin << rebins;
      
      TCanvas *can = new TCanvas("can","can", 1000, 1000);
      can->cd();
      chi2vCurvePlusHist->Draw("p0");
      chi2vCurvePlusHist->SetLineColor(2);
      chi2vCurvePlusHist->SetMarkerColor(2);
      chi2vCurvePlusHist->SetMarkerSize(8);
      chi2vCurvePlusHist->SetMinimum(minChi2 - 5);
      chi2vCurvePlusHist->SetMaximum(minChi2 + 25);
      chi2vCurvePlusHist->SetStats(0);
      chi2vCurvePlusHist->GetXaxis()->SetTitle("#Delta#kappa[c/TeV]");
      chi2vCurvePlusHist->GetYaxis()->SetTitle("#Chi^{2}");
      chi2vCurvePlusHist->SetTitle("#chi^{2}vs#Delta#kappa");
      chi2vCurveMinusHist->Draw("p0smames");
      chi2vCurveMinusHist->SetLineColor(4);
      chi2vCurveMinusHist->SetMarkerColor(4);
      chi2vCurveMinusHist->SetMarkerSize(8);
      chi2vCurveMinusHist->SetMinimum(minChi2Plus - 5);
      chi2vCurveMinusHist->SetMaximum(minChi2Plus + 25);
      chi2vCurveMinusHist->SetStats(0);
      can->SetGridx(1);
      can->SetGridy(1);
      can->Update();
      /*  TLegend* leg = new TLegend(0.3, 0.8, 0.7, 0.9);
      legendText1.str( std::string() );
      legendText1.clear();
      legendText1 << "Bias step: " << size << " [c/TeV] ";
      legendText2.str( std::string() );
      legendText2.clear();
      legendText2 << "Curvature Hist bin width:" << width << " [c/TeV] ";
      leg->AddEntry((TObject*) 0, TString(legendText1.str()), "");
      leg->AddEntry((TObject*) 0, TString(legendText2.str()), "");
      leg->SetTextSize(0.020);
      leg->Draw();
      */
      can->Update();
      can->SaveAs(TString("~/public/html/cosmics/"+comp+"QsepChi2rebin"+numRebin.str()+".png"));
    }
     
    ksvCurvePlusHist->Write();
    ndofvCurvePlusHist->Write();
    ksvCurveMinusHist->Write();
    ndofvCurveMinusHist->Write();
    return;
}
