#include <iostream>
#include <iomanip>
#include <memory>
#include <fstream>
#include <sstream>
#include <math.h>
//#include <libconfig.h++>

#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>
#include <TGraphErrors.h>
#include <TGraph.h>

#include "HistogramMaker.h"

namespace wsu {
  namespace dileptons {
    
    typedef struct endpointParameters {
      int nbins;
      std::string algo;
      std::string arb;
      std::string leg;
      
    } endpointParameters;

    double getChi2(TH1D* hobs, TH1D* href) {
      if (hobs->GetNbinsX() != href->GetNbinsX()) {
	std::cout << "histograms have different number of bins in X:" << std::endl
		  << hobs->GetName() << " has " << hobs->GetNbinsX()       << std::endl
		  << href->GetName() << " has " << href->GetNbinsX()       << std::endl;
	return -1.;
      }
      
      int nBins = hobs->GetNbinsX();
      double cChi2 = 0.;
      for (int b = 0; b < nBins; ++b) {
	double obs = hobs->GetBinContent(b+1);
	double ref = href->GetBinContent(b+1);
	
	double binChi2 = pow((obs-ref),2)/(ref);
	cChi2 += binChi2;
      }
      return cChi2;
    } // end getChi2
    
    TH1D* normalizeHist(TH1D* hobs, TH1D* href) {
      double obsInt = hobs->Integral();
      double refInt = href->Integral();
      TH1D* scaled = (TH1D*)hobs->Clone("scaled");
      scaled->Scale(refInt/obsInt);
      return scaled;
    }

    TGraphErrors* runMinimizationErrors(TFile* inFile, endpointParameters const& parms) {
      std::string histBase = "";
      if (parms.leg=="upper") {
	histBase += "upper";
      } else if (parms.leg=="lower") {
	histBase += "lower";
      } else if (parms.leg=="all") {
	histBase += "all";
      } else {
	std::cout << "Incorrect leg selection: " << parms.leg << " must be either 'upper', 'lower', or 'all'" << std::endl;
	return NULL;
      }

      double biasVals[parms.nbins*2+1];
      double chi2Vals[parms.nbins*2+1];
      double biasErrs[parms.nbins*2+1];
      double chi2Errs[parms.nbins*2+1];
      biasVals[parms.nbins+1] = 0; // middle of the array should be x=0
      biasErrs[parms.nbins+1] = 0; // middle of the array should be x=0

      std::string plusHist  = histBase + "CurvePlusBias";
      std::string minusHist = histBase + "CurveMinusBias";
      TH1D* qPtPlus  = (TH1D*)inFile->Get(plusHist.c_str());
      TH1D* qPtMinus = (TH1D*)inFile->Get(minusHist.c_str());
      TH1D *ref, *obs;
      if (parms.arb == "plus") {
	ref = (TH1D*)qPtPlus->Clone("scaled");
	obs = (TH1D*)qPtMinus->Clone("scaled");
	
      } else if (parms.arb == "minus") {
	ref = (TH1D*)qPtMinus->Clone("scaled");
	obs = (TH1D*)qPtPlus->Clone("scaled");
      } else {
	std::cout << "Incorrect arbitration specified: " << parms.arb << " must be either 'plus' or 'minus'" << std::endl;
	return NULL;
      }

      obs = normalizeHist(obs,ref);
      
      biasVals[parms.nbins+1] = 0; // middle of the array should be x=0
      biasErrs[parms.nbins+1] = 0; // middle of the array should be x=0

      chi2Vals[parms.nbins+1] = getChi2(obs,ref);
      chi2Errs[parms.nbins+1] = 0;// what is the error on the chi2?

      // didn't call new so don't have to clean up? just moving the pointer?
      // delete ref, obs, qPtPlus, qPtMinus;
      
      for (int inj = 0; inj < parms.nbins; ++inj) {
	std::stringstream histname;
	histname << plusHist << std::setw(3) << std::setfill('0') << (inj+1);
	qPtPlus  = (TH1D*)inFile->Get(TString(histname.str()));

	histname.str(std::string());
	histname.clear();
	histname << minusHist << std::setw(3) << std::setfill('0') << (inj+1);
	qPtMinus = (TH1D*)inFile->Get(TString(histname.str()));
	
	// positive bias
	if (parms.arb == "plus") {
	  ref = (TH1D*)qPtPlus->Clone("scaled");
	  obs = (TH1D*)qPtMinus->Clone("scaled");
	  
	} else if (parms.arb == "minus") {
	  ref = (TH1D*)qPtMinus->Clone("scaled");
	  obs = (TH1D*)qPtPlus->Clone("scaled");
	} else {
	  std::cout << "Incorrect arbitration specified: " << parms.arb << " must be either 'plus' or 'minus'" << std::endl;
	  return NULL;
	}
	
	obs = normalizeHist(obs,ref);
	
	// positive bias
	biasVals[parms.nbins+1+inj+1] = 0; // middle of the array should be x=0
	biasErrs[parms.nbins+1+inj+1] = 0; // middle of the array should be x=0
	
	chi2Vals[parms.nbins+1+inj+1] = getChi2(obs,ref);
	chi2Errs[parms.nbins+1+inj+1] = 0;// what is the error on the chi2?
	
	chi2Vals[parms.nbins+1+inj+1] = getChi2(obs,ref);

	// negative bias
	biasVals[parms.nbins-inj] = 0; // middle of the array should be x=0
	biasErrs[parms.nbins-inj] = 0; // middle of the array should be x=0
	
	chi2Vals[parms.nbins-inj] = getChi2(obs,ref);
	chi2Errs[parms.nbins-inj] = 0;// what is the error on the chi2?
	
	chi2Vals[parms.nbins+1+inj+1] = getChi2(obs,ref);
	// didn't call new so don't have to clean up? just moving the pointer?
	// delete ref, obs, qPtPlus, qPtMinus;
      }
      TGraphErrors* chi2VsBiasGraph = new TGraphErrors((2*parms.nbins+1),biasVals,chi2Vals,biasErrs,chi2Errs);
      return chi2VsBiasGraph;
      // hack for now as there is no return type yet
      // return NULL;
    }
    
    /*
    TGraph* runMinimization(TFile const& inFile, endpointParameters const& parms) {
      
    }
    */


  } // end namespace dileptons
} // end namespace wsu

// main function part
//#include "optionparser.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <iterator>

namespace po = boost::program_options;

int main(int argc, char **argv)
{
  // parse the command line options
  std::string inputFiles;
  std::string outputFile;
  std::string configFile;
  int debug;
  try {
    po::options_description desc("Allowed options");

    desc.add_options()
      ("help,h", "produce help message")
      ("debug,d",  po::value<int>(&debug)->default_value(0), "set debug level, default is 0")
      ("inputs,i", po::value<std::string>(&inputFiles)->required(), "input file list, a text file with the paths of the input files")
      ("output,o", po::value<std::string>(&outputFile)->required(), "output file name")
      ("config,c", po::value<std::string>(&configFile)->required(), "config file name");

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
    }
    
    // now since we haven't printed the help, do the parsing and see if any required arguments are missing
    po::notify(vm);

    if (vm.count("debug")) {
      std::cout << "Debug level was set to " 
	   << vm["debug"].as<int>() << "." << std::endl;
    } else {
      std::cout << "Debug level was not set, default is off" << std::endl;
    }
    
  } catch (boost::program_options::required_option& e) {
    std::cerr << "missing required option: " << e.what() << "" << std::endl;
    return 1;
  } catch (std::exception& e) {
    std::cerr << "error: " << e.what() << "" << std::endl;
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!" << std::endl;
  }
  
  //main body
  std::cout << "parsed command line options, running endpoint study" << std::endl;
  
  wsu::dileptons::cosmics::HistogramMaker myHistograms(inputFiles, outputFile, configFile, debug);
  int nEvents = myHistograms.runLoop(0);
  std::cout << "finished running endpoint study, processed " << nEvents << " events" << std::endl;
  return 0;
}
