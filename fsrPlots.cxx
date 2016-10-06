#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <cmath>
#include <limits>
#include <iomanip>
#include <vector>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TLegend.h>
#include <TText.h>
#include <TLatex.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  if(argc<2){
    std::cout << "wrong number of argument" << std::endl;
    return 0;
  }

  int energy=atoi(argv[1]);
  float dCut=10.0;
  if(argc>=3)
    dCut=atof(argv[2]);
  
  CaliceStyle();
    
  int ntrack;
  int nlayer;
  float edep;
  float showerMax;
  std::vector<double> *transverse;
  std::vector<double> *longitudinal;
  float distanceToProjection;

  std::ostringstream input(std::ostringstream::ate);
  input.str("");

  TH2D* hLGamma=new TH2D("hLGamma","",40,0,40,100000,0,100);
  TH2D* hLFSR=new TH2D("hLFSR","",40,0,40,100000,0,100);
  TH2D* hLFSRC=new TH2D("hLFSRC","",40,0,40,100000,0,100);
    
  TH2D* hTGamma=new TH2D("hTGamma","",15,0,15,100000,0,100);
  TH2D* hTFSR=new TH2D("hTFSR","",15,0,15,100000,0,100);  
  TH2D* hTFSRC=new TH2D("hTFSRC","",15,0,15,100000,0,100);
  
  TH1D* hEGamma=new TH1D("hEGamma","",60,0,60);
  TH1D* hEFSR=new TH1D("hEFSR","",60,0,60);
  TH1D* hEFSRC=new TH1D("hEFSRC","",60,0,60);

  TH1D* hMGamma=new TH1D("hMGamma","",25,0,25);
  TH1D* hMFSR=new TH1D("hMFSR","",25,0,25);
  TH1D* hMFSRC=new TH1D("hMFSRC","",25,0,25);

  std::ostringstream text( std::ostringstream::ate );
  TText *tex=new TText();
  tex->SetTextColor(kBlack);
  tex->SetTextSize(0.04);
  text.str("");
  text << "Photon energy = " << energy << " GeV" ;
  
  input << "../HGCALMarlinProcessor/single_gamma_1GeV_10000Events_Seed0.root";
  TFile* fileGamma=new TFile(input.str().c_str(),"READ");
  if( fileGamma->IsOpen() )
    fileGamma->Print();
  else
    std::cout << "can not open " << input.str() << std::endl;
  TTree *treeGamma = (TTree*)fileGamma->Get("tree");
  if (!treeGamma){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return 0;
  }
  treeGamma->SetBranchAddress("ntrack",&ntrack);
  treeGamma->SetBranchAddress("edep",&edep);
  treeGamma->SetBranchAddress("showerMax",&showerMax);
  treeGamma->SetBranchAddress("transverse", &transverse);
  treeGamma->SetBranchAddress("longitudinal", &longitudinal);
  treeGamma->SetBranchAddress("distanceToProjection",&distanceToProjection);
  int nEvts = treeGamma->GetEntries();
  for( int ievt(0); ievt<nEvts; ++ievt ){
    treeGamma->GetEntry(ievt);
    if(ntrack>0) continue;
    hEGamma->Fill(edep);
    hMGamma->Fill(showerMax);
    for(unsigned int iRing=0; iRing<transverse->size(); iRing++)
      hTGamma->Fill(iRing,transverse->at(iRing)*1000);
    for(unsigned int iLayer=0; iLayer<longitudinal->size(); iLayer++)
      hLGamma->Fill(iLayer,longitudinal->at(iLayer)*1000);
  }
  input.str("");

  input << "../HGCALMarlinProcessor/" << energy << "GeVMuon-FSR-10000Events-Seed0.root";
  TFile* fileFSRC=new TFile(input.str().c_str(),"READ");
  if( fileFSRC->IsOpen() )
    fileFSRC->Print();
  else
    std::cout << "can not open " << input.str() << std::endl;
  TTree *treeFSRC = (TTree*)fileFSRC->Get("tree");
  if (!treeFSRC){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return 0;
  }
  treeFSRC->SetBranchAddress("ntrack",&ntrack);
  treeFSRC->SetBranchAddress("edep",&edep);
  treeFSRC->SetBranchAddress("showerMax",&showerMax);
  treeFSRC->SetBranchAddress("transverse", &transverse);
  treeFSRC->SetBranchAddress("longitudinal", &longitudinal);
  treeFSRC->SetBranchAddress("distanceToProjection",&distanceToProjection);
  nEvts = treeFSRC->GetEntries();
  for( int ievt(0); ievt<nEvts; ++ievt ){
    treeFSRC->GetEntry(ievt);
    if( ntrack==0 || distanceToProjection>dCut ) continue;
    hEFSRC->Fill(edep-9.01852000000000070e-02*nlayer);
    hMFSRC->Fill(showerMax);
    for(unsigned int iRing=0; iRing<transverse->size(); iRing++){
      if( iRing==0 )
	if( transverse->at(iRing)*1000-9.01852000000000070e-02>0 )
	  hTFSRC->Fill(iRing,transverse->at(iRing)*1000-9.01852000000000070e-02*nlayer);
	else
	  hTFSRC->Fill(iRing,0.0);
      else
	hTFSRC->Fill(iRing,transverse->at(iRing)*1000);
    }
    for(unsigned int iLayer=0; iLayer<longitudinal->size(); iLayer++)
      if( longitudinal->at(iLayer)*1000-9.01852000000000070e-02>0 )
	hLFSRC->Fill(iLayer,longitudinal->at(iLayer)*1000-9.01852000000000070e-02);
      else
	hLFSRC->Fill(iLayer,0.0);
  }
  input.str("");
  
  input << "../HGCALMarlinProcessor/" << energy << "GeVMuon-FSR-10000Events-Seed0NC.root";
  TFile* fileFSR=new TFile(input.str().c_str(),"READ");
  if( fileFSR->IsOpen() )
    fileFSR->Print();
  else
    std::cout << "can not open " << input.str() << std::endl;
  TTree *treeFSR = (TTree*)fileFSR->Get("tree");
  if (!treeFSR){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return 0;
  }
  treeFSR->SetBranchAddress("ntrack",&ntrack);
  treeFSR->SetBranchAddress("edep",&edep);
  treeFSR->SetBranchAddress("showerMax",&showerMax);
  treeFSR->SetBranchAddress("transverse", &transverse);
  treeFSR->SetBranchAddress("longitudinal", &longitudinal);
  treeFSR->SetBranchAddress("distanceToProjection",&distanceToProjection);
  nEvts = treeFSR->GetEntries();
  for( int ievt(0); ievt<nEvts; ++ievt ){
    treeFSR->GetEntry(ievt);
    if( ntrack==0 || distanceToProjection>dCut ) continue;
    hEFSR->Fill(edep-9.01852000000000070e-02*nlayer);
    hMFSR->Fill(showerMax);
    for(unsigned int iRing=0; iRing<transverse->size(); iRing++){
      if( iRing==0 )
	if( transverse->at(iRing)*1000-9.01852000000000070e-02>0 )
	  hTFSR->Fill(iRing,transverse->at(iRing)*1000-9.01852000000000070e-02*nlayer);
	else
	  hTFSR->Fill(iRing,0.0);
      else
	hTFSR->Fill(iRing,transverse->at(iRing)*1000);
    }
    for(unsigned int iLayer=0; iLayer<longitudinal->size(); iLayer++)
      if( longitudinal->at(iLayer)*1000-9.01852000000000070e-02>0 )
	hLFSR->Fill(iLayer,longitudinal->at(iLayer)*1000-9.01852000000000070e-02);
      else
	hLFSR->Fill(iLayer,0.0);
  }
  input.str("");

  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  TCanvas *can=new TCanvas();
  can->SetWindowSize(1200,600);
  can->Divide(2,1);

  can->cd(1);
  TProfile *pLFSRC=hLFSRC->ProfileX();
  pLFSRC->SetMarkerStyle(20);
  pLFSRC->SetMarkerColor(kBlack);
  pLFSRC->SetMarkerSize(0.8);
  pLFSRC->GetXaxis()->SetTitle("HGCAL layer");
  pLFSRC->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pLFSRC->SetMinimum(0.0);

  TProfile *pLFSR=hLFSR->ProfileX();
  pLFSR->SetMarkerStyle(21);
  pLFSR->SetMarkerColor(kBlue-6);
  pLFSR->SetMarkerSize(0.8);
  pLFSR->GetXaxis()->SetTitle("HGCAL layer");
  pLFSR->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pLFSR->SetMinimum(0.0);

  TProfile *pLGamma=hLGamma->ProfileX();
  pLGamma->GetXaxis()->SetTitle("HGCAL layer");
  pLGamma->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pLGamma->SetFillStyle(1001);
  pLGamma->SetFillColor(kRed-6);

  
  TProfile *pL=pLFSR;
  if( pL->GetBinContent(pL->GetMaximumBin()) < pLGamma->GetBinContent(pLGamma->GetMaximumBin()) )
    pL = pLGamma;
  if( pL->GetBinContent(pL->GetMaximumBin()) < pLFSRC->GetBinContent(pLFSRC->GetMaximumBin()) )
    pL = pLFSRC;
  
  pL->Draw("axis");
  pLGamma->Draw("histsame");
  pLFSRC->Draw("psame");
  pLFSR->Draw("psame");
  pL->Draw("axissame");

  TLegend* legL=new TLegend(0.5,0.65,0.9,0.8);
  legL->SetHeader("Longitudinal profile");
  legL->SetFillStyle(0);
  legL->AddEntry(pLGamma,"Single photon","f");
  legL->AddEntry(pLFSR,"FSR","p");
  legL->AddEntry(pLFSRC,"FSR + photon clustering","p");
  legL->Draw();

  tex->DrawTextNDC(0.13,0.96,text.str().c_str());
  
  can->cd(2);
  //can->GetPad(2)->SetLogy();
  TProfile *pTFSRC=hTFSRC->ProfileX();
  pTFSRC->SetMarkerStyle(20);
  pTFSRC->SetMarkerColor(kBlack);
  pTFSRC->SetMarkerSize(0.8);
  pTFSRC->GetXaxis()->SetTitle("Distance to shower axis [cm]");
  pTFSRC->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pTFSRC->SetMinimum(0.0);
  
  TProfile *pTFSR=hTFSR->ProfileX();
  pTFSR->SetMarkerStyle(21);
  pTFSR->SetMarkerColor(kBlue-6);
  pTFSR->SetMarkerSize(0.8);
  pTFSR->GetXaxis()->SetTitle("Distance to shower axis [cm]");
  pTFSR->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pTFSR->SetMinimum(0.0);

  TProfile *pTGamma=hTGamma->ProfileX();
  pTGamma->GetXaxis()->SetTitle("Distance to shower axis [cm]");
  pTGamma->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pTGamma->SetFillStyle(1001);
  pTGamma->SetFillColor(kRed-6);

  TProfile *pT=pTFSR;
  if( pT->GetBinContent(pT->GetMaximumBin()) < pTGamma->GetBinContent(pTGamma->GetMaximumBin()) )
    pT = pTGamma;
  if( pT->GetBinContent(pT->GetMaximumBin()) < pTFSRC->GetBinContent(pTFSRC->GetMaximumBin()) )
    pT = pTFSRC;
  
  pT->Draw("axis");
  pTGamma->Draw("histsame");
  pTFSRC->Draw("psame");
  pTFSR->Draw("psame");
  pT->Draw("axissame");
  
  TLegend* legT=new TLegend(0.6,0.65,0.9,0.8);
  legT->SetHeader("Lateral profile");
  legT->SetFillStyle(0);
  legT->AddEntry(pTGamma,"Single photon","f");
  legT->AddEntry(pTFSR,"FSR","p");
  legT->AddEntry(pTFSRC,"FSR + photon clustering","p");
  legT->Draw();

  tex->DrawTextNDC(0.13,0.96,text.str().c_str());
  
  can->Update();
  can->WaitPrimitive();

  TCanvas *can1=new TCanvas();
  can1->SetWindowSize(1200,600);
  can1->Divide(2,1);
  can1->cd(1);
  
  hEFSRC->SetMarkerStyle(20);
  hEFSRC->SetMarkerColor(kBlack);
  hEFSRC->SetMarkerSize(0.8);
  hEFSRC->GetXaxis()->SetTitle("Total deposited energy [MeV]");
  hEFSRC->GetYaxis()->SetTitle("# events");
  hEFSRC->SetMinimum(0.0);
  
  hEFSR->SetMarkerStyle(21);
  hEFSR->SetMarkerColor(kBlue-6);
  hEFSR->SetMarkerSize(0.8);
  hEFSR->GetXaxis()->SetTitle("Total deposited energy [MeV]");
  hEFSR->GetYaxis()->SetTitle("# events");
  hEFSR->SetMinimum(0.0);

  hEGamma->GetXaxis()->SetTitle("Total deposited energy [MeV]");
  hEGamma->GetYaxis()->SetTitle("# events");
  hEGamma->SetFillStyle(1001);
  hEGamma->SetFillColor(kRed-6);

  TH1D *hE=hEFSR;
  if( hE->GetBinContent(hE->GetMaximumBin()) < hEGamma->GetBinContent(hEGamma->GetMaximumBin()) )
    hE = hEGamma;
  if( hE->GetBinContent(hE->GetMaximumBin()) < hEFSRC->GetBinContent(hEFSRC->GetMaximumBin()) )
    hE = hEFSRC;
  
  hE->Draw("axis");
  hEGamma->Draw("histsame");
  hEFSRC->Draw("psame");
  hEFSR->Draw("psame");
  hE->Draw("axissame");
  
  TLegend* legE=new TLegend(0.6,0.65,0.9,0.8);
  legE->SetFillStyle(0);
  legE->AddEntry(hEGamma,"Single photon","f");
  legE->AddEntry(hEFSR,"FSR","p");
  legE->AddEntry(hEFSRC,"FSR + photon clustering","p");
  legE->Draw();

  tex->DrawTextNDC(0.13,0.96,text.str().c_str());


  can1->cd(2);
  hMFSRC->SetMarkerStyle(20);
  hMFSRC->SetMarkerColor(kBlack);
  hMFSRC->SetMarkerSize(0.8);
  hMFSRC->GetXaxis()->SetTitle("Shower maximum [layer unit]");
  hMFSRC->GetYaxis()->SetTitle("# events");
  hMFSRC->SetMinimum(0.0);

  hMFSR->SetMarkerStyle(21);
  hMFSR->SetMarkerColor(kBlue-6);
  hMFSR->SetMarkerSize(0.8);
  hMFSR->GetXaxis()->SetTitle("Shower maximum [layer unit]");
  hMFSR->GetYaxis()->SetTitle("# events");
  hMFSR->SetMinimum(0.0);

  hMGamma->GetXaxis()->SetTitle("Shower maximum [layer unit]");
  hMGamma->GetYaxis()->SetTitle("# events");
  hMGamma->SetFillStyle(1001);
  hMGamma->SetFillColor(kRed-6);

  TH1D *hM=hMFSR;
  if( hM->GetBinContent(hM->GetMaximumBin()) < hMGamma->GetBinContent(hMGamma->GetMaximumBin()) )
    hM = hMGamma;
  if( hM->GetBinContent(hM->GetMaximumBin()) < hMFSRC->GetBinContent(hMFSRC->GetMaximumBin()) )
    hM = hMFSRC;
  
  hM->Draw("axis");
  hMGamma->Draw("histsame");
  hMFSRC->Draw("psame");
  hMFSR->Draw("psame");
  hM->Draw("axissame");
  
  TLegend* legM=new TLegend(0.6,0.65,0.9,0.8);
  legM->SetFillStyle(0);
  legM->AddEntry(hMGamma,"Single photon","f");
  legM->AddEntry(hMFSR,"FSR","p");
  legM->AddEntry(hMFSRC,"FSR + photon clustering","p");
  legM->Draw();

  tex->DrawTextNDC(0.13,0.96,text.str().c_str());

  can1->Update();
  can1->WaitPrimitive();
  
  delete pLGamma;
  delete pLFSR;
  delete pLFSRC;
  delete hLGamma;
  delete hLFSR;
  delete hLFSRC;
  delete legL;
  delete pTGamma;
  delete pTFSR;
  delete pTFSRC;
  delete hTGamma;
  delete hTFSR;
  delete hTFSRC;
  delete hEGamma;
  delete hEFSR;
  delete hEFSRC;
  delete hMGamma;
  delete hMFSR;
  delete hMFSRC;
  delete legT;
  delete tex;

  delete can;
  delete app;
  return 0;
}
