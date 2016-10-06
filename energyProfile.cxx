#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <cmath>
#include <limits>
#include <iomanip>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TLegend.h>
#include <TText.h>
#include <TLatex.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  if(argc<3){
    std::cout << "wrong number of argument" << std::endl;
    return 0;
  }

  std::string particle=argv[1];
  int energy=atoi(argv[2]);
  float rcut=5.0;
  if(argc==4)
    rcut=atof(argv[3]);
  
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  CaliceStyle();
    
  char fileName[200];  
  std::stringstream ss(std::stringstream::in | std::stringstream::out);
  ss << std::setprecision(3) ;

  sprintf(fileName,"%s%s%s%d%s","/data/root/ShowerProcessor/single_",particle.c_str(),"_",(int)energy,"GeV_0.root");
  TFile *file=new TFile(fileName,"READ");
  if( file->IsOpen() )
    file->Print();
  else
    std::cout << "can not open " << fileName << std::endl;
  TTree *tree = (TTree*)file->Get("tree");
  if (!tree){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return 0;
  }

  double Lmean=0.;
  double Lmeanerror=0.;
  double Lsigma=0.;
  double Lsigmaerror=0.;
  double Lnorm=0.;
  double Tmean=0.;
  double Tmeanerror=0.;
  double Tsigma=0.;
  double Tsigmaerror=0.;
  double Tnorm=0.;
  
  std::vector<double> *transverse;
  std::vector<double> *longitudinal;
  tree->SetBranchAddress("transverse",&transverse);
  tree->SetBranchAddress("longitudinal",&longitudinal);
  const unsigned nEvts = tree->GetEntries();
  TH2D* hT=new TH2D("hT","",30,0,30,10000,0,100);
  TH2D* hL=new TH2D("hL","",28,0,28,10000,0,100);
  TH1D* hEcore_vs_Etot=new TH1D("hEcore_vs_Etot","",100,0,1);
  hEcore_vs_Etot->SetLineWidth(2);
  hEcore_vs_Etot->SetLineStyle(1);
  hEcore_vs_Etot->SetLineColor(kBlack);
  hEcore_vs_Etot->GetXaxis()->SetTitle("Energy ratio");
  hEcore_vs_Etot->GetYaxis()->SetTitle("# events");
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
    tree->GetEntry(ievt);
    for(unsigned int iLayer=0; iLayer<longitudinal->size(); iLayer++){
      hL->Fill(iLayer,longitudinal->at(iLayer)*1000);
      Lmean+=iLayer*longitudinal->at(iLayer)*1000;
      Lsigma+=iLayer*iLayer*longitudinal->at(iLayer)*1000;
      Lnorm+=longitudinal->at(iLayer)*1000;
    }
    float etot=0;
    float ecor=0;
    for(unsigned int iRing=0; iRing<transverse->size(); iRing++){
      if( iRing<64 ){
	etot+=transverse->at(iRing)*1000;
	if( iRing<rcut )
	  ecor+=transverse->at(iRing)*1000;
	hT->Fill(iRing,transverse->at(iRing)*1000);
	Tmean+=iRing*transverse->at(iRing)*1000;
	Tsigma+=iRing*iRing*transverse->at(iRing)*1000;
	Tnorm+=transverse->at(iRing)*1000;
      }
    }
    hEcore_vs_Etot->Fill(ecor/etot);
    if( (ievt+1)%100==0 )
      std::cout << "Entry " << ievt+1 << std::endl;
  }

  Lmean/=Lnorm;
  Lsigma=std::sqrt( Lsigma/Lnorm - Lmean*Lmean );
  Lmeanerror=Lsigma/std::sqrt(Lnorm);
  Lsigmaerror=Lsigma/2/std::sqrt(Lnorm);
  Tmean/=Tnorm;
  Tsigma=std::sqrt( Tsigma/Tnorm - Tmean*Tmean );
  Tmeanerror=Tsigma/std::sqrt(Tnorm);
  Tsigmaerror=Tsigma/2/std::sqrt(Tnorm);

  TText *tex=new TText();
  tex->SetTextSize(0.04);
  tex->SetTextColor(kBlack);
  
  TCanvas *can=new TCanvas();
  can->SetWindowSize(1200,450);
  can->Divide(3,1);
  can->cd(1);
  TProfile *pL=hL->ProfileX();
  pL->SetMarkerStyle(20);
  pL->SetMarkerColor(kBlack);
  pL->SetMarkerSize(0.8);
  pL->GetXaxis()->SetTitle("HGCAL layer");
  pL->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pL->SetMinimum(0.0);
  pL->Draw("p");
  ss.str("");
  ss << "mean = " << Lmean << " +- " << Lmeanerror ;
  tex->DrawTextNDC(0.5,0.8,ss.str().c_str());
  ss.str("");
  ss << "rms = " << Lsigma << " +- " << Lsigmaerror ;
  tex->DrawTextNDC(0.5,0.75,ss.str().c_str());
  ss.str("");
  ss << energy << " GeV " << particle << " simulation";
  tex->DrawTextNDC(0.12,0.96,ss.str().c_str());
  
  can->cd(2);
  TProfile *pT=hT->ProfileX();
  pT->SetMarkerStyle(20);
  pT->SetMarkerColor(kBlack);
  pT->SetMarkerSize(0.8);
  pT->GetXaxis()->SetTitle("Distance to shower thrust [cm]");
  pT->GetYaxis()->SetTitle("Deposited energy [MeV]");
  pT->Draw("p");
  can->GetPad(2)->SetLogy();
  ss.str("");
  ss << "mean = " << Tmean << " +- " << Tmeanerror ;
  tex->DrawTextNDC(0.5,0.8,ss.str().c_str());
  ss.str("");
  ss << "rms = " << Tsigma << " +- " << Tsigmaerror ;
  tex->DrawTextNDC(0.5,0.75,ss.str().c_str());
  ss.str("");
  ss << energy << " GeV " << particle << " simulation";
  tex->DrawTextNDC(0.12,0.96,ss.str().c_str());

  can->cd(3);
  hEcore_vs_Etot->Draw("hist");
  ss.str("");
  ss << "mean = " << hEcore_vs_Etot->GetMean() << " +- " << hEcore_vs_Etot->GetMeanError() ;
  tex->DrawTextNDC(0.2,0.8,ss.str().c_str());
  ss.str("");
  ss << "rms = " << hEcore_vs_Etot->GetRMS() << " +- " << hEcore_vs_Etot->GetRMSError() ;
  tex->DrawTextNDC(0.2,0.75,ss.str().c_str());
  
  can->Update();
  can->WaitPrimitive();
  return 0;
}
