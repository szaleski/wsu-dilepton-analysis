#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TH1.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TVector2.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TString.h"


// function prototype     
// the name of formal parameter can ba anything in a function protoype
void Plot( std::string const & histName, TH1F * hist, TFile *f, std::string const& xAxis, std::string const & plotName, int isLogy);


// genStudyHistogram is equivalent to fucntin main in c++
void makeThePlots(std::string const & inputfile) {


  TFile *outfile = new TFile("Mass.root", "RECREATE");
  // Create a histogram for the values we read.
  //TH1F *h_InvariantMass = new TH1F("hInvariantMass", "demoInvarinatMass", 100, 0, 1000);

  // Hisgrogram  for Invariant mass of Dimuon pair
  TH1F *h_InvariantMass = new TH1F("hInvariantMass", "dimuonInvarinatMass", 100, 0, 1000);

  // Histogram for kinematic variables of Muon1
  TH1F *h_Pt1 = new TH1F("hPt1", "MuonPt1", 100, 0, 1000);
  TH1F *h_Eta1 = new TH1F("hEta1", "MuonEta1", 100, -5.0, 5.0);
  TH1F *h_Phi1 = new TH1F("hPhi1", "MuonPhi1", 100, -4.0, 4.0);
  TH1F *h_Energy1 = new TH1F("hEnergy1", "MuonEnergy1", 100, 0, 1000);
  TH1F *h_Et1 = new TH1F("hEt1", "MuonEt1", 100, 0, 1000);
  TH1F *h_Mass1 = new TH1F("hMass1", "MuonMass1", 100, 0, 1000);
  TH1F *h_Theta1 = new TH1F("hTheta1", "MuonTheta1", 100, -5.0, 5.0);

 // Histograms for kinematic variable of Muon2
 TH1F *h_Pt2 = new TH1F("hPt2", "MuonPt2", 100, 0, 1000);
 TH1F *h_Eta2 = new TH1F("hEta2", "MuonEta2", 100, -5.0, 5.0);
 TH1F *h_Phi2 = new TH1F("hPhi2", "MuonPhi2", 100, -4.0, 4.0);
 TH1F *h_Energy2 = new TH1F("hEnergy2", "MuonEnergy2", 100, 0, 1000);
 TH1F *h_Et2 = new TH1F("hEt2", "MuonEt2", 100, 0, 1000);
 TH1F *h_Mass2 = new TH1F("hMass2", "MuonMass2", 100, 0, 1000);
 TH1F *h_Theta2 = new TH1F("hTheta2", "MuonTheta2", 100, -4.0, 4.0);


 // Histograms for kinematic variable of boson                                                                                                                                     
 TH1F *h_boson_Pt = new TH1F("hBosonPt", "BosonPt", 100, 0, 1000);
 TH1F *h_boson_Eta = new TH1F("hBosonEta", "BosonEta", 100, -5.0, 5.0);
 TH1F *h_boson_Phi = new TH1F("hBosonPhi", "BosonPhi", 100, -4.0, 4.0);
 TH1F *h_boson_Energy = new TH1F("hBosonEnergy", "BosonEnergy", 100, 0, 1000);
 TH1F *h_boson_Et = new TH1F("hBosonEt", "BosonEt", 100, 0, 1000);
 TH1F *h_boson_Mass = new TH1F("hBosonMass", "BosonMass", 100, 0, 1000);
 TH1F *h_boson_Theta = new TH1F("hBosonTheta", "BosonTheta", 100, -4.0, 4.0);


 // Histograms for PID
 TH1F *h_Decay1_PID = new TH1F("hDecay1PID", "Decay1PID", 100, 0, 1000);
 TH1F *h_Decay2_PID = new TH1F("hDecay2PID", "Decay1PID", 100, 0, 1000);
 TH1F *h_Boson_PID = new TH1F("hBosonPID", "BosonPID", 100, 0, 1000);
 
 // Histogram for Cross section
  TH1F *h_cross_section = new TH1F("hcrossSection", "CrossSection", 100, 0, 1000);

// Open the file containing the tree. inputfile is given from command  line.
// Syntax for compiling and passing the file name inputfile from command line:
// root -x -b -q genStudyHistogram.cc++(\"Pythia8_Sep21_CI_L13000_13TeV_Sep21.root\")

  TFile *myFile = TFile::Open(TString("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test/" + inputfile) );

// Suceeds if a pointer myFile is a null
// Null pointer is assumed to point nothing

 if (!myFile)
   {

     std::cout << "Failed to open the file" << std::endl;
     return;
   }
 else 
   {
     std::cout << "File opened sucessfully" << std::endl;
     
   }// end of checkig input file 

 TTreeReader massReader ("Dimuon/pdfTree", myFile);

 // kinematic varibales for muon1 
  TTreeReaderValue<Float_t> muon1Pt(massReader, "decay1P4.pt");
  TTreeReaderValue<Float_t> muon1Eta(massReader, "decay1P4.eta");
  TTreeReaderValue<Float_t> muon1Phi(massReader, "decay1P4.phi");
  // Added items
  TTreeReaderValue<Float_t> muon1Energy(massReader, "decay1P4.energy");
  TTreeReaderValue<Float_t> muon1Et(massReader, "decay1P4.et");
  TTreeReaderValue<Float_t> muon1Mass(massReader, "decay1P4.mass");
  TTreeReaderValue<Float_t> muon1Theta(massReader, "decay1P4.theta");


  // kinematic variables for Muon2
  TTreeReaderValue<Float_t> muon2Pt(massReader, "decay2P4.pt");
   TTreeReaderValue<Float_t> muon2Eta(massReader, "decay2P4.eta");
  TTreeReaderValue<Float_t> muon2Phi(massReader, "decay2P4.phi");
  //Added items
  TTreeReaderValue<Float_t> muon2Energy(massReader, "decay2P4.energy");
  TTreeReaderValue<Float_t> muon2Et(massReader, "decay2P4.et");
  TTreeReaderValue<Float_t> muon2Mass(massReader, "decay2P4.mass");
  TTreeReaderValue<Float_t> muon2Theta(massReader, "decay2P4.theta");

  // kinematic variables for boson
  TTreeReaderValue<Float_t> bosonPt(massReader, "bosonP4.pt");
  TTreeReaderValue<Float_t> bosonEta(massReader, "bosonP4.eta");
  TTreeReaderValue<Float_t> bosonPhi(massReader, "bosonP4.phi");                                                                                                              
  TTreeReaderValue<Float_t> bosonEnergy(massReader, "bosonP4.energy");
  TTreeReaderValue<Float_t> bosonEt(massReader, "bosonP4.et");
  TTreeReaderValue<Float_t> bosonMass(massReader, "bosonP4.mass");
  TTreeReaderValue<Float_t> bosonTheta(massReader, "bosonP4.theta");

  // TTreeReaderValues for PID
   TTreeReaderValue<Int_t> decay1_PID(massReader, "decay1PID");
   TTreeReaderValue<Int_t> decay2_PID(massReader, "decay2PID");
   TTreeReaderValue<Int_t> boson_PID(massReader, "bosonPID");


  // TTreeReaerValye for cross section
   TTreeReaderValue<Double_t> cross_Section(massReader, "crossSec");
   
   while (massReader.Next())
     { 
       // Fills histogram for Invariant Mass of Dimuon Pair
       h_InvariantMass->Fill(sqrt( 2 * (*muon1Pt) * (*muon2Pt)*(cosh ((* muon1Eta) - (*muon2Eta)  ) - cos(TVector2::Phi_mpi_pi((* muon1Phi)- (*muon2Phi)) ))));

       // Fills histogram for kinematic variables of Muon1
       h_Pt1->Fill(*muon1Pt);
       h_Eta1->Fill(*muon1Eta);
       h_Phi1->Fill(*muon1Phi);
       h_Energy1->Fill(*muon1Energy);
       h_Et1->Fill(*muon1Et);
       h_Mass1->Fill(*muon1Mass);
       h_Theta1->Fill(*muon1Theta);

       // Fills histogram for Kinematic variales of Muon2
        h_Pt2->Fill(*muon2Pt);
	h_Eta2->Fill(*muon2Eta);
	h_Phi2->Fill(*muon2Phi);
	h_Energy2->Fill(*muon2Energy);
	h_Et2->Fill(*muon2Et);
	h_Mass2->Fill(*muon2Mass);
	h_Theta2->Fill(*muon2Theta);

	// Fills histogram for Kinematic variales of boson                                                                                                                        
        h_boson_Pt->Fill(*bosonPt);
        h_boson_Eta->Fill(*bosonEta);
        h_boson_Phi->Fill(*bosonPhi);
        h_boson_Energy->Fill(*bosonEnergy);
        h_boson_Et->Fill(*bosonEt);
        h_boson_Mass->Fill(*bosonMass);
        h_boson_Theta->Fill(*bosonTheta);
	

	//Fills histogram for PIDs
	h_Decay1_PID->Fill(*decay1_PID);
        h_Decay2_PID->Fill(*decay2_PID);
        h_Boson_PID->Fill(*boson_PID);

	// Fills histogram for cross section
	h_cross_section->Fill(*cross_Section);
	
}

   // function call for Invariant Masss of Dimuon pair  

   int isLogy = 1;
   Plot("Dimuon_Mass", h_InvariantMass, outfile, "Mass in Gev","Dimuon_Mass", isLogy);
   //function call for kinematic variables of Muon1
   Plot("Muon1_Pt", h_Pt1, outfile, "Muon1Pt  in Gev","Dimuon1_Pt", isLogy);
   Plot("Muon1_M", h_Mass1, outfile, "Muon1Mass in Gev","Dimuon1_Mass", isLogy);
   Plot("Muon2_Pt", h_Pt2, outfile, "Muon2Pt  in Gev","Dimuon2_Pt",isLogy);
   Plot("Muon2_M", h_Mass2, outfile, "Muon2Mass in Gev","Dimuon2_Mass", isLogy);
   Plot("Boson_Pt", h_boson_Pt, outfile, "BosonPt  in Gev","Boson_Pt", isLogy);
   Plot("Boson_M", h_boson_Mass, outfile, "BosonMass in Gev","Bososn_Mass", isLogy);

   isLogy =0;

   //Plot call for muon 1
   Plot("Muon1_Eta", h_Eta1, outfile, "Muon1Eta is dimensionless","Dimuon1_Eta", isLogy);
   Plot("Muon1_Phi", h_Phi1, outfile, "Muon1Phi in radians","Dimuon1_Phi", isLogy);
   Plot("Muon1_E", h_Energy1, outfile, "Muon1Energy  in Gev","Dimuon1_Energy", isLogy );
   Plot("Muon1_Et", h_Et1, outfile, "Muon1Et(transverse energy) in Gev","Dimuon1_Et", isLogy);
   Plot("Muon1_Theta", h_Theta1, outfile, "Muon1Theta in radians","Dimuon1_Theta", isLogy);
   
   //Plot call for muon 2
   Plot("Muon2_Eta", h_Eta2, outfile, "Muon2Eta is dimensionless","Dimuon2_Eta", isLogy);
   Plot("Muon2_Phi", h_Phi2, outfile, "Muon2Phi in radians","Dimuon2_Phi", isLogy);
   Plot("Muon2_E", h_Energy2, outfile, "Muon2Energy  in Gev","Dimuon2_Energy", isLogy);
   Plot("Muon2_Et", h_Et2, outfile, "Muon2Et(transverse energy) in Gev","Dimuon2_Et", isLogy);
   Plot("Muon2_Theta", h_Theta2, outfile, "Muon2Theta in radians","Dimuon2_Theta", isLogy);

   //Plot call for boson
   Plot("Boson_Eta", h_boson_Eta, outfile, "BosonEta is dimensionless","Boson_Eta", isLogy);
   Plot("Boson_Phi", h_boson_Phi, outfile, "BosonPhi in radians","Boson_Phi", isLogy);
   Plot("Boson_E", h_boson_Energy, outfile, "BosonEnergy  in Gev","Bososn_Energy", isLogy);
   Plot("Boson_Et", h_boson_Et, outfile, "BosonEt(transverse energy) in Gev","Boson_Et", isLogy);
   Plot("Boson_theta", h_boson_Theta, outfile, "BosonTheta in radians","Boson_Theta", isLogy);

   // function call for PIDs
   Plot("Muon2_PID", h_Decay1_PID, outfile, "Decay1 PID in numbers","Decay1_PID", isLogy);
   Plot("Muon2_PID", h_Decay2_PID, outfile, "Decay2 PID in numbers","Decay2_PID", isLogy);
   Plot("Boson_PID", h_Boson_PID, outfile, "Boson  PID in numbers","Boson_PID", isLogy);

     // function call  for cross section
   Plot("Cross_Section", h_cross_section, outfile, "Cross section in pico barn,","cross_Section", isLogy);
   

     outfile->Write();
     outfile->Close();
     return;

}

// function definition Plot
void Plot( std::string const & histName, TH1F * hist, TFile *f, std::string const& xAxis, std::string const & plotName, int isLogy)
{
  TCanvas *c = new TCanvas(TString(histName+"_Canvas"),TString(histName),1000,1000);

  f->cd();
  c->cd();
  // h_InvariantMass->SetMarkerColor(kRed + 2);                                                                                                                                   
  hist->SetLineColor(kRed + 2);
  hist->SetLineWidth(2);
  hist->SetLineStyle(1);
  //     h_InvariantMass->SetMarkerStyle(33);                                                                                                                                     
  hist->Draw();
  
  // sets Y axis into LogY if we pass the argument 1 in a function call
  // Y axis stays same if we pass the argument 0 in a functin call
  if(isLogy == 1)      c->SetLogy(1);
  else c->SetLogy(0);
  // end of if statement to set Y into LogY

  
  hist->GetXaxis()->SetTitle(TString(xAxis));
  hist->GetYaxis()->SetTitle("Number of Dimuons");
  c->Update();
  c->SaveAs(TString(plotName + ".png"));
  //  f->cd();
  c->Write();
  return;

}//end of function definition Plot



