{
  TFile *file = new TFile("/afs/cern.ch/work/s/szaleski/private/CMSSW_7_6_3_patch2/src/WSUDiLeptons/CosmicEndpoint/test", "READ");
  std::cout << "The file reading is" << file << std::endl;

  std::cout << "\nThe file is open" <<   file->IsOpen()  << std::endl;

    std::cout << "\nThe file is Zombie" <<   file->IsZombie() << std::endl;
    TGraph *graph = (TGraph*)file->Get("obs_Minus/graph_tunep_Chi2"); //file->Get("tunep_Chi2");

  std::cout << "\nThe graph to be read is " << graph << std::endl;
  TF1 *fitPoly = new TF1("fitPoly","pol10", -1,1);
  std::cout << "\nThe fit to be used is is " << fitPoly << std::endl;
  fitPoly->SetParameters(0,0,0,0,0,0,0);
  std::cout << "\nSet Parameters" << std::endl;

  
  graph->Fit("fitPoly","VEMFR","",-6,6);
  std::cout << "\nFit the polynomial" << std::endl;

  graph->Draw("alp");
  std::cout << "\nDrew graph" << std::endl;
  fitPoly->Draw("sames");
  std::cout << "\nDrew the Fit" << std::endl;

 double minX = fitPoly->GetMinimumX(- 0.5, 0.5, 1.E-10,100, 0);
 double minY = fitPoly->GetMinimum(-0.5,0.5,1.E-10,100,0);
 std::cout << "\nMinX is: " << minX;
 std::cout << "\nMinY is: " << minY << std::endl << std::endl;


}
