{
  TFile *file = new TFile("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_4_12/src/WSUCosmicAnalysis/CosmicEndpoint/test/testOutQHP.root", "READ");
  std::cout << "The file reading is" << file << std::endl;

  std::cout << "\nThe file is open" <<   file->IsOpen()  << std::endl;

    std::cout << "\nThe file is Zombie" <<   file->IsZombie() << std::endl;
  TGraph *graph = (TGraph*)graph_tunep_Chi2; //file->Get("tunep_Chi2");

  std::cout << "\nThe graph to be read is " << graph << std::endl;
  TF1 *fitPoly = new TF1("fitPoly","pol6", -1,1);
  std::cout << "\nThe fit to be used is is " << fitPoly << std::endl;
  fitPoly->SetParameters(0,0,0,0,0,0,0);
  std::cout << "\nSet Parameters" << std::endl;
  graph->GetMinimum();
  
  graph->Fit("fitPoly","","",-1,1);
  std::cout << "\nFit the polynomial" << std::endl;

  graph->Draw("ep0");
  std::cout << "\nDrew graph" << std::endl;
  fitPoly->Draw("sames");
  std::cout << "\nDrew the Fit" << std::endl;

  graph->SaveAs("testGraphChi2.jpg");
  graph->SaveAs("testGraphChi2.pdf");
}
