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
/// \file B1/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "TThread.h"
namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  // default particle kinematic
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="gamma");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(6.*MeV);
  makesumgraph();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) //Uses BrumLiT code to make our own neutrons
{
  int threadid=G4Threading::G4GetThreadId();

  G4String particleName;
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4double offset = G4RandGauss::shoot(0.,(39*mm)/(2.35));//39 mm FWHM
  G4double phi = G4UniformRand()*2*4*atan(1.);//0->2pi
  fParticleGun->SetParticlePosition(G4ThreeVector(offset*cos(phi),offset*sin(phi),-0.1*mm));//Surface of Li
  if(Einitial == 0) Einitial = fParticleGun->GetParticleEnergy(); //Set class variables once from the macro
  //Sample Ep_interaction
  double Eint = InteractionE(Einitial);
  //Sample theta CM
  double thetaCM = InteractionTheta(Eint);

  double thetalab = ConvertCMtoLab(Eint,thetaCM);
  G4double En = NeutronEnergy(Eint,thetalab,thetaCM);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sin(thetalab)*cos(phi),sin(thetalab)*sin(phi),cos(thetalab)));
  fParticleGun->SetParticleEnergy(En*MeV);
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName="neutron");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->GeneratePrimaryVertex(anEvent);
  G4ParticleDefinition* Be7def;
  Be7def=G4IonTable::GetIonTable()->GetIon(4,7,excited?0.:0.429*MeV);
  fParticleGun->SetParticleDefinition(Be7def);
  fParticleGun->SetParticleEnergy(0*MeV);
  fParticleGun->GeneratePrimaryVertex(anEvent);//Do the 7Be too (possibly excited)
//  G4cout<<"Neutron: "<<En<<G4endl;
// Make 7Be too - TODO JEB
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



double PrimaryGeneratorAction::InteractionE(double Ep) {
        //Rejection sampling for z_interaction based on XS(Ep) and stopping power
        //Then return Ep for z
        bool trials=true;
        double XSmax = gXS_t->Eval(Einitial);//Max XS is at the highest E
        XSmax = 1000;//Overwrite manually to 1000 mb
        int loopcounter=0;
//	protons_taken=0;
        while(trials) {//Randomly choose a depth into the target (rather than energy to keep same target thickness)
                double trial_z = max_depth*rndm->Rndm();//mm
                //Calc XS at Z
                double energy = Eloss(Ep,trial_z);
                if(energy<=Eth) continue;
                double XS = gXS_t->Eval(energy);
                if(energy<1.925) {//NEED TO ADD IN THE TWO OPTIONS
                        double C0=6;//From Lee and Zhou
                        double A=164.913;//mb MeV/sr
                        double x=C0*sqrt(1-Eth/energy);
                        XS=4.*pi*(A*x)/(energy*(1+x)*(1+x));
                }
                if(energy>2.5 && rndm->Rndm()<gXS_1->Eval(energy)/gXS_0->Eval(energy)) {
                        excited=true;
                }
                else excited=false;
                if(XS>XSmax) {G4cout<<"Max XS is exceeded!!"<<G4endl;}//
                if(XS>XSmax*rndm->Rndm()) {
                        return energy;
                }
                loopcounter++;
                if(loopcounter>1000) {
                        G4cout<<"Rejection sampling not working properly - 1000 samples taken"<<G4endl;
                        return 0;
                }
		double number_density = trial_z*0.1*0.534*6.023e23/6.941; //mm->[cm]*[g/cm3]*[mol/g]=atoms/cm2
		protons_taken += 1/(XSmax*1e-27*number_density);//mb->cm2*atoms/cm2 --> atoms
//		G4cout<<protons_taken<<G4endl;
        }
	return 0;
}

void PrimaryGeneratorAction::makesumgraph() {
//      gXS_t = gXS_0;
        std::ifstream in;
        in.open("XS_ang0.txt");
        double inA0[13],inA1[13],inA2[13],inE[13];
        int count=0;
        while(in.good() && count<13) {
                in>>inE[count]>>inA0[count]>>inA1[count]>>inA2[count];
                count++;
        }
	gA0 = new TGraph(13,inE,inA0);
        gA1 = new TGraph(13,inE,inA1);
        gA2 = new TGraph(13,inE,inA2);
//
  	std::ifstream in2;
        in2.open("XS_ang1.txt");
        double inA0p[2],inA1p[2],inA2p[2],inEp[2];
        count=0;
        while(in2.good() && count<2) {
                double readE,readA0,readA1,readA2;
//              in2>>readE>>readA0>>readA1>>readA2;
                in2>>inEp[count]>>inA0p[count]>>inA1p[count]>>inA2p[count];
                count++;
        }
	gA0p = new TGraph(2,inEp,inA0p);
        gA1p = new TGraph(2,inEp,inA1p);
        gA2p = new TGraph(2,inEp,inA2p);
}

double PrimaryGeneratorAction::Eloss(double Ep, double z) {//Energy of proton of initial energy Ep after z mm in nat Li
        double range=0.0237*Ep*Ep+0.0369*Ep-0.0102;//mm
        if(z>range) return 0;//Fully stopped before Z
        range-=z;
        double Eout=-7.98*range*range+10.0761*range+0.6029;//MeV
        return Eout;
}

double PrimaryGeneratorAction::InteractionTheta(double Ep) {//COM theta
//      return (100+10*rndm->Rndm())*d2r;
        double random_theta = acos(-1+2*rndm->Rndm());
        random_theta = 2.*pi*rndm->Rndm();
        double A[3]={1,0,0};
        bool sample=true;
        double XSmax=3;
        int counter=0;
        if(Ep<1.95) {//Below where we have data it is isotropic
                A[0]=1.;
                A[1]=0.;
                A[2]=0.;
        }
	else {
              	if(!excited) {
                        A[0]=gA0->Eval(Ep);
                        A[1]=gA1->Eval(Ep);
                        A[2]=gA2->Eval(Ep);
                }
                if(excited) {
                        A[0]=gA0p->Eval(Ep);
                        A[1]=gA1p->Eval(Ep);
                        A[2]=gA2p->Eval(Ep);
                }
        }
        while(sample) {
                double XS_sample = XSmax*rndm->Rndm();
                random_theta = 2.*pi*rndm->Rndm();
                double XS=sin(random_theta)*(A[0]*1+A[1]*cos(random_theta)+A[2]*0.5*(3*cos(random_theta)*cos(random_theta)-1));
                if(XS>XSmax) G4cout<<"Sampling error for angle"<<G4endl;
//              cout<<XS<<"\t"<<XSmax<<endl;
                if(XS_sample<XS) {
                        return random_theta;//Isotropic for now
                }
                counter++;
                if(counter>1000) {
                        G4cout<<"Over 1000 samples taken - no solution"<<G4endl;
                        return 0;
                }
        }
}

double PrimaryGeneratorAction::ConvertCMtoLab(double Ep, double thetaCM) {//Convert thetaCM to lab
        double Ex=0;
        if(excited) Ex=0.477;
        double gamma = sqrt(((mp*mn)/(mBe*mLi))*(Ep/(Ep+(Q-Ex)*(1.+mp/mLi))));//Calculate gamma for the conversion to lab COM
        gamma = gamma*((mp+mLi)/(mn+mBe));//Account for change in the CM velocity
        double thetalab = atan2(sin(thetaCM),(cos(thetaCM)+gamma));//
        secondks = false;
        if(gamma*cos(thetaCM)<=-1.) secondks=true;//Neutron is going bac
        return thetalab;
}

double PrimaryGeneratorAction::NeutronEnergy(double Ep, double theta, double thetaCM) {//Get neutron energy for a given Ep and theta
  	double En;
        double Ex=0;
        if(excited) Ex=0.431;//MeV
        double ECM = mLi*Ep/(mLi+mp)+Q-Ex;
        if(ECM<0) {
//                G4cout<<"Below threshold! Something is wrong!"<<G4endl;
                return 0;
        }
	double p_nT = sqrt(2.*ECM*mn*(mBe/(mn+mBe)));
        double phi=2*pi*rndm->Rndm();
        double pn[3]={p_nT*sin(thetaCM)*cos(phi),p_nT*sin(thetaCM)*sin(phi),p_nT*cos(thetaCM)};//COM neutron momentum
        pn[2]+=(mp/(mp+mLi))*sqrt(2.*mp*Ep);//Add the momentum boost for the lab frame
        double En_new = 0;
        for(int i=0;i<3;i++) En_new+=pn[i]*pn[i];
        En_new /= (2.*mn);
        En = En_new;
        theta = acos(pn[2]/p_nT);
        return En;
}


}//End namespace shenanigans :(

