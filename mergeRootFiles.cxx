#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <TROOT.h>
#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TChain.h>
#include <exception>

int main(int argc,char** argv)
{
  std::vector< std::string > fileList;
  std::vector< std::string > h1List;
  std::vector< std::string > h2List;
  for(int i=1; i<argc; i++){
    std::string aStr=std::string(argv[i]);

    if( aStr.find(".root")!=std::string::npos )
      fileList.push_back(aStr);

    if( aStr.find("h1_")!=std::string::npos )
      h1List.push_back(aStr);

    if( aStr.find("h2_")!=std::string::npos )
      h2List.push_back(aStr);
  }

  if( fileList.empty() ){
    std::cout << "Error the list of files is empty or not correct" << std::endl;
    return 0;
  }
  
  for(std::vector<std::string>::iterator it=fileList.begin(); it!=fileList.end(); ++it){
    TFile *input=new TFile( (*it).c_str(), "READ");
    if( !input->IsOpen() ){
      std::cout << "---------------------------------------------------" << std::endl;
      std::cout << "----------------- !!!! ERROR !!!! -----------------" << std::endl;
      std::cout << "---------------------------------------------------" << std::endl;
      std::cout << (*it).c_str() << " no such file or directory -> std::abort() "<< std::endl;
      std::abort();
    }
    if( !input->GetListOfKeys()->Contains("tree") ){
      std::cout << "---------------------------------------------------" << std::endl;
      std::cout << "----------------- !!!! ERROR !!!! -----------------" << std::endl;
      std::cout << "---------------------------------------------------" << std::endl;
      std::cout << "wrong file format : no TTree called tree was found in " << (*it).c_str() << " -> std::abort() "<< std::endl;
      std::abort();
    }
    for(std::vector<std::string>::iterator jt=h1List.begin(); jt!=h1List.end(); ++jt){
      if( !input->GetListOfKeys()->Contains((*jt).c_str()) ){
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "----------------- !!!! ERROR !!!! -----------------" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "wrong histogramm name : " << (*it) << "was not found in " << (*jt) << " -> std::abort() "<< std::endl;
	std::abort();
      }
    }
    for(std::vector<std::string>::iterator jt=h2List.begin(); jt!=h2List.end(); ++jt){
      if( !input->GetListOfKeys()->Contains((*jt).c_str()) ){
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "----------------- !!!! ERROR !!!! -----------------" << std::endl;
	std::cout << "---------------------------------------------------" << std::endl;
	std::cout << "wrong histogramm name : " << (*it) << "was not found in " << (*jt) << " -> std::abort() "<< std::endl;
	std::abort();
      }
    }
    input->Close();
  }
  
  TChain chain("tree");
  for(std::vector<std::string>::iterator it=fileList.begin(); it!=fileList.end(); ++it){
    chain.Add( (*it).c_str() );
    std::cout << (*it) << std::endl;
  }
  chain.Merge("output.root");


  TFile *output=new TFile( "output.root", "UPDATE");
  for(std::vector<std::string>::iterator it=h1List.begin(); it!=h1List.end(); ++it){
    TList *list = new TList();
    for(std::vector<std::string>::iterator jt=fileList.begin(); jt!=fileList.end(); ++jt){
      TFile *input=new TFile( (*jt).c_str(), "READ");
      list->Add( (TH1D*)input->Get( (*it).c_str() ) );
    }
    TH1D *h=(TH1D*)list->At(0);
    h->Reset();
    h->Merge(list);
    output->cd();
    h->Write();
    std::cout << (*it) << std::endl;
  }
  for(std::vector<std::string>::iterator it=h2List.begin(); it!=h2List.end(); ++it){
    TList *list = new TList();
    for(std::vector<std::string>::iterator jt=fileList.begin(); jt!=fileList.end(); ++jt){
      TFile *input=new TFile( (*jt).c_str(), "READ");
      list->Add( (TH2D*)input->Get( (*it).c_str() ) );
    }
    TH2D *h=(TH2D*)list->At(0);
    h->Reset();
    h->Merge(list);
    output->cd();
    h->Write();
    std::cout << (*it) << std::endl;
  }
  output->Close();
  return 0;
}
