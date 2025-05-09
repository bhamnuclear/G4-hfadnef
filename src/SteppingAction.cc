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
//  step->SetStepLength(1e-7*m); //Doesn't work

  G4ParticleDefinition* projectile = step->GetTrack()->GetDefinition();
  G4String logicname = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetName();
  TString particlename=projectile->GetParticleName();
//  if(step->GetTrack()->GetCurrentStepNumber()==0) G4cout<<particlename<<G4endl;//What is going on :)

//  Limit simulation time
  G4double time = step->GetTrack()->GetGlobalTime();
//  G4cout<<particlename<<"\t"<<time/second<<G4endl;
  if(projectile->GetPDGLifeTime()/(second*3.154e+7) > 1) {
    step->GetTrack()->SetTrackStatus(fStopAndKill);//Ignore any tracks longer than one day
//    G4cout<<"Killing: "<<particlename<<G4endl;
  }
 // Kill anything below 1 metre negative Y
  G4ThreeVector location = step->GetTrack()->GetPosition();
//  if(particlename=="neutron") G4cout<<logicname<<"\t"<<step->GetTrack()->GetKineticEnergy()/keV<<"\t"<<step->GetTotalEnergyDeposit()<<"\t"<<location.x()<<"\t"<<location.y()<<"\t"<<location.z()<<G4endl;
  
  if(location.y()<-1*m || (step->GetTrack()->GetKineticEnergy()>0 && step->GetStepLength()<1e-7*m)) {
//    step->GetTrack()->SetTrackStatus(fStopAndKill);
    //G4cout<<"Killing: "<<particlename<<" at "<<location<<G4endl;
  }
//  if(projectile->GetParticleName()=="e-") {
//    step->GetTrack()->SetTrackStatus(fStopAndKill);//Ignore any electrons!
//  }

//G4cout<<projectile->GetParticleName()<<G4endl;
//  G4cout<<logicname<<G4endl;
//  if(projectile->GetParticleName()!="neutron" && projectile->GetParticleName()!="e-" && projectile->GetParticleName()!="e+" && projectile->GetParticleName()!="gamma" && logicname=="World") {
//  if(projectile->GetParticleName()=="Ar41" && logicname=="World") {
//    G4cout<<projectile->GetParticleName()<<"\t"<<step->GetTrack()->GetParentID()<<G4endl;
//    step->GetTrack()->SetTrackStatus(fStopAndKill);//
//  }


  int runnumber = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  G4double edep=step->GetTotalEnergyDeposit();
// TEMPORARILY DISABLED THE LOGICTARGET
//  if(logicname=="logicCLLBC" && time/second<1) {//CLLBC detector mode -- prompt only (1 second)
//   G4cout<<projectile->GetParticleName()<<"\t"<<logicname<<G4endl;
  if(((projectile->GetParticleName()=="neutron" || 1)  && (logicname=="logictarget"))) {//Neutron flux mode
//     fEventAction->AddEdep(runnumber,particlename,step->GetTrack()->GetDynamicParticle()->GetKineticEnergy()/MeV,step->GetTrack()->GetPosition());//Option for neutron monitoring
     fEventAction->AddEdep(runnumber,particlename,step->GetTrack()->GetDynamicParticle()->GetKineticEnergy()/MeV,step->GetTrack()->GetPosition(),step->GetStepLength());//Option for neutron monitoring biased by step length
//     fEventAction->AddEdep(runnumber,particlename,edep/MeV,step->GetTrack()->GetPosition(),step->GetStepLength());//Option for CLLBC
//    G4cout<<step->GetTrack()->GetDynamicParticle()->GetKineticEnergy()/MeV<<G4endl;
  }


//  if((logicname=="gamma_logical" && edep>0)) {//Neutron/gamma damage mode
//     fEventAction->AddEdep(runnumber,particlename,step->GetTrack()->GetDynamicParticle()->GetKineticEnergy()/MeV,step->GetTrack()->GetMomentum().theta());
//     fEventAction->AddEdep(runnumber,particlename,edep/MeV,step->GetTrack()->GetMomentum().theta());//Option for neutron monitoring
//     fEventAction->AddEdep(runnumber,particlename,step->GetTrack()->GetDynamicParticle()->GetKineticEnergy()/MeV,step->GetTrack()->GetPosition());//Option for neutron monitoring
//     fEventAction->AddEdep(runnumber,particlename,edep/MeV,step->GetTrack()->GetPosition());//Option for activation analysis
//     step->GetTrack()->SetTrackStatus(fStopAndKill);//Kill neutron once it is in target
//    }
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

