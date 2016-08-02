#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TLegend.h>
#include <TText.h>
#include <TLatex.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  std::ostringstream os( std::ostringstream::ate );

  TH2D* h0=new TH2D("h0","",40,0,40,20,0,20);
  TH2D* h1=new TH2D("h1","",40,0,40,20,0,20);
  
  int npart=atoi(argv[1]);
  float dCut=15;
  if(argc>2)
    dCut=atof(argv[2]);
  int seed=0;
  if(argc>3)
    seed=atoi(argv[3]);
  os.str("");
  os << "muonPlusJet" << npart << "_" << seed << ".root";
  TFile file(os.str().c_str(),"READ");
  if( file.IsOpen() )
    file.Print();
  else
    std::cout << "can not open " << os.str() << std::endl;

  if( !file.GetListOfKeys()->Contains("tree") ){
    std::cout << " -- Error, no TTree called tree in" << os.str() << std::endl;
    return 0;
  }
  TTree *tree = (TTree*)file.Get("tree");
  float distanceToProjection;
  int ntrack;
  std::vector<int> *nhitInCylinder;
  tree->SetBranchAddress("distanceToProjection",&distanceToProjection);
  tree->SetBranchAddress("ntrack",&ntrack);
  tree->SetBranchAddress("nhitInCylinder",&nhitInCylinder);
  const unsigned nEvts = tree->GetEntries();
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
    tree->GetEntry(ievt);
    if(ntrack==0) continue;
    for(int ilayer=0; ilayer<40; ilayer++){
      if( distanceToProjection<dCut ) h0->Fill(ilayer,nhitInCylinder->at(ilayer));
      else h1->Fill(ilayer,nhitInCylinder->at(ilayer));
    }
    if( (ievt+1)%1000==0 )
      std::cout << "Entry " << ievt+1 << std::endl;
  }

  // if( h1->GetEntries()nEvts/100.){
    
  // }
  
  TProfile *p0=h0->ProfileX();
  p0->SetMarkerStyle(20);
  p0->SetMarkerColor(kRed);
  p0->SetLineColor(kRed);
  p0->GetXaxis()->SetTitle("Layer number");
  p0->GetYaxis()->SetTitle("<N_{hit}> #in 2cm cylinder");
  
  TProfile *p1=h1->ProfileX();
  p1->SetMarkerStyle(21);
  p1->SetMarkerColor(kBlack);
  p1->SetLineColor(kBlack);
  p1->GetXaxis()->SetTitle("Layer number");
  p1->GetYaxis()->SetTitle("<N_{hit}> #in 2cm cylinder");

  CaliceStyle();
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);
  TCanvas *can=new TCanvas();
  can->SetWindowSize(600,600);

  p1->Draw("p");
  p0->Draw("psame");

  TLegend *leg=new TLegend(0.5,0.7,1.0,0.9);
  leg->SetFillStyle(0);
  leg->AddEntry(p0,"Reconstructed muons","p");
  leg->AddEntry(p1,"Non reconstructed muons","p");
  leg->Draw();
  can->Update();

  can->WaitPrimitive();
  can->SaveAs("plots/muonPlusJetHitCylinder.pdf");
  can->SaveAs("plots/muonPlusJetHitCylinder.C");
  delete can;
  return 0;
}
