#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <unistd.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TLegend.h>
#include <TText.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  std::ostringstream os( std::ostringstream::ate );

  float dCut=10.0;
  if(argc>=2)
    dCut=atof(argv[1]);

  int npartmax=10;
  if(argc>=3)
    npartmax=atoi(argv[2]);
  
  float energy=20;
  if(argc>=4)
    energy=atof(argv[3]);

  Double_t efficiency[npartmax+1];
  Double_t efficiency_error[npartmax+1];
  Double_t nparticle[npartmax+1];
  Double_t nparticle_error[npartmax+1];
  
  for(int i=0; i<npartmax+1; i++){
    os.str("");
    os << energy << "GeVMuon-" << i << "Particles.root";

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
    float simEta;
    int nhit;
    int nevent=0;
    efficiency[i]=0.0;
    efficiency_error[i]=0.0;
    nparticle[i]=i;
    nparticle_error[i]=0.0;
    tree->SetBranchAddress("distanceToProjection",&distanceToProjection);
    tree->SetBranchAddress("ntrack",&ntrack);
    tree->SetBranchAddress("nhit",&nhit);
    tree->SetBranchAddress("simEta",&simEta);
    const unsigned nEvts = tree->GetEntries();
    for( unsigned ievt(0); ievt<nEvts; ++ievt ){
      tree->GetEntry(ievt);
      if( nhit>10 && simEta>1.71 )
	nevent++;
      if(ntrack>0&&distanceToProjection<dCut)
	efficiency[i]++;
      if( (ievt+1)%1000==0 )
	std::cout << "Entry " << ievt+1 << std::endl;
    }
    efficiency[i]/=nevent;
    efficiency_error[i]=std::sqrt( efficiency[i]*(1-efficiency[i])/nevent );
  }

  TGraphErrors* graphE=new TGraphErrors(npartmax+1,nparticle,efficiency,nparticle_error,efficiency_error);
  graphE->SetMarkerStyle(20);
  graphE->SetMarkerColor(kBlack);
  graphE->SetLineWidth(2);
  graphE->SetLineColor(kBlack);

  TH1D* hE=new TH1D("hE","",100,-0.5,npartmax+0.5);
  hE->GetXaxis()->SetTitle("# particles");
  hE->GetYaxis()->SetTitle("Muon reconstruction efficiency");
  hE->GetYaxis()->SetTitleOffset(1.4);
  hE->SetMinimum(0.5);
  hE->SetMaximum(1.1);

  
  CaliceStyle();
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  TCanvas *can=new TCanvas();
  can->SetWindowSize(600,600);
  hE->Draw("axis");
  graphE->Draw("psame");

  can->Update();
  can->SaveAs("plots/muonPlusJetEfficiency.pdf");
  can->SaveAs("plots/muonPlusJetEfficiency.C");
  
  sleep(10);
  delete can;
  return 0;
}
