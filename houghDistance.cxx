#include <iostream>
#include <sstream>
#include <string>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TApplication.h>
#include <TText.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  CaliceStyle();
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);
  TCanvas *can=new TCanvas();
  can->SetWindowSize(400,400);

  TH1D* hDistance=new TH1D("hDistance","",100,0,50);
  hDistance->SetLineWidth(2);
  hDistance->SetLineColor(kBlack);
  hDistance->GetXaxis()->SetTitle("Distance to vertex [mm]");
  hDistance->GetXaxis()->SetTitleOffset(0.9);
  hDistance->GetYaxis()->SetTitle("# events");
  hDistance->GetYaxis()->SetTitleOffset(1.1);
  
  char fileName[200];
  sprintf(fileName,"%s%d%s","/data/root/houghEfficiencyProcessor/single_mu-_20GeV_",atoi(argv[1]),".root");
  TFile file(fileName,"READ");
  if( file.IsOpen() )
    file.Print();
  else
    std::cout << "can not open " << fileName << std::endl;
  TTree *tree = (TTree*)file.Get("tree");
  if (!tree){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return 0;
  }

  float distanceToVertex;
  tree->SetBranchAddress("distanceToVertex",&distanceToVertex);
  const unsigned nEvts = tree->GetEntries();
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
    tree->GetEntry(ievt);
    hDistance->Fill(distanceToVertex);
    if( (ievt+1)%1000==0 )
      std::cout << "Entry " << ievt+1 << std::endl;
  }

  hDistance->Draw();

  TText *tex=new TText();
  tex->SetTextColor(kGray+2);
  tex->SetTextSize(0.035);

  std::ostringstream text(std::ostringstream::ate);
  text.str("Noise probability = ");
  text << (float)atoi(argv[1])/10.0 << "%";
  tex->DrawTextNDC(0.4,0.96,text.str().c_str());

  text.precision(3);
  text.str("Mean = ");
  text << hDistance->GetMean() << " +/- " << hDistance->GetMeanError();
  tex->DrawTextNDC(0.4,0.7,text.str().c_str());
  text.str("RMS = ");
  text << hDistance->GetRMS() << " +/- " << hDistance->GetRMSError();
  tex->DrawTextNDC(0.4,0.65,text.str().c_str());
  
  can->Update();
  // can->WaitPrimitive();
  
  std::ostringstream os(std::ostringstream::ate);
  os.str("./plots/distanceToVertex");
  os << atoi(argv[1]) << ".pdf";
  can->SaveAs(os.str().c_str());

  file.Close();
  delete hDistance;
  delete can;
  return 0;
}
