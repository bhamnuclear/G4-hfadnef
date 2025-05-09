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
/// \file B1/include/EventAction.hh
/// \brief Definition of the B1::EventAction class

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>
#include "TString.h"
#include "G4ThreeVector.hh"
/// Event action class
///

namespace B1
{

class RunAction;

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;
    std::vector<int> feventno;
    std::vector<double> fEn;
    std::vector<double> fth;
    std::vector<double> flength;
    std::vector<G4ThreeVector> fpos;
    std::vector<TString> fname;
    void AddEdep(G4int eventno, TString pname, G4double edep,G4double th) {feventno.push_back(eventno), fname.push_back(pname); fEn.push_back(edep);fth.push_back(th); }
    void AddEdep(G4int eventno, TString pname, G4double edep,G4ThreeVector th) {feventno.push_back(eventno), fname.push_back(pname); fEn.push_back(edep);fpos.push_back(th);flength.push_back(1); }
    void AddEdep(G4int eventno, TString pname, G4double edep,G4ThreeVector th,G4double length) {feventno.push_back(eventno), fname.push_back(pname); fEn.push_back(edep);fpos.push_back(th);flength.push_back(length); }

  private:
    RunAction* fRunAction = nullptr;
    G4double   fEdep = 0.;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


