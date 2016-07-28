#ifndef __LOOPFORFSRSTUDY__
#define __LOOPFORFSRSTUDY__

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <TH2D.h>
#include <TH1D.h>

class loopForFSRStudy
{
 public:
  loopForFSRStudy();
  ~loopForFSRStudy(){;}
  void runLongiProfile(std::string fileName,TH2D* hLongi, bool muonTag);
  void runRadialProfile(std::string fileName,TH2D* hRadial, bool muonTag);
  void runEdep(std::string fileName,TH1D* h);

  void setDistanceCut(float val){distanceCut=val;}

 private:
  int ntrack;
  int nlayer;
  std::vector<double> *transverse;
  std::vector<double> *longitudinal;
  float distanceToProjection;
  float edep;

  float distanceCut;
};

#endif
