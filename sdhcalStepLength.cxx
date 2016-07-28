#include <iostream>
#include <sstream>
#include <string>

#include <TROOT.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TTree.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TApplication.h>
#include <TText.h>
#include <TArrow.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);
  gStyle->SetOptStat(0);
  TCanvas *can=new TCanvas();
  can->SetWindowSize(600,600);
  can->SetFillColor(0);
  can->SetBorderMode(0);
  can->SetBorderSize(2);
  can->SetRightMargin(0.15);
  can->SetTopMargin(0.125);
  can->SetBottomMargin(0.125);
  can->SetFrameBorderMode(0);
  
  TH2D* hLDz=new TH2D("hDLDz","",100,-0.6,0.6,100,0,0.3);
  hLDz->GetXaxis()->SetTitle("#Delta_{z} [mm]");
  hLDz->GetYaxis()->SetTitle("step length [mm]");
  hLDz->GetYaxis()->SetTitleOffset(1.4);
  hLDz->GetZaxis()->SetTitle("number of entries");
  hLDz->GetZaxis()->SetTitleOffset(1.2);
  
  TFile file("/Users/arnaudsteen/SDHCAL/data/aida_pi-_30GeV.root","READ");
  if( file.IsOpen() )
    file.Print();
  TDirectory* directory=file.GetDirectory("MySimDigital");
  directory->cd();
  TTree *tree = (TTree*)directory->Get("SimDigitalStep");
  if (!tree){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return 0;
  }
  tree->Print();

  Float_t stepLength;
  Float_t deltaLayer;
  tree->SetBranchAddress("stepLength",&stepLength);
  tree->SetBranchAddress("deltaLayer",&deltaLayer);
  const unsigned nEvts = tree->GetEntries();
  std::cout << nEvts << std::endl;
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
     tree->GetEntry(ievt);
     if( stepLength<0.3 )
       hLDz->Fill( deltaLayer ,
		   stepLength );
     
     if( (ievt+1)%1000000==0 )
       std::cout << "Entry " << ievt+1 << std::endl;
  }

  can->SetLogz();
  hLDz->Draw("colz");
  TArrow *ar1 = new TArrow(-0.45,0.04,-0.58,0.005,0.03,"|>");
  ar1->SetAngle(60);
  ar1->SetLineWidth(2);
  ar1->SetFillColor(1);
  ar1->Draw();
  
  TArrow *ar2 = new TArrow(0.45,0.04,0.58,0.005,0.03,"|>");
  ar2->SetAngle(-60);
  ar2->SetLineWidth(2);
  ar2->SetFillColor(1);
  ar2->Draw();

  can->Update();
  can->SaveAs("/Users/arnaudsteen/SDHCAL/root_macros/plots/stepLength_vs_deltaZ_zoom.pdf");
}
