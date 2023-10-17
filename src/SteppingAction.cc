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
/// \file B1/src/SteppingAction.cc
/// \brief Implementation of the B1::SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "TString.h"
namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction)
: fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4ParticleDefinition* projectile = step->GetTrack()->GetDefinition();
  G4String logicname = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
  TString particlename=projectile->GetParticleName();
//  Limit simulation time
  G4double time = step->GetTrack()->GetGlobalTime();
//  G4cout<<particlename<<"\t"<<time/second<<G4endl;
  if(projectile->GetPDGLifeTime()/(second*3.154e+7) > 1) {
    step->GetTrack()->SetTrackStatus(fStopAndKill);//Ignore any tracks longer than one day
    G4cout<<"Killing: "<<particlename<<G4endl;
  }
//  if(particlename.Contains("Be7")) G4cout<<projectile->GetParticleName()<<"\t"<<logicname<<G4endl;
//  if(projectile->GetParticleName()=="neutron"/*&& step->GetTrack()->GetCurrentStepNumber()==1*/) {
//	G4ThreeVector position = step->GetTrack()->GetPosition();
//	G4cout<<logicname<<"\t"<<position.getX()<<"\t"<<position.getY()<<"\t"<<position.getZ()<<G4endl;
//   }

//  if(projectile->GetParticleName()=="neutron"/*&& step->GetTrack()->GetCurrentStepNumber()==1*/) {
  int runnumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  G4double edep=step->GetTotalEnergyDeposit();
//  if(projectile->GetParticleName()=="neutron" && (logicname=="gamma_logical" || logicname=="logictarget")) {//Neutron flux mode
  if((logicname=="gamma_logical" && edep>0)) {//Neutron/gamma damage mode
//     fEventAction->AddEdep(runnumber,particlename,step->GetTrack()->GetDynamicParticle()->GetKineticEnergy()/MeV,step->GetTrack()->GetMomentum().theta());
     fEventAction->AddEdep(runnumber,particlename,edep/MeV,step->GetTrack()->GetMomentum().theta());
     step->GetTrack()->SetTrackStatus(fStopAndKill);//Kill neutron once it is in target
  }
  if (!fScoringVolume) {
    const auto detConstruction = static_cast<const DetectorConstruction*>(
      G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detConstruction->GetScoringVolume();
  }

  // get volume of the current step
  G4LogicalVolume* volume
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

