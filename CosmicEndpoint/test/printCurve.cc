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

void printCurve(std::string const& inFile){
  
  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/"+inFile));
  if(f == 0){
    std::cout << "Error: cannot open file1! \nMake sure that you specified directory without '/' !\n";
    return;
  }
  TH1F* dataHist = (TH1F*)f->Get("TunePdataHist_Rebinned;2");
  TH1F* mcHist = (TH1F*)f->Get("TunePMC_Curve_Rebinned_NoBias");

  Int_t dataBins = dataHist->GetNbinsX();
  Int_t mcBins = mcHist->GetNbinsX();
  Double_t dataErr = 0.0;
  Double_t mcErr = 0.0;
  Double_t dataVal = 0.0;
  Double_t mcVal = 0.0;
  
  std::cout << "\n\nCRAFT15\tMCnoBias\tCRAFT_ERROR\tMC_ERROR\n_______\t________\t___________\t________\n" ;
    
    if(dataBins == mcBins){
      
      for(int i = 0; i < dataBins; i++){
	dataVal = dataHist->GetBinContent(i+1);
	mcVal = mcHist->GetBinContent(i+1);
	dataErr = dataHist->GetBinError(i+1);
	mcErr = mcHist->GetBinError(i+1);
	
	std::cout << dataVal << "\t" << mcVal << "\t\t" << dataErr << "\t\t" << mcErr <<  std::endl;
	
      }
    }
  
  return;
  
}
