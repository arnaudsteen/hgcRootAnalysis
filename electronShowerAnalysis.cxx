#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <cmath>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TLegend.h>
#include <TText.h>

#include "CaliceStyle.h"

double resolution(double *x, double *par)
{
  return sqrt( par[0]*par[0]/x[0] + par[1]*par[1] + par[2]*par[2]/(x[0]*x[0]) );
}

int main(int argc,char** argv)
{
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  CaliceStyle();
  int nP=6;

  Double_t meanE[nP];
  Double_t meanErrorE[nP];
  Double_t sigmaE[nP];
  Double_t sigmaErrorE[nP];
  Double_t deviationE[nP];
  Double_t deviationErrorE[nP];
  Double_t energies[]={5,10,15,20,25,30};
  Double_t energiesError[]={5,10,15,20,25,30};
  
  float dCut=15.0;
  if(argc==2)
    dCut=atof(argv[2]);

  char fileName[200];

  TH1D hE("hE","",100,0,50);
  hE.GetXaxis()->SetTitle("Reconstructed Energy [GeV]");
  hE.GetXaxis()->SetTitleOffset(0.9);
  hE.GetYaxis()->SetTitle("# events");
  hE.GetYaxis()->SetTitleOffset(1.1);
  hE.SetLineWidth(2);
  hE.SetLineColor(kBlack);
  TF1 *func=new TF1("func","gaus",0,50);
  func->SetLineWidth(1);
  func->SetLineColor(kRed);
  
  std::ostringstream os(std::ostringstream::ate);
  
  for(unsigned int i(0); i<nP; i++){
    
    sprintf(fileName,"%s%d%s%d%s","/data/root/ShowerProcessor/single_e-_",(int)energies[i],"GeV_",0,".root");
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

    float energy;
    tree->SetBranchAddress("energy",&energy);
    const unsigned nEvts = tree->GetEntries();
    for( unsigned ievt(0); ievt<nEvts; ++ievt ){
      tree->GetEntry(ievt);
      hE.Fill(energy);
      if( (ievt+1)%100==0 )
	std::cout << "Entry " << ievt+1 << std::endl;
    }
    file.Close();

    TCanvas *can=new TCanvas();
    can->SetWindowSize(400,400);
    hE.Fit(func,"NQ");
    hE.Draw("hist");
    func->Draw("same");

    meanE[i]=func->GetParameter(1);
    meanErrorE[i]=func->GetParError(1);
    deviationE[i]=(func->GetParameter(1)-energies[i])/energies[i];
    deviationErrorE[i]=meanErrorE[i]/energies[i];
    sigmaE[i]=func->GetParameter(2)/func->GetParameter(1);
    sigmaErrorE[i]=1/(func->GetParameter(1)*func->GetParameter(1))*sqrt( func->GetParError(2)*func->GetParError(2) +
									 sigmaE[i]*sigmaE[i]*meanErrorE[i]*meanErrorE[i]);

    energiesError[i]=0;
    std::cout << meanE[i] << " +- " << meanErrorE[i] << "\n"
	      << deviationE[i] << " +- " << deviationErrorE[i] << "\n"
      	      << sigmaE[i] << " +- " << sigmaErrorE[i] << std::endl;

    can->Update();
    os.str("");
    os << "plots/ereco_" << (int)energies[i] << "GeV.pdf";
    can->SaveAs(os.str().c_str());
    delete can;
    hE.Reset();
  }

  TCanvas *lin=new TCanvas();
  lin->SetWindowSize(500,700);
  TPad *pad1 = new TPad("pad1", "Energy_1",0.02,0.37,0.95,0.99);
  pad1->Draw();
  pad1->cd();
  pad1->Range(-19,0.01,95,95);
  pad1->SetFillColor(0);
  pad1->SetBorderSize(2);
  pad1->SetTickx(1);
  pad1->SetTicky(1);
  pad1->SetLeftMargin(0.16);
  pad1->SetRightMargin(0.01);
  pad1->SetTopMargin(0.02);
  pad1->SetBottomMargin(0);
  pad1->SetFrameBorderMode(0);
  
  TGraphErrors* grLin=new TGraphErrors(nP,energies,meanE,energiesError,meanErrorE);
  grLin->SetMarkerStyle(20);
  grLin->SetMarkerSize(0.8);
  grLin->SetMarkerColor(kBlack);

  TH1D hL("hL","",100,0,35);
  hL.SetMinimum(0.0);
  hL.SetMaximum(35);
  hL.GetXaxis()->SetTitle("E_{beam} [GeV]");
  hL.GetXaxis()->SetTitleOffset(0.9);
  hL.GetYaxis()->SetTitle("E_{reco} [GeV]");
  hL.GetYaxis()->SetTitleOffset(1.1);
  hL.GetXaxis()->SetLabelFont(43);
  hL.GetXaxis()->SetLabelSize(0);
  hL.GetXaxis()->SetTitleFont(43);
  hL.GetXaxis()->SetTitleSize(0); 
  hL.GetYaxis()->SetLabelFont(43);
  hL.GetYaxis()->SetTitleSize(30);
  hL.GetYaxis()->SetLabelSize(20);
  hL.GetYaxis()->SetTitleFont(43);
  hL.GetYaxis()->SetTitleOffset(1.7);
  hL.GetZaxis()->SetLabelFont(42);
  hL.GetZaxis()->SetTitleSize(0.05);
  hL.GetZaxis()->SetTitleFont(42);

  hL.Draw("axis");
  grLin->Draw("p");
  pad1->Modified();

  /***********/

  lin->cd();
  TPad *pad2 = new TPad("pad2", "Energy_2",0.02,0.0,0.95,0.375);
  pad2->Draw();
  pad2->cd();
  pad2->Range(-19,-0.06545455,95,0.048);
  pad2->SetFillColor(0);
  pad2->SetBorderMode(0);
  pad2->SetBorderSize(2);
  pad2->SetTickx(1);
  pad2->SetTicky(1);
  pad2->SetLeftMargin(0.16);
  pad2->SetRightMargin(0.01);
  pad2->SetTopMargin(0.0);
  pad2->SetBottomMargin(0.23);
  pad2->SetFrameBorderMode(0);
  pad2->SetFrameBorderMode(0);
    
  TGraphErrors* grDev=new TGraphErrors(nP,energies,deviationE,energiesError,deviationErrorE);
  grDev->SetMarkerStyle(20);
  grDev->SetMarkerSize(0.8);
  grDev->SetMarkerColor(kBlack);

  TH1D hD("hD","",100,0,35);
  hD.SetMinimum(-0.05);
  hD.SetMaximum(0.05);
  hD.GetXaxis()->SetTitle("E_{beam} [GeV]");
  hD.GetXaxis()->SetTitleOffset(0.9);
  hD.GetYaxis()->SetTitle(" #Delta_{E}/E_{beam}");
  hD.GetYaxis()->SetTitleOffset(1.1);
  hD.GetXaxis()->SetLabelFont(43);
  hD.GetXaxis()->SetLabelSize(20);
  hD.GetXaxis()->SetTitleFont(43);
  hD.GetXaxis()->SetTitleSize(30);
  hD.GetXaxis()->SetTitleOffset(2.);
  hD.GetYaxis()->SetLabelFont(43);
  hD.GetYaxis()->SetLabelSize(20);
  hD.GetYaxis()->SetTitleSize(30);
  hD.GetYaxis()->SetTitleOffset(1.7);
  hD.GetYaxis()->SetTitleFont(43);
  hD.GetYaxis()->SetNdivisions(505);
  hD.GetZaxis()->SetLabelFont(42);
  hD.GetZaxis()->SetTitleSize(0.05);
  hD.GetZaxis()->SetTitleFont(42);

  hD.Draw("axis");
  grDev->Draw("p");
  pad2->Modified();

  lin->Modified();
  lin->Update();
  lin->SaveAs("plots/ereco_vs_ebeam.pdf");

  //  lin->WaitPrimitive();
  delete grLin;
  delete grDev;
  delete pad1;
  delete pad2;
  delete lin;


  /***************************************************************************/

  TCanvas *res=new TCanvas();
  res->SetWindowSize(500,500);
  res->SetLeftMargin(0.15);
  res->SetRightMargin(0.02);
  res->SetTopMargin(0.02);
  res->SetBottomMargin(0.12);
  
  TGraphErrors* grRes=new TGraphErrors(nP,energies,sigmaE,energiesError,sigmaErrorE);
  grRes->SetMarkerStyle(20);
  grRes->SetMarkerSize(0.8);
  grRes->SetMarkerColor(kBlack);

  TH1D hR("hR","",100,0,35);
  hR.SetMinimum(0.0);
  hR.SetMaximum(0.2);
  hR.GetXaxis()->SetTitle("E_{beam} [GeV]");
  hR.GetYaxis()->SetTitle("#sigma_{E}/E_{reco}");
  hR.GetXaxis()->SetLabelFont(43);
  hR.GetXaxis()->SetTitleFont(43);
  hR.GetYaxis()->SetLabelFont(43);
  hR.GetYaxis()->SetTitleFont(43);
  hR.GetYaxis()->SetLabelSize(20);
  hR.GetXaxis()->SetLabelSize(20);
  hR.GetYaxis()->SetTitleSize(30);
  hR.GetXaxis()->SetTitleSize(30);   
  hR.GetYaxis()->SetTitleOffset(1.1);
  hR.GetXaxis()->SetTitleOffset(0.8);

  TF1 *resf=new TF1("resf",resolution,0,35,3);
  resf->SetLineWidth(2);
  resf->SetLineColor(kRed);
  grRes->Fit(resf,"N");
  
  hR.Draw("axis");
  grRes->Draw("psame");
  resf->Draw("same");
  res->Modified();
  res->Update();
  res->SaveAs("plots/resolution_vs_ebeam.pdf");
  //  res->WaitPrimitive();
  return 0;
}
