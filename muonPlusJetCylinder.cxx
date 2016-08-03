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
  float emip=8.58102e-05; //from simulation landau fit
  
  std::ostringstream os( std::ostringstream::ate );

  TH2D* h0=new TH2D("h0","",40,0,40,20,-0.5,19.5);
  TH2D* h1=new TH2D("h1","",40,0,40,20,-0.5,19.5);
  TH2D* hE0=new TH2D("hE0","",40,0,40,1000,0.0,100.0);
  TH2D* hE1=new TH2D("hE1","",40,0,40,1000,0.0,100.0);
  
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
  std::vector<double> *energyInCylinder;
  tree->SetBranchAddress("distanceToProjection",&distanceToProjection);
  tree->SetBranchAddress("ntrack",&ntrack);
  tree->SetBranchAddress("nhitInCylinder",&nhitInCylinder);
  tree->SetBranchAddress("energyInCylinder",&energyInCylinder);
  const unsigned nEvts = tree->GetEntries();
  float meanE1[40];
  int nE1[40];
  float meanE0[40];
  int nE0[40];
  for(int ilayer=0; ilayer<40; ilayer++){
    nE1[ilayer]=0;
    meanE1[ilayer]=0.0;
    nE0[ilayer]=0;
    meanE0[ilayer]=0.0;
  }
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
    tree->GetEntry(ievt);
    if(ntrack==0) continue;
    for(int ilayer=0; ilayer<40; ilayer++){
      if( distanceToProjection<dCut ) {
	h0->Fill(ilayer,nhitInCylinder->at(ilayer));
	hE0->Fill(ilayer,energyInCylinder->at(ilayer)/emip);
 	nE0[ilayer]++;
	meanE0[ilayer]+=energyInCylinder->at(ilayer)/emip;
      }	
      else {
	h1->Fill(ilayer,nhitInCylinder->at(ilayer));
	hE1->Fill(ilayer,energyInCylinder->at(ilayer)/emip);
 	nE1[ilayer]++;
	meanE1[ilayer]+=energyInCylinder->at(ilayer)/emip;
      }
    }
    if( (ievt+1)%1000==0 )
      std::cout << "Entry " << ievt+1 << std::endl;
  }

  for(int ilayer=0; ilayer<40; ilayer++)
    std::cout << ilayer << "\t" << meanE1[ilayer]/nE1[ilayer] << "\t" << meanE0[ilayer]/nE0[ilayer] << std::endl;
  
  // if( h1->GetEntries()nEvts/100.){
    
  // }

  TProfile *p0=NULL;
  TProfile *pE0=NULL;
  TProfile *p1=NULL;
  TProfile *pE1=NULL;
  
  if( h0->GetEntries()>50 ){
    p0=h0->ProfileX();
    p0->SetMarkerStyle(20);
    p0->SetMarkerColor(kRed);
    p0->SetLineColor(kRed);
    p0->GetXaxis()->SetTitle("Layer number");
    p0->GetYaxis()->SetTitle("<N_{hit}> #in 2cm cylinder");
  
    pE0=hE0->ProfileX();
    pE0->SetMarkerStyle(20);
    pE0->SetMarkerColor(kRed);
    pE0->SetLineColor(kRed);
    pE0->GetXaxis()->SetTitle("Layer number");
    pE0->GetYaxis()->SetTitle("<E> [mip] #in 2cm cylinder");
  }  
  if( h1->GetEntries()>50 ){
    p1=h1->ProfileX();
    p1->SetMarkerStyle(21);
    p1->SetMarkerColor(kBlack);
    p1->SetLineColor(kBlack);
    p1->GetXaxis()->SetTitle("Layer number");
    p1->GetYaxis()->SetTitle("<N_{hit}> #in 2cm cylinder");

    pE1=hE1->ProfileX();
    pE1->SetMarkerStyle(21);
    pE1->SetMarkerColor(kBlack);
    pE1->SetLineColor(kBlack);
    pE1->GetXaxis()->SetTitle("Layer number");
    pE1->GetYaxis()->SetTitle("<E> [mip] #in 2cm cylinder");
  }
  CaliceStyle();
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  TCanvas *can=new TCanvas();
  can->SetWindowSize(600,600);
  TCanvas *canE=new TCanvas();
  canE->SetWindowSize(600,600);
  TLegend *leg=new TLegend(0.5,0.7,1.0,0.9);
  leg->SetFillStyle(0);
  TLegend *legE=new TLegend(0.5,0.7,1.0,0.9);
  legE->SetFillStyle(0);

  if( h1->GetEntries()>50 ){
    can->cd();
    p1->Draw("p");
    leg->AddEntry(p1,"Non reconstructed muons","p");
    canE->cd();
    pE1->Draw("p");
    legE->AddEntry(pE1,"Non reconstructed muons","p");
    if( h0->GetEntries()>50 ){
      can->cd();
      p0->Draw("psame");
      leg->AddEntry(p0,"Reconstructed muons","p");
      leg->Draw();
      canE->cd();
      pE0->Draw("psame");
      legE->AddEntry(pE0,"Reconstructed muons","p");
      legE->Draw();
    }
  }
  else{
    can->cd();
    p0->Draw("p");
    leg->AddEntry(p0,"Reconstructed muons","p");
    leg->Draw();
    canE->cd();
    pE0->Draw("p");
    legE->AddEntry(pE0,"Reconstructed muons","p");
    legE->Draw();
  }
  
  can->Update();
  canE->Update();

  can->WaitPrimitive();
  os.str("");
  os << "plots/muonPlusJetHitCylinder" << npart << "_" << seed << ".pdf";
  can->SaveAs(os.str().c_str());
  os.str("");
  os << "plots/muonPlusJetHitCylinder" << npart << "_" << seed << ".C";
  can->SaveAs(os.str().c_str());
  delete can;
  os.str("");
  os << "plots/muonPlusJetEnergyCylinder" << npart << "_" << seed << ".pdf";
  canE->SaveAs(os.str().c_str());
  os.str("");
  os << "plots/muonPlusJetEnergyCylinder" << npart << "_" << seed << ".C";
  canE->SaveAs(os.str().c_str());
  delete canE;
  return 0;
}
