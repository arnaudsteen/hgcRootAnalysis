#include "loopForFSRStudy.h"
#include <TFile.h>
#include <TTree.h>

loopForFSRStudy::loopForFSRStudy() : distanceCut(10.0)
{;}

void loopForFSRStudy::runLongiProfile(std::string fileName,TH2D* hLongi, bool muonTag)
{
  TFile* file=new TFile(fileName.c_str(),"READ");
  if( file->IsOpen() )
    file->Print();
  else
    std::cout << "can not open " << fileName << std::endl;
  TTree *tree = (TTree*)file->Get("tree");
  if (!tree){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return;
  }
  
  tree->SetBranchAddress("ntrack",&ntrack);
  tree->SetBranchAddress("nlayer",&nlayer);
  tree->SetBranchAddress("longitudinal",&longitudinal);
  tree->SetBranchAddress("distanceToProjection",&distanceToProjection);

  const unsigned nEvts = tree->GetEntries();
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
    tree->GetEntry(ievt);
    if( muonTag ){
      if( ntrack>0 && distanceToProjection < distanceCut ){
	for(unsigned int iLayer=0; iLayer<longitudinal->size(); iLayer++){
	  if( longitudinal->at(iLayer)*1000-9.01852000000000070e-02>0 )
	    hLongi->Fill(iLayer,longitudinal->at(iLayer)*1000-9.01852000000000070e-02);
	  else
	    hLongi->Fill(iLayer,0.0);
	}
      }
    }
    else
      for(unsigned int iLayer=0; iLayer<longitudinal->size(); iLayer++)
	hLongi->Fill(iLayer,longitudinal->at(iLayer)*1000);
  }
}

void loopForFSRStudy::runRadialProfile(std::string fileName,TH2D* hRadial,bool muonTag)
{
  TFile* file=new TFile(fileName.c_str(),"READ");
  if( file->IsOpen() )
    file->Print();
  else
    std::cout << "can not open " << fileName << std::endl;
  TTree *tree = (TTree*)file->Get("tree");
  if (!tree){
    std::cout << " -- Error, tree cannot be opened. Exiting..." << std::endl;
    return;
  }

  tree->SetBranchAddress("ntrack",&ntrack);
  tree->SetBranchAddress("transverse", &transverse);
  tree->SetBranchAddress("distanceToProjection",&distanceToProjection);

  const unsigned nEvts = tree->GetEntries();
  for( unsigned ievt(0); ievt<nEvts; ++ievt ){
    tree->GetEntry(ievt);
    if( muonTag ){
      if( ntrack>0 && distanceToProjection < distanceCut ){
	for(unsigned int iRing=0; iRing<transverse->size(); iRing++){
	  if( iRing==0 )
	    if( transverse->at(iRing)*1000-9.01852000000000070e-02>0 )
	      hRadial->Fill(iRing,transverse->at(iRing)*1000-9.01852000000000070e-02);
	    else
	      hRadial->Fill(iRing,0.0);
	  else
	    hRadial->Fill(iRing,transverse->at(iRing)*1000);
	}
      }
    }
    else
      for(unsigned int iRing=0; iRing<transverse->size(); iRing++)
	hRadial->Fill(iRing,transverse->at(iRing)*1000);
  }
}

void loopForFSRStudy::runEdep(std::string fileName,TH1D* h)
{
}
