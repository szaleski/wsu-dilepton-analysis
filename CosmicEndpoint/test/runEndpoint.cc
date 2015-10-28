#include <iostream>
#include <memory>
#include <math>

#include <TROOT.h>
#include <TH1D.h>

namespace wsu {
  namespace dileptons {
    
    struct endpointParameters {
      int nbins;
      std::string algo;
      std::string arb;
      std::string leg;
      
    } endpointParameters;

    double getChi2(TH1D const& hobs, TH1D const& href) {
      if (hobs.GetBinsX() != href.GetBinsX()) {
	std::cout << "histograms have different number of bins in X:" << std::endl
		  << hobs.GetName() << " has " << hobs.GetBinsX()       << std::endl
		  << href.GetName() << " has " << href.GetBinsX()       << std::endl;
	return -1.;
      }
      
      int nBins = obs.GetBinsX();
      double cChi2 = 0.;
      for (int b = 0; b < nBins; ++b) {
	double obs = hobs.GetBinContent(b+1);
	double ref = href.GetBinContent(b+1);
	
	double binChi2 = (obs - ref)**2/(ref)
	  }
      return cChi2;
    } // end getChi2
    
    TGraphErrors runMinimizationErrors(TFile const& inFile, endpointParameters const& parms) {
      std::string histBase = "";
      if (parms.leg=="upper") {
	histBase += "upper";
      } else if (parms.leg=="lower") {
	histBase += "lower";
      } else if (parms.leg=="all") {
	histBase += "all";
      } else {
	std::cout << "Incorrect leg selection: " << params.leg << " must be either 'upper', 'lower', or 'all'" << std::endl;
	exit(1);
      }
      double biasVals[parms.nbins*2+1];
      double chi2Vals[parms.nbins*2+1];
      biasVals[params.nbins+1] = 0; // middle of the array should be x=0

      std::string plusHist  = histBase + "CurvePlusBias";
      std::string minusHist = histBase + "CurveMinusBias";
      TH1D* qPtPlus  = (TH1D*)inFile->Get(TString(histname.str()));
      TH1D* qPtMinus = (TH1D*)inFile->Get(minusHist);
      TH1D *ref, *obs;
      if (params.arb == "plus") {
	ref = qPtPlus->Clone();
	obs = qPtMinus->Clone();
	
      } else if (params.arb == "minus") {
	ref = qPtMinus->Clone();
	obs = qPtPlus->Clone();
      } else {
	std::cout << "Incorrect arbitration specified: " << params.arb << " must be either 'plus' or 'minus'" << std::endl;
	exit(1);
      }

      &obs = normalizeHist(&obs,&ref);
      
      chi2Vals[parms.nbins+1] = getChi2 ();

      // didn't call new so don't have to clean up? just moving the pointer?
      // delete ref, obs, qPtPlus, qPtMinus;
      
      for (int inj = 0; inj < parms.nbins; ++inj) {
	std::stringstream histname;
	histname << plusHist << std::setw(3) << std::setfill('0') << (inj+1);
	qPtPlus  = (TH1D*)inFile->Get(TString(histname.str()));

	histname.str(std::string());
	histname.clear();
	histname << minusHist << std::setw(3) << std::setfill('0') << (inj+1);
	qPtMinus = (TH1D*)inFile->Get(minusHist);
	
	// positive bias
	if (params.arb == "plus") {
	  ref = qPtPlus->Clone();
	  obs = qPtMinus->Clone();
	  
	} else if (params.arb == "minus") {
	  ref = qPtMinus->Clone();
	  obs = qPtPlus->Clone();
	} else {
	  std::cout << "Incorrect arbitration specified: " << params.arb << " must be either 'plus' or 'minus'" << std::endl;
	  exit(1);
	}
	
	&obs = normalizeHist(&obs,&ref);
	
	chi2Vals[parms.nbins+1+inj+1] = getChi2 ();
	
	// didn't call new so don't have to clean up? just moving the pointer?
	// delete ref, obs, qPtPlus, qPtMinus;
      }
    }
    
    TGraph runMinimization(TFile const& inFile, endpointParameters const& parms) {
      
    }
    
    TH1D normalizeHist(TH1D const& hobs, TH1D const& href) {
      double obsInt = hobs.GetIntegral();
      double refInt = href.GetIntegral();
      TH1D scaled = hobs.Clone();
      scaled.Scale(refInt/obsInt);
      return scaled;
    }
  } // end namespace dileptons
} // end namespace wsu
