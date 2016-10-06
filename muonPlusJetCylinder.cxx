#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include <unistd.h>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TH1D.h>
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
  if(argc<4){
    std::cout << "Error : 4 arguments are expected instead of the " << argc << "given -> return" << std::endl; 
    return 0;
  }
  float energy=atof(argv[1]);
  float dCut=atof(argv[2]);
  
  TH2D* h0[argc-3];
  TH2D* h1[argc-3];
  int nevent1[argc-3];
  int nevent0[argc-3];
  TProfile* p0[argc-3];
  TProfile* p1[argc-3];
  int color=1;
  std::ostringstream os( std::ostringstream::ate );
  for(int i=0; i<argc-3; i++){
    if( color==5 ) color++;
    int npart=atoi(argv[i+3]);
    os.str("");
    os << "h0-" << npart;
    h0[i]=new TH2D(os.str().c_str(),"",40,0,40,20,-0.5,19.5);
    os.str("");
    os << "h1-" << npart;
    h1[i]=new TH2D(os.str().c_str(),"",40,0,40,20,-0.5,19.5);
    color++;
    nevent0[i]=0;
    nevent1[i]=0;
    p0[i]=NULL;
    p1[i]=NULL;
  }
  color=1;
  
  for(int i=0; i<argc-3; i++){
    int npart=atoi(argv[i+3]);
    os.str("");
    os << energy << "GeVMuon-" << npart << "Particles.root";
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
    int nhit;
    float simEta;
    std::vector<int> *nhitInCylinder;
    tree->SetBranchAddress("distanceToProjection",&distanceToProjection);
    tree->SetBranchAddress("ntrack",&ntrack);
    tree->SetBranchAddress("nhit",&nhit);
    tree->SetBranchAddress("simEta",&simEta);
    tree->SetBranchAddress("nhitInCylinder",&nhitInCylinder);
    const unsigned nEvts = tree->GetEntries();
    for( unsigned ievt(0); ievt<nEvts; ++ievt ){
      tree->GetEntry(ievt);
      if( nhit<10 || simEta<1.71 )
	continue;
      if( ntrack>0 && distanceToProjection<dCut ) {
	nevent0[i]++;
	for(int ilayer=0; ilayer<40; ilayer++)
	  h0[i]->Fill(ilayer,nhitInCylinder->at(ilayer));
      }
      else{
	nevent1[i]++;
	for(int ilayer=0; ilayer<40; ilayer++)
	  h1[i]->Fill(ilayer,nhitInCylinder->at(ilayer));
      }
      if( (ievt+1)%1000==0 )
	std::cout << "Entry " << ievt+1 << std::endl;
    }
  }
  
  CaliceStyle();
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  TCanvas *can=new TCanvas();
  can->SetWindowSize(600,600);
  TLegend *leg=new TLegend(0.4,0.5,1.0,0.9);
  leg->SetFillStyle(0);

  TH1D* hToto=new TH1D("hToto","",40,0,40);
  hToto->GetXaxis()->SetTitle("Layer number");
  hToto->GetYaxis()->SetTitle("<N_{hit}> #in 2cm cylinder");
  hToto->SetMinimum(0.8);
  hToto->SetMaximum(3.5);
  hToto->Draw("axis");

  for(int i=0; i<argc-3; i++){
    int npart=atoi(argv[i+3]);
    if( nevent0[i] > 50 ) p0[i]=h0[i]->ProfileX();
    if( nevent1[i] > 50 ) p1[i]=h1[i]->ProfileX();
    if( color==5 ) color++;
    if( NULL!=p0[i] ){
      p0[i]->SetMarkerStyle(20);
      p0[i]->SetMarkerColor(color);
      p0[i]->SetLineColor(color);
      p0[i]->Draw("psame");
      os.str("");os << "Reconstructed muons (" << npart << " particles)" << std::endl;
      leg->AddEntry(p0[i],os.str().c_str(),"p");
    }
    if( NULL!=p1[i] ){
      p1[i]->Draw("psame");
      p1[i]->SetMarkerStyle(24);
      p1[i]->SetMarkerColor(color);
      p1[i]->SetLineColor(color);
      os.str("");os << "Non reconstructed muons (" << npart << " particles)" << std::endl;
      leg->AddEntry(p1[i],os.str().c_str(),"p");
    }
    color++;
  }
  leg->Draw();
  can->Update();

  sleep(10);

  os.str("");
  os << "plots/" << energy << "GeVMuon_HitCylinder.pdf";
  can->SaveAs(os.str().c_str());
  os.str("");
  os << "plots/" << energy << "GeVMuon_HitCylinder.C";
  can->SaveAs(os.str().c_str());
  delete can;
  return 0;
}
