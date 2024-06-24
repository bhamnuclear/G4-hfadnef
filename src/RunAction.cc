//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1/src/RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
// #include "Run.hh"
#include <iostream>
#include <fstream>
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
//#include "G4AnalysisManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "TThread.h"
#include "G4AutoDelete.hh"
namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // add new units for dose
  //
  const G4double milligray = 1.e-3*gray;
  const G4double microgray = 1.e-6*gray;
  const G4double nanogray  = 1.e-9*gray;
  const G4double picogray  = 1.e-12*gray;

  new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);
  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fEdep);
  accumulableManager->RegisterAccumulable(fEdep2);
  int threadid=G4Threading::G4GetThreadId();
  if(fout[threadid]==nullptr && threadid>=0) {
    G4cout<<"THREAD: "<<Form("neutrons%d.root",threadid)<<G4endl;

    fout[threadid]=new TFile(Form("neutrons%d.root",threadid),"RECREATE");
    tree[threadid] = new TTree("data","data");
//    G4AutoDelete::Register(fout[threadid]);
//    G4AutoDelete::Register(tree[threadid]);

    tree[threadid]->Branch("feventno",&feventno);
    tree[threadid]->Branch("fname",&fname);
    tree[threadid]->Branch("fEn",&fEn);
    tree[threadid]->Branch("fth",&fth);
    tree[threadid]->Branch("fX",&fX);
    tree[threadid]->Branch("fY",&fY);
    tree[threadid]->Branch("fZ",&fZ);
    tree[threadid]->Branch("fZ",&fZ);
    tree[threadid]->Branch("flength",&flength);
  }

}
RunAction::~RunAction()
{
  int threadid=G4Threading::G4GetThreadId();
    G4cout<<"Trying to close thread "<<threadid<<G4endl;
//    if(threadid < 0) threadid=0;
//	if(fout[threadid]!=0) fout[threadid]->Close();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  // Get analysis manager
//  auto analysisManager = G4AnalysisManager::Instance();
  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = fEn.size();
  if (nofEvents == 0) return;

  int threadid=G4Threading::G4GetThreadId();
  G4cout<<nofEvents<<" neutrons in target for thread "<<threadid<<G4endl;
//  G4cout<<"Run ID: "<<thr<<G4endl;
  std::ofstream outs;
//  outs.open(Form("pneutrons_%d.txt",thr),std::ios_base::app);
  G4cout<<"Events: "<<nofEvents<<G4endl;
  if(nofEvents==0) return;
//  if(threadid<0) threadid=0;
  if(fout[threadid] != nullptr) {
    G4cout<<"FILL "<<threadid<<"\t"<<tree[threadid]<<"\t"<<fout[threadid]<<G4endl;
    tree[threadid]->Fill();
//    tree[threadid]->Print();
    fout[threadid]->cd();
    tree[threadid]->Write();
    fout[threadid]->Close();//Test here
//    fout[threadid]->ls();
//    for(int i=0;i<1000;i++) {
//      G4cout<<threadid<<G4endl;
//    }

//    delete tree[threadid];
//    tree[threadid] = 0;//Reset pointer
//    delete fout[threadid];
//    fout[threadid] = 0;//Reset pointer
  }
  else {
    G4cout<<"Error for threadid "<<threadid<<G4endl;
  }

//  tree->Branch("hE",&hE);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::AddEdep(int eventno, TString pname, G4double edep, G4double th)
{
  feventno.push_back(int(eventno));
  fname.push_back(pname);
  fEn.push_back(edep);
  fth.push_back(th);
}

void RunAction::AddEdep(int eventno, TString pname, G4double edep, G4ThreeVector pos, G4double length)
{
  feventno.push_back(int(eventno));
  fname.push_back(pname);
  fEn.push_back(edep);
  fX.push_back(pos.x());
  fY.push_back(pos.y());
  fZ.push_back(pos.z());
  flength.push_back(length);
//  G4cout<<eventno<<"\t"<<edep<<"\t"<<length<<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
