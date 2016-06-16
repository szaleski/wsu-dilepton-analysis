/*#include "DataFormats/Math/interface/Vector.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
*/
//#include "TLorentzVector.h"
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
#include "TArray.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>

double calcChi2(TFile *file, int bias, int bins, double muMinusTot, double muPlusTot, bool isPos, int debugLevel);
double getChi2(TH1F *hist1, TH1F *hist2, int bins, double muMinusTot, double muPlusTot, int debugLevel);
double getNewChi2(TH1F *hist1, TH1F *hist2, int bins, double muMinusTot, double muPlusTot, int debugLevel);
//double calcError(TFile *file, int bias, int bins, double muMinusTot, double muPlusTot, bool isPos, int debugLevel);
double calcNewChi2(TFile* file, int bias, int bins, double muMinusTot, double muPlusTot, bool isPos, int debugLevel);
//double getError(TH1F *hist1, TH1F *hist2, int bins, double muMinusTot, double muPlusTot, int debugLevel);
double calcRootChi2(TFile* file, int bias, bool isPos, int debugLevel);

void checkChi2(){
  int debug = 0;
  int inBias = 0;

  TFile *file = TFile::Open("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_4_12/src/WSUDiLeptons/CosmicEndpoint/test/maxbias0005b1000pt50f1000r500_sym.root", "READ");
  if(file == 0){
    std::cout << "\nError: cannot open file " << std::endl;
    return;
  }

  std::cout << "\nSuccessfully opened file: " << std::endl;



  //Add ROOT Chi2Test call and print it!

  TH1F *h_muMinusCurve = (TH1F*)file->Get("obs_Minus/looseMuUpperMinusCurve;4");
  if(debug > 1) std::cout << "h_muMinusCurve" << std::hex << h_muMinusCurve << std::dec << std::endl;
  TH1F *h_muPlusCurve = (TH1F*)file->Get("obs_Minus/looseMuUpperPlusCurve;4");
  if(debug > 1) std::cout << "h_muPlusCurve" << std::hex << h_muPlusCurve << std::dec << std::endl;
  //  TH1F *h_rootChi2 =(TH1F*)file->Get("picky;1-looseMuUpperPlusCurvePlusBias607;4/tunep_Chi2;1");
  TH1F *h_rootChi2 =(TH1F*)file->Get("obs_Minus/tunep_Chi2");
  if(debug > 0) std::cout << "h_rootChi2" << std::hex << h_rootChi2 << std::dec << std::endl;

    
  Int_t numMinusBins = h_muMinusCurve->GetSize();
  if(debug > 1) std::cout << "\nThe number of bins for muMinus are: " << numMinusBins << std::endl;
  Int_t numPlusBins = h_muPlusCurve->GetSize();
  if(debug > 1) std::cout << "\nThe number of bins for muPlus are: " << numPlusBins << std::endl;

  Int_t numBins = 0;
  if(numMinusBins == numPlusBins) numBins = numMinusBins;
  if(debug > 1) std::cout << "\nThe total number of bins are: " << numBins;

  for(int i = 0; i < numBins - 2; i++){
    if(debug > 1) std::cout << "\nNumber of muMinus in bin " << i + 1 << " are: "  <<  h_muMinusCurve->GetBinContent(i + 1) << "\t";
    if(debug > 1) std::cout << "Number of muPlus in bin " << i + 1 << " are: " << h_muPlusCurve->GetBinContent(i + 1) << std::endl;
       
  }

  double M = 0.0;
  double N = 0.0;
  double rootChi2 = 0.0;
  double rootCalcChi2 = 0.0;
  double_t chi2Val = 0.0;
  Int_t ndofVal = 0;
  Int_t igood = 3;

  for(int i = 0; i < numBins - 2; i++){
    
    M = M + h_muMinusCurve->GetBinContent(i + 1);
    N = N + h_muPlusCurve->GetBinContent(i + 1);
    
  }

  std::cout << "\nThe total number of muMinus entries are: " << M << "\nThe total number of muPlus entries are: " << N << std::endl;

  double chi2 = getChi2(h_muMinusCurve, h_muPlusCurve, numBins, M, N, 0);
  double newChi2 = getNewChi2(h_muMinusCurve, h_muPlusCurve, numBins, M, N, 0);
  double_t thechi2 = h_muMinusCurve->Chi2TestX(h_muPlusCurve, chi2Val, ndofVal, igood, "UU", 0);
  double_t chiNdof = chi2Val/ndofVal;
  std::cout << "\nThe zero bin for chi2/ndof is: " << chiNdof << std::endl << std::endl;
  //double error = getError(h_muMinusCurve, h_muPlusCurve, numBins, M, N, 0);

    for(int j = -1000; j < 0; j++){
    inBias = -j;
    if(debug > 0)std::cout << "\nThe injected bias for histogram is: " << inBias << std::endl;
    chi2 =  calcChi2(file, inBias, numBins, M, N, 0, 0);
    //std::cout << "\nThe value for chi2 of bias " << j << " is: " << chi2 << std::endl;
    newChi2 = calcNewChi2(file, inBias, numBins, M, N, 0, 1);
    std::cout << "\nThe value for newChi2/ndf of bias " << j <<  " is: " << newChi2 << std::endl;
    rootChi2 = h_rootChi2->GetBinContent(j + 1001);
    //std::cout << "\nThe Value for ROOTChi2 of bias " << j << " is: " << rootChi2 << std::endl;
    rootCalcChi2 = calcRootChi2(file, inBias, 0, 0);
    //std::cout << "\nThe value that ROOT CALCULATED for chi2 of bias " << j << " is: " << rootCalcChi2 << std::endl;
    
    /*error = calcError(file, inBias, numBins, M, N, 0, 0);
    std::cout << "\nThe value for chi2 Error of bias " << j << "is: " << error << std::endl;
    */ }
  
  inBias = 0;
  for(int k = 1; k < 1001; k++){

    inBias = k;
    if(debug > 0) std::cout << "\nThe injected bias for histogram is: " << inBias << std::endl;
    chi2 = calcChi2(file, inBias, numBins, M, N, 1, 0);
    //std::cout << "\nThe value for chi2 of bias " << k << " is: " << chi2 << std::endl;
    newChi2 = calcNewChi2(file, inBias, numBins, M, N, 1, 1);
    std::cout << "\nThe value for newChi2/ndf of bias " << k << " is: " << newChi2 << std::endl;
    rootChi2 = h_rootChi2->GetBinContent(k + 1001);
    //std::cout << "\nThe Value for ROOTChi2 of bias " << k << " is: " << rootChi2 << std::endl;
    rootCalcChi2 = calcRootChi2(file, inBias, 0, 0);
    //std::cout << "\nThe value that ROOT CALCULATED for chi2 of bias " << k << " is: " << rootCalcChi2 << std::endl;

    /*error = calcError(file, inBias, numBins, M, N, 0, 0);
    std::cout << "\nThe value for chi2 Error of bias " << k << "is: " << error << std::endl;
    */

  }

  // std::cout << "\n\nChi2 value returned to main function is: " << chi2 << std::endl << std::endl;
  //  std::cout << "\n\nError value returned to main function is: " << error << std::endl << std::endl;
  //double chi2 = calcChi2(file, 11, 0, 1);


  return;
}


  double calcChi2(TFile* file, int bias, int bins, double muMinusTot, double muPlusTot, bool isPos, int debugLevel){


    double chi2 = 0.0;
    
    string stringBias;

    std::ostringstream oss;
    
    
    oss << bias;
    
    newBias = oss.str();
    
    
    if((bias > 0) && (bias < 10)){
      if(isPos){
	if(debugLevel > 2) std::cout << "\n\nENTERED PLUS BIAS!" << std::endl << std::endl;
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias00"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << "are: " << numMuons << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	chi2 = getChi2(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      
      else{
	if(debugLevel > 2) std::cout << "\n\nENTERED MINUS BIAS!" << std::endl << std::endl;
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias00"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total nuber of bins in this histogram " << bias << " are: " << numMuons << std::endl; 
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	chi2 = getChi2(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
	
	
      }
    }
    	
    
    else if((bias >= 10) && (bias < 100)){
      if(debugLevel > 2)  std::cout << "\nMade it into the secondary function!" << std::endl;
      
      if(isPos){
	if(debugLevel > 2) std::cout << "\n\nENTERED PLUS BIAS!" << std::endl << std::endl;
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias0"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias0"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << endl;
	chi2 = getChi2(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      else{
	if(debugLevel > 2) std::cout << "\n\nENTERED MINUS BIAS!" << std::endl << std::endl;
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias0"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "h_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias0"+newBias+";4"));
	//Int_t numMuons = h_muPlusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "h_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	chi2 = getChi2(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
    }
    
    
    else{
      if(isPos){
	if(debugLevel > 2) std::cout << "\n\nENTERED PLUS BIAS!" << std::endl << std::endl;
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	chi2 = getChi2(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      
      else{
	if(debugLevel > 2) std::cout << "\n\nENTERED MINUS BIAS!"<< std::endl << std::endl;
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	chi2 = getChi2(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
    }
    
    
    return chi2;
    
  }

double getChi2(TH1F *hist1, TH1F *hist2, int bins, double muMinusTot, double muPlusTot, int debugLevel){
  debugLevel = 0;
  double prefactor = muMinusTot * muPlusTot;
  if(debugLevel > 0) std::cout << "\nThe product of number of muMinus and muPlus are: " << prefactor << std::endl;
  double factor = 1 / prefactor;
  if(debugLevel > 0) std::cout << "\nThe multiplicative factor for the chi2 is: " << factor << std::endl;
  double muMinus = 0.0;
  double muPlus = 0.0;
  double denominator = 0.0;
  double crossTerm1 = 0.0;
  double crossTerm2 = 0.0;
  double preNumerator = 0.0;
  double numerator = 0.0;
  double preChi2 = 0.0;
  double chi2 = 0.0;
  
  for(int i = 0; i < bins - 2; i++){
    muMinus = hist1->GetBinContent(i + 1);
    if(debugLevel > 1) std::cout << "\nThe number of muMinus in bin " << i + 1 << " are: " << muMinus << std::endl;
    muPlus = hist2->GetBinContent(i + 1);
    if(debugLevel > 1) std::cout << "\nThe number of muPlus in bin " << i + 1 << " are: " << muPlus << std::endl;
    denominator = muMinus + muPlus;
    if(debugLevel > 1) std::cout << "\nThe denominator is: " << denominator << std::endl;
    crossTerm1 = muPlusTot * muMinus;
    if(debugLevel > 1) std::cout << "\nThe first term in numerator is: " << crossTerm1 << std::endl;
    crossTerm2 = muMinusTot * muPlus;
    if(debugLevel > 1) std::cout << "\nThe second term in the numerator is: " << crossTerm2 << std::endl;
    preNumerator = crossTerm1 - crossTerm2;
    if(debugLevel > 1) std::cout << "\nThe numerator prior to suaring is: " << preNumerator << std::endl;
    numerator = preNumerator * preNumerator;
    if(debugLevel > 1) std:: cout<< "\nThe numerator is: " << numerator << std::endl;
    if(denominator == 0) preChi2 = 0;
    else preChi2  = numerator/denominator;
    if(debugLevel > 0) std::cout << "\nThe chi2 prior to factor is: " << preChi2 << std::endl;
    chi2 = chi2 + (factor * preChi2);
    if(debugLevel > 0) std::cout << "\nThe chi2 value is: " << chi2 << std::endl;

  }
  
  // std::cout << "\nThe value for chi2 for this bin is: " << chi2 << std::endl;

  return chi2;

}

  double calcNewChi2(TFile* file, int bias, int bins, double muMinusTot, double muPlusTot, bool isPos, int debugLevel){


    double chi2 = 0.0;
    
    string stringBias;

    std::ostringstream oss;
    
    
    oss << bias;
    
    newBias = oss.str();
    
    
    if((bias > 0) && (bias < 10)){
      if(isPos){
	
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias00"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << "are: " << numMuons << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	chi2 = getNewChi2(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias00"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total nuber of bins in this histogram " << bias << " are: " << numMuons << std::endl; 
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	chi2 = getNewChi2(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
	
	
      }
    }
    	
    
    else if((bias >= 10) && (bias < 100)){
      std::cout << "\nMade it into the secondary function!" << std::endl;
      if(isPos){
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias0"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias0"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << endl;
	chi2 = getNewChi2(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias0"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "h_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias0"+newBias+";4"));
	//Int_t numMuons = h_muPlusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "h_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	chi2 = getNewChi2(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
    }
    
    
    else{
      if(isPos){
	
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	chi2 = getNewChi2(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	chi2 = getNewChi2(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
    }
    
    
    return chi2;
    
  }


double getNewChi2(TH1F *hist1, TH1F *hist2, int bins, double muMinusTot, double muPlusTot, int debugLevel){
  debugLevel = 0;

  double muMinus = 0.0;
  double muPlus = 0.0;
  double denominator = 0.0;
  double difference = 0.0;
  double numerator = 0.0;
  double preChi2 = 0.0;
  double chi2 = 0.0;
  int ndof = 0;
  int muMinusNdof = 0;
  int muPlusNdof = 0;
  double chi2Ndof = 0.0;
  
  for(int i = 0; i < bins - 2; i++){
    muMinus = hist1->GetBinContent(i + 1);
    if(muMinus > 0){ 
      ndof = ndof + 1;
      muMinusNdof = muMinusNdof + 1;
    }
    if(debugLevel > 0) std::cout << "\nThe number of muMinus in bin " << i + 1 << " are: " << muMinus << std::endl;
    muPlus = hist2->GetBinContent(i + 1);
    if(muPlus > 0) muPlusNdof = muPlusNdof + 1;
    if(debugLevel > 0) std::cout << "\nThe number of muPlus in bin " << i + 1 << " are: " << muPlus << std::endl;
    denominator = muMinus + muPlus;
    if(debugLevel > 1) std::cout << "\nThe denominator is: " << denominator << std::endl;
    difference = muMinus - muPlus;
    if(debugLevel > 1) std::cout << "\nThe difference is: " << difference << std::endl;
    numerator = difference * difference;
    if(debugLevel > 1) std:: cout<< "\nThe numerator is: " << numerator << std::endl;
    if(denominator == 0) preChi2 = 0;
    else preChi2  = numerator/denominator;
    if(debugLevel > 0) std::cout << "\nThe preChi2 value is: " << preChi2 << std::endl;
    chi2 = chi2 + preChi2;
    if(debugLevel > 1) std::cout << "\nThe chi2 value is: " << chi2 << std::endl;

  }
  chi2Ndof = chi2/(ndof -1);
  if(muMinusNdof != muPlusNdof){
    std::cout << "\nThe number of degrees of freedom are not the same in this histogram!\n";
    std::cout << "\nThe number of muon and antiMuon degrees of freedom are: " << muMinusNdof << "\t" << muPlusNdof << std::endl;
  }
  else {
    std::cout << "\nThe number of degrees of freedom are the same for both histograms!" << std::endl;
    std::cout << "\nThe number of muon and antiMuon degrees of freedom are: " << muMinusNdof << "\t" << muPlusNdof << std::endl;
    
    //std::cout << "\nThe value for chi2 for this bin is: " << chi2 << std::endl;
  }
  return chi2Ndof;

}

double calcRootChi2(TFile* file, int bias, bool isPos, int debugLevel){


    double chi2 = 0.0;
    
    string stringBias;

    std::ostringstream oss;
    
    
    oss << bias;
    
    newBias = oss.str();
    
    Double_t chi2Val = 0.0;
    Int_t ndofVal = 0;
    Int_t igood = 3;
    Double_t chiNdof = 0.0;

    if((bias > 0) && (bias < 10)){
      if(isPos){
	
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	chi2 = h_muMinusPosBiasCurve->Chi2TestX(h_muPlusPosBiasCurve, chi2Val, ndofVal, igood, "UU", 0);
	//std::cout << "\n\nThe THE FUNCTION gives CHI2VAL of: " << chi2Val << std::endl << std::endl;
	//std::cout << "\nThe THE FUNCTION gives ndofVal of: " << ndofVal << std::endl;
	chiNdof = chi2Val/ndofVal;
	std::cout << chiNdof << std::endl;
	//std::cout << "\nThe THE FUNCTION gives chi2/ndof of: " << chiNdof << std::endl;
      }
      
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	chi2 = h_muMinusNegBiasCurve->Chi2TestX(h_muPlusNegBiasCurve, chi2Val, ndofVal, igood, "UU", 0);
	//std::cout << "\n\nThe THE FUNCTION gives CHI2VAL of: " << chi2Val << std::endl << std::endl;	
	//std::cout << "\nThe THE FUNCTION gives ndofVal of: " << ndofVal << std::endl;
	chiNdof = chi2Val/ndofVal;
	std::cout << chiNdof << std::endl;
	//std::cout << "\nThe THE FUNCTION gives chi2/ndof of: " << chiNdof << std::endl;	
      }
    }
    	
    
    else if((bias >= 10) && (bias < 100)){
      std::cout << "\nMade it into the secondary function!" << std::endl;
      if(isPos){
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias0"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias0"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << endl;
	chi2 = h_muMinusPosBiasCurve->Chi2TestX(h_muPlusPosBiasCurve, chi2Val, ndofVal, igood, "UU", 0);
	//std::cout << "\n\nThe THE FUNCTION gives CHI2VAL of: " << chi2Val << std::endl << std::endl;
	//std::cout << "\nThe THE FUNCTION gives ndofVal of: " << ndofVal << std::endl;
	chiNdof = chi2Val/ndofVal;
	std::cout << chiNdof << std::endl;
	//std::cout << "\nThe THE FUNCTION gives chi2/ndof of: " << chiNdof << std::endl;
      }
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias0"+newBias+";4"));
	if(debugLevel > 0) std::cout << "h_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias0"+newBias+";4"));
	if(debugLevel > 0) std::cout << "h_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	chi2 = h_muMinusNegBiasCurve->Chi2TestX(h_muPlusNegBiasCurve, chi2Val, ndofVal, igood, "UU", 0);
	//std::cout << "\n\nThe THE FUNCTION gives CHI2VAL of: " << chi2Val << std::endl << std::endl;
	//std::cout << "\nThe THE FUNCTION gives ndofVal of: " << ndofVal << std::endl;
	chiNdof = chi2Val/ndofVal;
	std::cout << chiNdof << std::endl;
	//std::cout << "\nThe THE FUNCTION gives chi2/ndof of: " << chiNdof << std::endl;
      }
    }
    
    
    else{
      if(isPos){
	
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias"+newBias+";4"));
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	chi2 = h_muMinusPosBiasCurve->Chi2TestX(h_muPlusPosBiasCurve, chi2Val, ndofVal, igood, "UU", 0);
	//std::cout << "\n\nThe THE FUNCTION gives CHI2VAL of: " << chi2Val << std::endl << std::endl;
	//std::cout << "\nThe THE FUNCTION gives ndofVal of: " << ndofVal << std::endl;
	chiNdof = chi2Val/ndofVal;
	std::cout << chiNdof << std::endl;
	//std::cout << "\nThe THE FUNCTION gives chi2/ndof of: " << chiNdof << std::endl;
      }
      
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	chi2 = h_muMinusNegBiasCurve->Chi2TestX(h_muPlusNegBiasCurve, chi2Val, ndofVal, igood, "UU", 0);
	//std::cout << "\n\nThe THE FUNCTION gives CHI2VAL of: " << chi2Val << std::endl << std::endl;
	//std::cout << "\nThe THE FUNCTION gives ndofVal of: " << ndofVal << std::endl;
	chiNdof = chi2Val/ndofVal;
	std::cout << "\n\nThe number of degrees of freedom for this histogram is: " << ndofVal << std::endl << std::endl;
	std::cout << chiNdof << std::endl;
	//std::cout << "\nThe THE FUNCTION gives chi2/ndof of: " << chiNdof << std::endl;
      }
    }
    
    
    return chi2;
    
  }

/*
  double calcError(TFile* file, int bias, int bins, double muMinusTot, double muPlusTot, bool isPos, int debugLevel){


    double error = 0.0;
    
    string stringBias;

    std::ostringstream oss;
    
    
    oss << bias;
    
    newBias = oss.str();
    
    
    if((bias > 0) && (bias < 10)){
      if(isPos){
	
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias00"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << "are: " << numMuons << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	error = getError(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias00"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total nuber of bins in this histogram " << bias << " are: " << numMuons << std::endl; 
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias00"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	error = getError(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
	
	
      }
    }
    	
    
    else if((bias >= 10) && (bias < 100)){
      std::cout << "\nMade it into the secondary function!" << std::endl;
      if(isPos){
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias0"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias0"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << endl;
	error = getError(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias0"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "h_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias0"+newBias+";4"));
	//Int_t numMuons = h_muPlusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "h_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	error = getError(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
    }
    
    
    else{
      if(isPos){
	
	TH1F *h_muMinusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurvePlusBias"+newBias+";4"));
	Int_t numMuons = h_muMinusPosBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	if(debugLevel > 0) std::cout << "\nh_muMinusPosBiasCurve" << std::hex << h_muMinusPosBiasCurve << std::dec << std::endl;
	TH1F *h_muPlusPosBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurvePlusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusPosBiasCurve" << std::hex << h_muPlusPosBiasCurve << std::dec << std::endl;
	error = getError(h_muMinusPosBiasCurve, h_muPlusPosBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
      
      else{
	TH1F *h_muMinusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperMinusCurveMinusBias"+newBias+";4"));
	Int_t numMuons = h_muMinusNegBiasCurve->GetSize();
	if(debugLevel > 0) std::cout << "\nh_muMinusNegBiasCurve" << std::hex << h_muMinusNegBiasCurve << std::dec << std::endl;
	if(debugLevel > 0) std::cout << "\nThe total number of bins in this histogram " << bias << " are: " << numMuons << std::endl;
	TH1F *h_muPlusNegBiasCurve = (TH1F*)file->Get(TString("obs_Minus/looseMuUpperPlusCurveMinusBias"+newBias+";4"));
	if(debugLevel > 0) std::cout << "\nh_muPlusNegBiasCurve" << std::hex << h_muPlusNegBiasCurve << std::dec << std::endl;
	error = getError(h_muMinusNegBiasCurve, h_muPlusNegBiasCurve, bins, muMinusTot, muPlusTot, debugLevel);
      }
    }
    
    
    return error;
    
  }

double getError(TH1F *hist1, TH1F *hist2, int bins, double muMinusTot, double muPlusTot, int debugLevel){
  debugLevel = 0;
  double prefactor = muMinusTot * muPlusTot;
  if(debugLevel > 0) std::cout << "\nThe product of number of muMinus and muPlus are: " << prefactor << std::endl;
  double factor = 1 / prefactor;
  if(debugLevel > 0) std::cout << "\nThe multiplicative factor for the chi2 is: " << factor << std::endl;
  double muMinus = 0.0;
  double muPlus = 0.0;
  double denominator = 0.0;
  double crossTerm1 = 0.0;
  double crossTerm2 = 0.0;
  double preNumerator = 0.0;
  double numerator = 0.0;
  double preMinusErrorTerm1 = 0.0;
  double prePlusErrorTerm1 = 0.0;
  double preMinusErrorTerm2 = 0.0;
  double prePlusErrorTerm2 = 0.0;
  double preMinusError = 0.0;
  double prePlusError = 0.0;
  double minusError = 0.0;
  double plusError = 0.0;
  double preError = 0.0;
  double error = 0.0;

  for(int i = 0; i < bins - 2; i++){
    muMinus = hist1->GetBinContent(i + 1);
    if(debugLevel > 1) std::cout << "\nThe number of muMinus in bin " << i + 1 << " are: " << muMinus << std::endl;
    muPlus = hist2->GetBinContent(i + 1);
    if(debugLevel > 1) std::cout << "\nThe number of muPlus in bin " << i + 1 << " are: " << muPlus << std::endl;
    denominator = muMinus + muPlus;
    if(debugLevel > 1) std::cout << "\nThe denominator is: " << denominator << std::endl;
    crossTerm1 = muPlusTot * muMinus;
    if(debugLevel > 1) std::cout << "\nThe first term in numerator is: " << crossTerm1 << std::endl;
    crossTerm2 = muMinusTot * muPlus;
    if(debugLevel > 1) std::cout << "\nThe second term in the numerator is: " << crossTerm2 << std::endl;
    preNumerator = crossTerm1 - crossTerm2;
    if(debugLevel > 1) std::cout << "\nThe numerator prior to suaring is: " << preNumerator << std::endl;
    numerator = preNumerator * preNumerator;
    if(debugLevel > 1) std:: cout<< "\nThe numerator is: " << numerator << std::endl;
    
    if(denominator == 0) preMinusErrorTerm1 = 0;
    else preMinusErrorTerm1 = 2*muMinusTot*preNumerator/denominator;
    if(debugLevel > 1) std::cout << "\nThe preTerm1 for muMinusError is: " << preMinusErrorTerm1 << std::endl;
    if(denominator == 0) preMinusErrorTerm2 = 0;
    else preMinusErrorTerm2 = (preNumerator * preNumerator)/(denominator*denominator);
    if(debugLevel > 1) std::cout << "\nThe preTerm2 for muMinusError is: " << preMinusErrorTerm2 << std::endl;
    preMinusError = preMinusErrorTerm1 - preMinusErrorTerm2;
    if(debugLevel > 1) std::cout << "\nThe preMinusError is: " << preMinusError << std::endl;
    if(denominator == 0) prePlusErrorTerm1 = 0;
    else prePlusErrorTerm1 = 2*muPlusTot*preNumerator/denominator;
    if(debugLevel > 1) std::cout << "\nThe preTerm1 for muPlusError is: " << prePlusErrorTerm1 << std::endl;
    if(denominator == 0) prePlusErrorTerm2 = 0;
    else  prePlusErrorTerm2 = (preNumerator * preNumerator)/(denominator * denominator);
    if(debugLevel > 1) std::cout << "\nThe preTerm2 for muPlusError is: " << prePlusErrorTerm2 << std::endl;
    prePlusError = prePlusErrorTerm1 - prePlusErrorTerm2;
    if(debugLevel > 1) std::cout << "\nThe prePlusError is: " << prePlusError << std::endl;
    minusError = factor * preMinusError;
    if(debugLevel > 0) std::cout << "\nThe minusError is: " << minusError << std::endl;
    plusError = factor * prePlusError;
    if(debugLevel > 0) std::cout << "\nThe plusError is: " << plusError << std::endl;
    
    preError = (preMinusError * preMinusError) + (prePlusError * prePlusError);
    if(debugLevel > 0) std::cout << "\nThe preError is: " << preError << std::endl;
    error = error + sqrt(preError);
    
  }
  
  std::cout << "\nThe value for chi2 Error for this bin is: " << error << std::endl;

  return error;

}
*/
