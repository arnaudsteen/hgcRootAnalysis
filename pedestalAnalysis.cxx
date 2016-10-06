#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <cmath>
#include <limits>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
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

int main(int argc,char** argv)
{
  int nlayers=8;
  int nskirocs=2; //per layer
  int nchannels=64; //per skiroc

  std::fstream outtex;
  outtex.open("/Users/arnaudsteen/cms/presentation/hgcal/tb_pedestal/10-2016/pedestal.tex",std::ios::out);
  std::fstream outfit;
  outfit.open("./txtFile/pedestalFitOutput.txt",std::ios::out);
  
  int argc1=0;
  char* argv1=(char*)"";
  TApplication* app = new TApplication("toto",&argc1,&argv1);

  CaliceStyle();
  gStyle->SetPadTopMargin(0.10);
  gStyle->SetOptStat(0);
	
  TCanvas cc; cc.SetWindowSize(1000,1200);
  cc.Divide(8,8);
  
  std::ostringstream os( std::ostringstream::ate );  
  os.str("");
  os << "/data/root/hgcalSep2016/PED_Output_Displays.root";
  TFile file(os.str().c_str());
  if( !file.IsOpen() )
    return 0;
  TDirectoryFile* dir=(TDirectoryFile*)file.Get("hgcaltbrechitsplotter_highgain_correlation_cm");

  TText tex;
  tex.SetTextColor(kBlack);
  tex.SetTextSize(0.05);

  TH1F hMean("hMean","",1000,-1,1);
  hMean.SetLineColor( kBlack );
  hMean.SetLineWidth( 1 );
  hMean.GetXaxis()->SetTitle( "mean" );
  TH1F hSigma("hSigma","",1000,0,10);
  hSigma.SetLineColor( kBlack );
  hSigma.SetLineWidth( 1 );
  hSigma.GetXaxis()->SetTitle( "sigma" );
  TH1F hChi2("hChi2","",1000,0,10);
  hChi2.SetLineColor( kBlack );
  hChi2.SetLineWidth( 1 );
  hChi2.GetXaxis()->SetTitle( "chi2" );

  for( int il=0; il<nlayers; il++ ){
    for( int is=0; is<nskirocs; is++ ){
      for( int ic=0; ic<nchannels; ic++ ){
	cc.cd();
	cc.cd(ic+1);
	os.str("");
	os << "Ski_" << is+1 << "_Channel_" << ic << "_Layer_" << il;
	TH1F* h=(TH1F*)dir->Get(os.str().c_str());
	h->SetLineColor(kBlack);
	h->SetLineWidth(1);
	TH1F hBase("hBase","",800,-20,60);
	hBase.GetXaxis()->SetTitle("ADC count (high gain)");
	hBase.GetXaxis()->SetTitleOffset(0.9);
	hBase.GetYaxis()->SetTitle("# events");
	hBase.SetMaximum( h->GetBinContent(h->GetMaximumBin()) + 0.05*h->GetBinContent(h->GetMaximumBin()) );
	hBase.DrawCopy("axis");
	TF1 f("f","gaus",-20,100);
	f.SetParameters(h->GetBinContent(h->GetMaximumBin()),0,2);
	f.SetLineColor(kRed);
	f.SetLineWidth(1);
	h->Draw("same");
	h->Fit("f","Q");
	os.str("");
	os << "Channel " << ic;
	tex.DrawTextNDC(0.45,0.92,os.str().c_str());

	outfit << il << "\t"
	       << is+1 << "\t"
	       << ic << "\t"
	       << f.GetParameter(1) << "\t"
	       << f.GetParError(1) << "\t"
	       << f.GetParameter(2) << "\t"
	       << f.GetParError(2) << "\t"
	       << f.GetChisquare()/f.GetNDF() << "\n";

	if( f.GetChisquare() != f.GetChisquare() || f.GetNDF() != f.GetNDF() || f.GetChisquare()/f.GetNDF()>5 ){
	  f.SetLineColor(kBlue);
	  h->Fit("f","Q");
	  continue;
	}
	
	hMean.Fill( f.GetParameter(1) );
	hSigma.Fill( f.GetParameter(2) );
	hChi2.Fill( f.GetChisquare()/f.GetNDF() );

      }
      cc.Update();
      os.str("");
      os << "~/cms/presentation/hgcal/tb_pedestal/10-2016/figs/Pedestal_Layer_" << il<< "_Skiroc" << is+1 << ".pdf";
      cc.SaveAs(os.str().c_str());

      outtex << "\\begin{frame}\n"
	     << "\t \\frametitle{Layer " << il << "; Skiroc " << is+1 << "}\n"
	     << "\t \\begin{figure}\n" 
	     << "\t \t \\includegraphics[width=1.0\\textwidth]{Pedestal_Layer_" << il<< "_Skiroc" << is+1 << ".pdf}\n"
	     << "\t \\end{figure}\n"
	     << "\\end{frame}"
	     << std::endl;
    }
  }
  
  outtex.close();

  TCanvas cSummary;
  cSummary.SetWindowSize(1800,600);
  cSummary.Divide( 3,1 );

  cSummary.cd(1);
  hMean.Draw("hist");
  cSummary.cd(2);
  hSigma.Draw("hist");
  cSummary.cd(3);
  hChi2.Draw("hist");
  cSummary.SaveAs("./plots/pedestalSummary.pdf");
  
  return 0;
}
