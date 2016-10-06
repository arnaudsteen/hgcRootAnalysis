#include <iostream>
#include <fstream>
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
#include <TGaxis.h>
#include <TLegend.h>

#include "CaliceStyle.h"

double resolution(double *x, double *par)
{
  return sqrt( par[0]*par[0]/x[0] + par[1]*par[1] + par[2]*par[2]/(x[0]*x[0]) );
}

double linearFitFunc(double *x, double *par)
{
  return par[0]*x[0];
}

int main(int argc,char** argv)
{
  float linearLimitEnergy=30.0;
  if(argc>=2)
    linearLimitEnergy=atof(argv[1]);
  
  float linearLGLimitEnergy=260;
  if(argc>=3)
    linearLGLimitEnergy=atof(argv[2]);

  int nP=7;
  if(argc>=4)
    nP=atoi(argv[3]);

  TGaxis::SetMaxDigits(3);
  
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  CaliceStyle();
  gStyle->SetPadTopMargin(0.10);
  
  Double_t meanE[nP];
  Double_t meanErrorE[nP];
  Double_t sigmaE[nP];
  Double_t sigmaErrorE[nP];
  Double_t energies[]={20.,35.,70.,150.,200.,250.};
  Double_t energiesError[]={0.,0.,0.,0.,0.,0.,0.};
  Double_t lg_meanE[nP];
  Double_t lg_meanErrorE[nP];
  Double_t lg_sigmaE[nP];
  Double_t lg_sigmaErrorE[nP];
  
  std::ostringstream os(std::ostringstream::ate);
  os.str("");
  os << "fitResult.txt";

  std::fstream in;
  in.open( os.str().c_str() );
  int i=0;

  float e,m,me,s,se;
  
  if( in.good() ){
    std::cout << "Reading " << os.str() << std::endl;
    while(i<nP){
      in >> e >> m >> me >> s >> se;
      energies[i]=e;
      energiesError[i]=e/100.0;
      meanE[i]=m;
      meanErrorE[i]=me;
      sigmaE[i]=s/m;
      sigmaErrorE[i]=1/m*std::sqrt( se*se + s*s*me*me/(m*m) );
      std::cout << e << "\t"
		<< m << ", " << me << "\t"
		<< s << ", " << se << std::endl;
      i++;
    }
  }
  in.close();

  os.str("");
  os << "fitResultLow.txt";
  in.open( os.str().c_str() );
  i=0;
  if( in.good() ){
    std::cout << "Reading " << os.str() << std::endl;
    while(i<nP){
      in >> e >> m >> me >> s >> se;
      lg_meanE[i]=m;
      lg_meanErrorE[i]=me;
      lg_sigmaE[i]=s/m;
      lg_sigmaErrorE[i]=1/m*std::sqrt( se*se + s*s*me*me/(m*m) );
      std::cout << e << "\t"
		<< m << ", " << me << "\t"
		<< s << ", " << se << std::endl;
      i++;
    }
  }
  in.close();
  
  
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
  pad1->SetTopMargin(0.05);
  pad1->SetBottomMargin(0);
  pad1->SetFrameBorderMode(0);
  //pad1->SetLogy();
  
  TGraphErrors* grLin=new TGraphErrors(nP,energies,meanE,energiesError,meanErrorE);
  grLin->SetMarkerStyle(20);
  grLin->SetMarkerSize(0.8);
  grLin->SetLineColor(kBlack);
  grLin->SetMarkerColor(kBlack);
  TF1 *func0=new TF1("func0",linearFitFunc,0,linearLimitEnergy,1);
  func0->SetLineWidth(2);
  func0->SetLineColor(kBlack);
  func0->SetLineStyle(1);
  func0->SetParameter(0,100);
  TF1 *func1=new TF1("func1",linearFitFunc,linearLimitEnergy,260,1);
  func1->SetLineWidth(2);
  func1->SetLineColor(kBlack);
  func1->SetLineStyle(2);

  TGraphErrors* grLinLG=new TGraphErrors(nP,energies,lg_meanE,energiesError,lg_meanErrorE);
  grLinLG->SetMarkerStyle(21);
  grLinLG->SetMarkerSize(0.8);
  grLinLG->SetMarkerColor(kBlue);
  grLinLG->SetLineColor(kBlue);
  TF1 *func0LG=new TF1("func0LG",linearFitFunc,0,linearLGLimitEnergy,1);
  func0LG->SetLineWidth(2);
  func0LG->SetLineColor(kBlue);
  func0LG->SetLineStyle(1);
  func0LG->SetParameter(0,100);
  TF1 *func1LG=new TF1("func1LG",linearFitFunc,linearLGLimitEnergy,260,1);
  func1LG->SetLineWidth(2);
  func1LG->SetLineColor(kBlue);
  func1LG->SetLineStyle(2);

  
  TH1D hL("hL","",100,0,260);
  hL.SetMinimum(10);
  hL.SetMaximum(30000);
  hL.GetXaxis()->SetTitle("E_{beam} [GeV]");
  hL.GetXaxis()->SetTitleOffset(0.9);
  hL.GetYaxis()->SetTitle("#bar{ADC} (High gain)");
  hL.GetYaxis()->SetTitleOffset(1.8);
  hL.GetXaxis()->SetLabelFont(43);
  hL.GetXaxis()->SetLabelSize(0);
  hL.GetXaxis()->SetTitleFont(43);
  hL.GetXaxis()->SetTitleSize(0); 
  hL.GetYaxis()->SetLabelFont(43);
  hL.GetYaxis()->SetTitleSize(30);
  hL.GetYaxis()->SetLabelSize(20);
  hL.GetYaxis()->SetTitleFont(43);
  hL.GetZaxis()->SetLabelFont(42);
  hL.GetZaxis()->SetTitleSize(0.05);
  hL.GetZaxis()->SetTitleFont(42);

  hL.Draw("axis");
  grLin->Draw("psame");
  grLin->Fit(func0,"N","",0.0,linearLimitEnergy);
  func1->SetParameter( 0, func0->GetParameter(0) );
  func1->Draw("same");
  func0->Draw("same");
  grLinLG->Draw("psame");
  grLinLG->Fit(func0LG,"N","",0.0,linearLGLimitEnergy);
  func1LG->SetParameter( 0, func0LG->GetParameter(0) );
  func1LG->Draw("same");
  func0LG->Draw("same");

  TLegend *legLin=new TLegend(0.2,0.6,0.55,0.75);
  legLin->AddEntry(grLin,"High gain","p");
  legLin->AddEntry(grLinLG,"Low gain","p");
  legLin->Draw();
  
  pad1->Modified();

  /***********/

  Double_t deviationE[nP];
  Double_t deviationErrorE[nP];
  Double_t lg_deviationE[nP];
  Double_t lg_deviationErrorE[nP];
  for( unsigned int i=0; i<nP; i++){
    Double_t fParError[]={func0->GetParameter(0)+func0->GetParError(0)};
    Double_t x[]={energies[i]};
    Double_t fError=func0->EvalPar( x,fParError );

    fParError[0]=func0->GetParameter(0)-func0->GetParError(0);
    fError-=func0->EvalPar( x,fParError );
    fError=std::fabs(fError);
    
    deviationE[i]= (meanE[i]-func0->Eval( energies[i] ))/func0->Eval( energies[i] );
    deviationErrorE[i]= 1/func0->Eval( energies[i] )*std::sqrt( meanErrorE[i]*meanErrorE[i] +
								meanE[i]*meanE[i]*fError*fError/(func0->Eval( energies[i] )*func0->Eval( energies[i] )));
    
    Double_t fParErrorLG[]={func0LG->GetParameter(0)+func0LG->GetParError(0)};
    Double_t fErrorLG=func0LG->EvalPar( x,fParErrorLG );

    fParErrorLG[0]=func0LG->GetParameter(0)-func0LG->GetParError(0);
    fErrorLG-=func0LG->EvalPar( x,fParErrorLG );
    fErrorLG=std::fabs(fErrorLG);
    
    lg_deviationE[i]= (lg_meanE[i]-func0LG->Eval( energies[i] ))/func0LG->Eval( energies[i] );
    lg_deviationErrorE[i]= 1/func0LG->Eval( energies[i] )*std::sqrt( lg_meanErrorE[i]*lg_meanErrorE[i] +
								     lg_meanE[i]*lg_meanE[i]*fErrorLG*fErrorLG/(func0LG->Eval( energies[i] )*func0LG->Eval( energies[i] )));
  }
  
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
  grDev->SetLineColor(kBlack);
  TGraphErrors* grDevLG=new TGraphErrors(nP,energies,lg_deviationE,energiesError,lg_deviationErrorE);
  grDevLG->SetMarkerStyle(21);
  grDevLG->SetMarkerSize(0.8);
  grDevLG->SetMarkerColor(kBlue);
  grDevLG->SetLineColor(kBlue);
  
  TH1D hD("hD","",100,0,260);
  hD.SetMinimum(-0.5);
  hD.SetMaximum(0.5);
  hD.GetXaxis()->SetTitle("E_{beam} [GeV]");
  hD.GetXaxis()->SetTitleOffset(0.9);
  hD.GetYaxis()->SetTitle(" #Delta_{ADC}/ADC_{Fit}");
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

  TF1 *func2=new TF1("func2","pol0",0,260);
  func2->SetLineWidth(1);
  func2->SetLineColor(kBlack);
  func2->SetLineStyle(1);
  func2->SetParameter(0,0.0);

  hD.Draw("axis");
  grDev->Draw("p");
  grDevLG->Draw("p");
  func2->Draw("same");
  pad2->Modified();

  lin->Modified();
  lin->Update();
  lin->SaveAs("ereco_vs_ebeam.pdf");
  
  //lin->WaitPrimitive();

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
  TGraphErrors* grResLG=new TGraphErrors(nP,energies,lg_sigmaE,energiesError,lg_sigmaErrorE);
  grResLG->SetMarkerStyle(21);
  grResLG->SetMarkerSize(0.8);
  grResLG->SetMarkerColor(kBlue);

  TH1D hR("hR","",100,0,260);
  hR.SetMinimum(0.0);
  hR.SetMaximum(0.5);
  hR.GetXaxis()->SetTitle("E_{beam} [GeV]");
  hR.GetYaxis()->SetTitle("#sigma_{ADC}/#bar{ADC}");
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

  TF1 *resf=new TF1("resf",resolution,0,260,3);
  resf->SetLineWidth(2);
  resf->SetLineColor(kBlack);
  //  resf->SetParLimits(2,0.0,7.0);
  //grResLG->Fit(resf,"N");
  
  TLegend *legRes=new TLegend(0.5,0.5,0.85,0.65);
  legRes->AddEntry(grRes,"High gain","p");
  legRes->AddEntry(grResLG,"Low gain","p");

  hR.Draw("axis");
  grRes->Draw("psame");
  grResLG->Draw("psame");
  //resf->Draw("same");
  legRes->Draw();
  res->Modified();
  res->Update();
  res->SaveAs("resolution_vs_ebeam.pdf");
  res->WaitPrimitive();

  delete func0;
  delete func1;
  delete func0LG;
  delete func1LG;
  delete grLin;
  delete grDev;
  delete grLinLG;
  delete grDevLG;
  delete pad1;
  delete pad2;
  delete lin;

  delete res;
  delete grRes;
  delete grResLG;
  delete resf;
  return 0;
}
