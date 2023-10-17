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

  fout=new TFile("output.root","RECREATE");
  tree = new TTree("data","data");
  tree->Branch("feventno",&feventno);
  tree->Branch("fname",&fname);
  tree->Branch("fEn",&fEn);
  tree->Branch("fth",&fth);


}
RunAction::~RunAction()
{
    fout->Close();
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
//  G4cout<<nofEvents<<" neutrons"<<G4endl;
  int thr=G4Threading::G4GetThreadId();
  G4cout<<"Thread: "<<thr<<G4endl;
//  G4cout<<"Run ID: "<<thr<<G4endl;
  if (nofEvents == 0) return;
  std::ofstream outs;
//  outs.open(Form("pneutrons_%d.txt",thr),std::ios_base::app);
  outs.open(Form("pneutrons_%d.txt",0),std::ios_base::app);
  for(int i=0;i<nofEvents;i++) {
//    G4cout<<"Neutron:\t"<<thr<<"\t"<<fEn.at(i)/MeV<<G4endl;
    outs<<fEn.at(i)/MeV<<"\t"<<fth.at(i)<<std::endl;
  }
  outs.close();
  G4cout<<"Events: "<<nofEvents<<G4endl;
  if(nofEvents==0) return;
//  if(nofEvents>4000) return;//Fix when it does this again
    // Open an output file
  G4String fileName = "output.root";
//  TThread::Initialize();
//  G4String fileName = "neutrons.root";
//  analysisManager->OpenFile(fileName);
  if(fout!=0) {
    G4cout<<"FILL"<<G4endl;
    tree->Fill();
    tree->Print();
    tree->Write();
    G4cout<<"CLOSE"<<G4endl;

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
