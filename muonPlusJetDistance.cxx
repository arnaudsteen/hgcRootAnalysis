#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <TLegend.h>
#include <TText.h>

#include "CaliceStyle.h"

int main(int argc,char** argv)
{
  std::ostringstream os( std::ostringstream::ate );

  TH1D* h[argc-1];
  TGraph* graph[argc-1];
  int color=1;
  for(int i=0; i<argc-1; i++){
    float npart=atoi(argv[i+1]);
    os.str("");
    os << "h" << npart;
    h[i]=new TH1D(os.str().c_str(),"",2000,0,1000);
    h[i]->SetLineColor(color);
    h[i]->SetLineWidth(2);
    h[i]->GetXaxis()->SetRangeUser(0,20);
    color++;
  }
  
  for(int i=0; i<argc-1; i++){
    float npart=atoi(argv[i+1]);
    os.str("");
    os << "muonPlusJet" << npart << "_0.root";
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
    tree->SetBranchAddress("distanceToProjection",&distanceToProjection);
    tree->SetBranchAddress("ntrack",&ntrack);
    const unsigned nEvts = tree->GetEntries();
    for( unsigned ievt(0); ievt<nEvts; ++ievt ){
      tree->GetEntry(ievt);
      if(ntrack>0)
	h[i]->Fill(distanceToProjection);      
      if( (ievt+1)%1000==0 )
	std::cout << "Entry " << ievt+1 << std::endl;
    }
  }
  int nbin=100;
  int style=20;
  color=1;
  Double_t x[argc-1][nbin];
  Double_t y[argc-1][nbin];
  for(int i=0; i<argc-1; i++){
    for(int j=0; j<nbin; j++)
      x[i][j]=(Double_t)(j+1)/nbin;
    h[i]->GetQuantiles(nbin,y[i],x[i]);
    graph[i]=new TGraph(nbin,y[i],x[i]);
    graph[i]->SetMarkerColor(color);
    graph[i]->SetMarkerStyle(style);
    graph[i]->SetMarkerSize(0.8);
    graph[i]->GetXaxis()->SetTitle("Distance to projection [mm]");
    graph[i]->GetYaxis()->SetTitle("Fraction of events");
    graph[i]->GetXaxis()->SetRangeUser(0,20);
    graph[i]->GetYaxis()->SetTitleOffset(1.5);
    color++;
    style++;
  }
  
  TH1D* hh=h[0];
  for(int i=0; i<argc-1; i++){
    h[i]->Sumw2();
    h[i]->Scale(1.0/h[i]->Integral());
    if( h[i]->GetBinContent(h[i]->GetMaximumBin()) > hh->GetBinContent(hh->GetMaximumBin()) )
      hh=h[i];
  }
  hh->GetXaxis()->SetTitle("Distance to projection [mm]");
  hh->GetYaxis()->SetTitle("# events");
  hh->GetYaxis()->SetTitleOffset(1.5);

  CaliceStyle();
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);
  TCanvas *can=new TCanvas();
  can->SetWindowSize(1200,600);
  can->Divide(2,1);

  can->cd(1);
  hh->Draw("axis");
  for(int i=0; i<argc-1; i++){
    h[i]->GetXaxis()->SetRangeUser(0,20);
    h[i]->Draw("histsame");
  }

  TLegend *leg=new TLegend(0.6,0.6,0.85,0.85);
  leg->SetFillStyle(0);
  for(int i=0; i<argc-1; i++){
    float npart=atoi(argv[i+1]);
    os.str("");
    os << npart << " particles";
    leg->AddEntry(h[i],os.str().c_str(),"l");
  }
  leg->Draw();

  can->cd(2);
  graph[0]->Draw("ap");
  for(int i=1; i<argc-1; i++)
    graph[i]->Draw("psame");
  TLegend *legQ=new TLegend(0.6,0.4,0.85,0.65);
  legQ->SetFillStyle(0);
  for(int i=0; i<argc-1; i++){
    float npart=atoi(argv[i+1]);
    os.str("");
    os << npart << " particles";
    legQ->AddEntry(graph[i],os.str().c_str(),"p");
  }
  legQ->Draw();
  
  can->cd();
  can->Update();
  can->WaitPrimitive();
  
  
  delete can;
  return 0;
  }
