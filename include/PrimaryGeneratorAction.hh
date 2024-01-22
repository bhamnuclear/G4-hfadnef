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
/// \file B1/include/PrimaryGeneratorAction.hh
/// \brief Definition of the B1::PrimaryGeneratorAction class

#ifndef B1PrimaryGeneratorAction_h
#define B1PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "TGraph.h"
#include "TRandom3.h"
#include <iostream>
class G4ParticleGun;
class G4Event;
class G4Box;

/// The primary generator action class with particle gun.
///
/// The default kinematic is a 6 MeV gamma, randomly distribued
/// in front of the phantom across 80% of the (X,Y) phantom size.

namespace B1
{

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;
    // BrumLiT functions
    double InteractionE(double);
    double InteractionTheta(double);
    void makesumgraph();
    double Eloss(double,double);
    double ConvertCMtoLab(double Eint,double thetaCM);
    double NeutronEnergy(double Eint, double thetalab, double thetaCM);
    double Einitial = 0;
    //BrumliT variables
    TGraph *gXS_0 = new TGraph("XS_0.txt");
    TGraph *gXS_1 = new TGraph("XS_1.txt");
    TGraph *gstopping = new TGraph("stopping_power.txt");
    TGraph *gXS_t = new TGraph("XS_t.txt"); //Atomic Data and Nuclear Data Tables, Vol. 15, No. 1, January 1975
    TGraph *gA0,*gA1,*gA2;
    TGraph *gA0p,*gA1p,*gA2p;
    TRandom3 *rndm = new TRandom3();
    double d2r=atan(1.)/45.;
    double pi=4.*atan(1.);
    double mp=1.007825;
    double mn=1.008665;
    double mLi=7.016004;
    double mBe=7.016929;
    double amu=931.4941;
    double Q=-1.64424;
    double max_depth = 0.2;//mm
    double Eth=1.8804 ;//Threshold energy
    double thetaCM,thetalab;
    bool secondks=false;
    bool excited=false;
    double protons_taken=0;

    // method from the base class
    void GeneratePrimaries(G4Event*) override;

    // method to access particle gun
    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

  private:
    G4ParticleGun* fParticleGun = nullptr; // pointer a to G4 gun class
    G4Box* fEnvelopeBox = nullptr;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
