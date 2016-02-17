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
#include "TPad.h"
#include "TPaveStats.h"
#include "TString.h"

#include <iostream>
#include <fstream>


void findDiff(std::string const& file){

  std:: cout << "The file to be opened is: " << file << std::endl;

  TFile *f = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/MuonAnalyzer/test/"+file));
  if(f == 0){
    std::cout << "Error: cannot open file\n";
    return;
  }

  std::cout << "Successfully opened file! \n\n";

  TTreeReader muonReader("analysisMuons/MuonTree", f);
  TTreeReader globalReader("analysisGlobalMuons/MuonTree", f);
  TTreeReader lowerReader("analysisLowerTagTunePMuons/MuonTree", f);
  TTreeReader upperReader("analysisUpperTagTunePMuons/MuonTree", f);

  TTreeReaderValue<Int_t> muonEvent(muonReader, "muonEventNumber");
  TTreeReaderValue<Int_t> muonRun(muonReader, "muonRunNumber");
  TTreeReaderValue<Int_t> muonLumi(muonReader, "muonLumiBlock");
  TTreeReaderValue<Double_t> muonUpperPt(muonReader, "upperMuon_pT");
  TTreeReaderValue<Double_t> muonLowerPt(muonReader, "lowerMuon_pT");
  TTreeReaderValue<Int_t> muonUpperGlobal(muonReader, "upperMuon_isGlobal");
  TTreeReaderValue<Int_t> muonLowerGlobal(muonReader, "lowerMuon_isGlobal");
  TTreeReaderValue<Int_t> muonUpperPixelHits(muonReader, "upperMuon_pixelHits");
  TTreeReaderValue<Int_t> muonLowerPixelHits(muonReader, "lowerMuon_pixelHits");
  TTreeReaderValue<Int_t> muonFoundMatch(muonReader, "foundMatch");
  TTreeReaderValue<Int_t> globalEvent(globalReader, "muonEventNumber");
  TTreeReaderValue<Int_t> globalRun(globalReader, "muonRunNumber");
  TTreeReaderValue<Int_t> globalLumi(globalReader, "muonLumiBlock");
  TTreeReaderValue<Double_t> globalUpperPt(globalReader, "upperMuon_pT");
  TTreeReaderValue<Double_t> globalLowerPt(globalReader, "lowerMuon_pT");
  TTreeReaderValue<Int_t> globalUpperGlobal(globalReader, "upperMuon_isGlobal");
  TTreeReaderValue<Int_t> globalLowerGlobal(globalReader, "lowerMuon_isGlobal");
  TTreeReaderValue<Int_t> globalUpperPixelHits(globalReader, "upperMuon_pixelHits");
  TTreeReaderValue<Int_t> globalLowerPixelHits(globalReader, "lowerMuon_pixelHits");
  TTreeReaderValue<Int_t> globalFoundMatch(globalReader, "foundMatch");
  TTreeReaderValue<Int_t> lowerEvent(lowerReader, "muonEventNumber");
  TTreeReaderValue<Int_t> lowerRun(lowerReader, "muonRunNumber");
  TTreeReaderValue<Int_t> lowerLumi(lowerReader, "muonLumiBlock");
  TTreeReaderValue<Double_t> lowerUpperPt(lowerReader, "upperMuon_pT");
  TTreeReaderValue<Double_t> lowerLowerPt(lowerReader, "lowerMuon_pT");
  TTreeReaderValue<Int_t> lowerUpperGlobal(lowerReader, "upperMuon_isGlobal");
  TTreeReaderValue<Int_t> lowerLowerGlobal(lowerReader, "lowerMuon_isGlobal");
  TTreeReaderValue<Int_t> lowerUpperPixelHits(lowerReader, "upperMuon_pixelHits");
  TTreeReaderValue<Int_t> lowerLowerPixelHits(lowerReader, "lowerMuon_pixelHits");
  TTreeReaderValue<Int_t> lowerFoundMatch(lowerReader, "foundMatch");
  TTreeReaderValue<Int_t> upperEvent(upperReader, "muonEventNumber");
  TTreeReaderValue<Int_t> upperRun(upperReader, "muonRunNumber");
  TTreeReaderValue<Int_t> upperLumi(upperReader, "muonLumiBlock");
  TTreeReaderValue<Double_t> upperUpperPt(upperReader, "upperMuon_pT");
  TTreeReaderValue<Double_t> upperLowerPt(upperReader, "lowerMuon_pT");
  TTreeReaderValue<Int_t> upperUpperGlobal(upperReader, "upperMuon_isGlobal");
  TTreeReaderValue<Int_t> upperLowerGlobal(upperReader, "lowerMuon_isGlobal");
  TTreeReaderValue<Int_t> upperUpperPixelHits(upperReader, "upperMuon_pixelHits");
  TTreeReaderValue<Int_t> upperLowerPixelHits(upperReader, "lowerMuon_pixelHits");
  TTreeReaderValue<Int_t> upperFoundMatch(upperReader, "foundMatch");

  bool muonChk = muonReader.Next();
  bool globalChk = globalReader.Next();
  bool lowerChk = lowerReader.Next();
  bool upperChk = upperReader.Next();

  ofstream muonFile;
  ofstream globalFile;
  ofstream lowerFile;
  ofstream upperFile;

  muonFile.open("analysisMuons.txt");
  // while(muonChk || globalChk || lowerChk || upperChk){
  while(muonChk){
    if(muonChk && (*muonUpperPixelHits > 0) && (*muonLowerPixelHits > 0) && (*muonFoundMatch > 0) && (*muonUpperPt > 100) && (*muonLowerPt > 100) && (*muonUpperGlobal > 0) && (*muonLowerGlobal > 0)){
      //   std::cout << "\nMuon Event# " << *muonEvent << "\t Run# " << *muonRun << "\t LumiSection: " << *muonLumi << std::endl;
      muonFile << *muonRun << "/" << *muonLumi << "/" << *muonEvent << std::endl;
    }
  muonChk = muonReader.Next();
  }
  muonFile.close();

  globalFile.open("analysisGlobalMuons.txt");
  while(globalChk){
    if(globalChk && (*globalUpperGlobal > 0) && (*globalLowerGlobal > 0) && (*globalUpperPixelHits > 0) && (*globalLowerPixelHits > 0) && (*globalFoundMatch > 0) && (*globalUpperPt > 100) && (*globalLowerPt > 100)){
      //      std::cout << "\nGlobal Event# " << *globalEvent << "\t Run# " << *globalRun << "\t LumiSection: " << *globalLumi << std::endl;
      globalFile << *globalRun << "/" << *globalLumi << "/" << *globalEvent << std::endl;
    }
  globalChk = globalReader.Next();
  }
  globalFile.close();


  lowerFile.open("analysisLowerTag.txt");
  while(lowerChk){
    if(lowerChk  && (*lowerUpperGlobal > 0) && (*lowerLowerGlobal > 0) && (*lowerUpperPixelHits > 0) && (*lowerLowerPixelHits > 0) && (*lowerFoundMatch > 0) && (*lowerUpperPt > 100) && (*lowerLowerPt > 100)){
      //      std::cout << "\nLower Tag Event# " << *lowerEvent << "\t Run# " << *lowerRun << "\t LumiSection: " << *lowerLumi << std::endl;
      lowerFile << *lowerRun << "/" << *lowerLumi << "/" << *lowerEvent << std::endl;
    }
  lowerChk = lowerReader.Next();
  }
  lowerFile.close();

  upperFile.open("analysisUpperTag.txt");
  while(upperChk){
    if(upperChk && (*upperUpperGlobal > 0) && (*upperLowerGlobal > 0) && (*upperUpperPixelHits > 0) && (*upperLowerPixelHits > 0) && (*upperFoundMatch > 0) && (*upperUpperPt > 100) && (*upperLowerPt > 100)){
      //      std::cout << "\nUpper Tag Event# " << *upperEvent << "\t Run# " << *upperRun << "\t LumiSection: " << *upperLumi << std::endl;
      upperFile << *upperRun << "/" << *upperLumi << "/" << *upperEvent << std::endl;
    }
    upperChk = upperReader.Next();
  }
  upperFile.close();

  
  

  

  std::cout << "\n\nList Done!";

  return;

}
