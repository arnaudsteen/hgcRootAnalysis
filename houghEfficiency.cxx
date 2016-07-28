#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TLegend.h>
#include <TText.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  CaliceStyle();
  int nP=11;
  float dCut=15.0;
  if(argc==2)
    dCut=atof(argv[1]);
  
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);
  TCanvas *can=new TCanvas();
  can->SetWindowSize(800,800);

  Double_t efficiency[nP];
  Double_t efficiency_error[nP];
  Double_t noiseRate[nP];
  Double_t noiseRate_error[nP];
  Double_t efficiencyMF[nP];
  Double_t efficiencyMF_error[nP];
  for(int i=0; i<nP; i++){
    efficiency[i]=0.;
    efficiency_error[i]=0.;
    efficiencyMF[i]=0.;
    efficiencyMF_error[i]=0.;
    noiseRate[i]=(float)i/10;
    noiseRate_error[i]=0.;
  }

  char fileName[200];
  for(unsigned int i(0); i<nP; i++){
    
    sprintf(fileName,"%s%d%s","/data/root/houghEfficiencyProcessor/single_mu-_20GeV_",i,".root");
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
      
      if(distanceToVertex<dCut)
	efficiency[i]++;
      
      if( (ievt+1)%1000==0 )
	std::cout << "Entry " << ievt+1 << std::endl;
    }
    efficiency[i]/=nEvts;
    efficiency_error[i]=std::sqrt(efficiency[i]*(1-efficiency[i])/nEvts);
    std::cout << (float)i/10 << "\t" << efficiency[i] << "\t" << efficiency_error[i] << std::endl;
    file.Close();
  }

  for(unsigned int i(0); i<nP; i++){
    int seed=i+20;
    sprintf(fileName,"%s%d%s","/data/root/houghEfficiencyProcessor/single_mu-_20GeV_",seed,".root");
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
      
      if(distanceToVertex<dCut)
	efficiencyMF[i]++;
      
      if( (ievt+1)%1000==0 )
	std::cout << "Entry " << ievt+1 << std::endl;
    }
    efficiencyMF[i]/=nEvts;
    efficiencyMF_error[i]=std::sqrt(efficiencyMF[i]*(1-efficiencyMF[i])/nEvts);
    std::cout << (float)i/10 << "\t" << efficiencyMF[i] << "\t" << efficiencyMF_error[i] << std::endl;
    file.Close();
  }

  TH1D h("h","",100,-.01,1.1);
  h.SetMinimum(0.3);
  h.SetMaximum(1.01);
  h.GetXaxis()->SetTitle("Noise probability [%]");
  h.GetXaxis()->SetTitleOffset(0.9);
  h.GetYaxis()->SetTitle("Efficiency");
  h.GetYaxis()->SetTitleOffset(1.1);

  TGraphErrors* gr=new TGraphErrors(nP,noiseRate,efficiency,noiseRate_error,efficiency_error);
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(0.8);
  gr->SetMarkerColor(kBlack);

  TGraphErrors* grMF=new TGraphErrors(nP,noiseRate,efficiencyMF,noiseRate_error,efficiencyMF_error);
  grMF->SetMarkerStyle(21);
  grMF->SetMarkerSize(0.8);
  grMF->SetMarkerColor(kRed);

  h.Draw("axis");
  gr->Draw("p");
  grMF->Draw("p");
  
  TLegend *leg=new TLegend(0.12,0.47,0.51,0.64);
  leg->SetFillStyle(0);
  leg->AddEntry(gr,"No magnetic field","p");
  leg->AddEntry(grMF,"Magnetic field (0,0,3.8) T","p");
  leg->Draw();

  TText* tex=new TText();
  tex->SetTextSize(0.035);
  tex->SetTextColor(kBlack);
  tex->DrawTextNDC(0.15,0.96,"20 GeV single muons simulation");
  tex->Draw();
  
  can->Update();
  
  can->SaveAs("plots/houghEfficiency_vs_noise.pdf");


  delete gr;
  delete can;
  return 0;
}
