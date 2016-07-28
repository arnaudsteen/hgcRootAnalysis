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
#define PI 3.14159265
int main(int argc,char** argv)
{
  CaliceStyle();

  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);
  TCanvas *canC=new TCanvas();
  canC->SetWindowSize(800,800);
  TCanvas *canT=new TCanvas();
  canT->SetWindowSize(800,800);
  TCanvas *canE=new TCanvas();
  canE->SetWindowSize(800,800);

  std::string particle="mu-";
  if(argc>=2)
    particle=argv[2];
  int energy=20;
  if(argc>=3)
    energy=atoi(argv[3]);
  int seed=0;
  if(argc>=4)
    seed=atoi(argv[4]);
  

  char fileName[100];
  sprintf(fileName,"%s%s%s%d%s%d%s","single_",particle.c_str(),"_",energy,"GeV_",seed,".root");
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

  TH1D* hC=new TH1D("hC","",200,0.6,1);
  hC->GetXaxis()->SetTitle("cos #{theta}");
  hC->GetYaxis()->SetTitle("# events");
  hC->SetLineColor(kBlack);
  hC->SetLineWidth(2);
  TH1D* hT=new TH1D("hT","",200,0,40);
  hT->GetXaxis()->SetTitle("#{theta}");
  hT->GetYaxis()->SetTitle("# events");
  hT->SetLineColor(kBlack);
  hT->SetLineWidth(2);
  TH1D* hE=new TH1D("hE","",200,0,4);
  hE->GetXaxis()->SetTitle("#{eta}");
  hE->GetYaxis()->SetTitle("# events");
  hE->SetLineColor(kBlack);
  hE->SetLineWidth(2);
  
  double cosTheta;
  tree->SetBranchAddress("cosTheta",&cosTheta);
  const unsigned nEvts = tree->GetEntries();
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
    tree->GetEntry(ievt);

    hC->Fill(cosTheta);
    hT->Fill( std::acos(cosTheta)*180/PI );
    hE->Fill( -std::log(std::tan(std::acos(cosTheta))) );
    
    if( (ievt+1)%1000==0 )
      std::cout << "Entry " << ievt+1 << std::endl;
  }
  canC->cd();
  hC->Draw("");
  canC->Update();
  
  canT->cd();
  hT->Draw("");
  canT->Update();

  canE->cd();
  hE->Draw("");
  canE->Update();

  canC->WaitPrimitive();
  canC->SaveAs("plots/cosTheta.pdf");
  canT->SaveAs("plots/theta.pdf");
  canE->SaveAs("plots/eta.pdf");
  file.Close();
}
