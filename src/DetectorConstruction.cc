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
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"
//#define USE_CADMESH_TETGEN
#include "CADMesh.hh" //From https://github.com/christopherpoole/CADMesh/blob/master/CADMesh.hh
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  //
  G4double env_sizeXY = 200*cm, env_sizeZ = 200*um;//Titanium target size

  G4Material* lithium = nist->FindOrBuildMaterial("G4_Li");
  G4Material* Si = nist->FindOrBuildMaterial("G4_Si");
  G4Material* copper = nist->FindOrBuildMaterial("G4_Cu");
  G4Material* aluminium = nist->FindOrBuildMaterial("G4_Al");
  G4Material* iron = nist->FindOrBuildMaterial("G4_Fe");
  G4Material* cobalt = nist->FindOrBuildMaterial("G4_Co");
  G4Material* HPGe = nist->FindOrBuildMaterial("G4_Ge");
  G4Material* Ti = nist->FindOrBuildMaterial("G4_Ti");
  G4Material* lead = nist->FindOrBuildMaterial("G4_Pb");
  G4Material* molybdenum = nist->FindOrBuildMaterial("G4_Mo");
  G4Material* concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
  G4Material* paraffin = nist->FindOrBuildMaterial("G4_PARAFFIN");
  G4Material* graphite = nist->FindOrBuildMaterial("G4_GRAPHITE");
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXZ = 500.*m;
  G4double world_sizeY  = 100.*m;
  G4Material* vacuum =
    new G4Material("Vacuum",      //Name as String
                   1,             //Atomic Number,  in this case we use 1 for hydrogen  
                   1.008*g/mole,  //Mass per Mole "Atomic Weight"  1.008*g/mole for Hydoren 
                   1.e-25*g/cm3,  //Density of Vaccuum  *Cant be Zero, Must be small insted 
                   kStateGas,     //kStateGas for Gas
                   2.73*kelvin,   //Temperature for gas
                   1.e-25*g/cm3); //Pressure for Vaccum
  
  auto solidWorld = new G4Box("World",                           // its name
    0.5 * world_sizeXZ, 0.5 * world_sizeY, 0.5 * world_sizeXZ);  // its size

  auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
    air,                                       // its material
    "World");                                        // its name

  auto physWorld = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                           // at (0,0,0)
    logicWorld,                                // its logical volume
    "World",                                   // its name
    nullptr,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    checkOverlaps);                            // overlaps checking

  //
  // Lithium
  //
  auto solidEnv = new G4Box("Lithium",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);  // its size

  auto logicEnv = new G4LogicalVolume(solidEnv,  // its solid
    lithium,                                     // its material
    "Lithium");                                 // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    logicEnv,                 // its logical volume
    "Lithium",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
  //
  // Copper
  //
  G4double back_sizeZ=3.*mm;

  auto solidback = new G4Box("Copper",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5*back_sizeZ);  // its size

  auto logicback = new G4LogicalVolume(solidback,  // its solid
    copper,                                     // its material
    "Copper");                                 // its name

  auto coppercolour = new G4VisAttributes();
  coppercolour->SetColour(1,0.2,0.2);

  logicback->SetVisAttributes(coppercolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.5*env_sizeZ+0.5*back_sizeZ),          // at (0,0,0)
    logicback,                 // its logical volume
    "Copper",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

  //
  // Graphite
  //

  auto solidgraphite = new G4Box("Graphite",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5*7*mm);  // its size

  auto logicgraphite = new G4LogicalVolume(solidback,  // its solid
    graphite,                                     // its material
    "Graphite");                                 // its name

  auto graphitecolour = new G4VisAttributes();
  graphitecolour->SetColour(0.2,0.2,0.2);

  logicgraphite->SetVisAttributes(graphitecolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.5*env_sizeZ+0.5*back_sizeZ+7*mm),          // at (0,0,0)
    logicgraphite,                 // its logical volume
    "Graphite",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
  //
  // Titanium
  //
  auto solidtitanium = new G4Box("titanium",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5*6.*mm);  // its size

  auto logictitanium = new G4LogicalVolume(solidtitanium,  // its solid
    Ti,                                     // its material
    "titanium");                                 // its name

  auto titaniumcolour = new G4VisAttributes();
  titaniumcolour->SetColour(0.5,0.5,0.2);

  logictitanium->SetVisAttributes(titaniumcolour);

/*  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,42*mm-0.5*6*mm),          // Back edge at 42 mm due to 6 mm thickness of Ti
    logictitanium,                 // its logical volume
    "titanium",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/

// Forward Scatterer

  auto solidscatterer = new G4Box("scatterer",0.5*10*cm,0.5*10*cm,0.5*1*mm);
  auto logicscatterer = new G4LogicalVolume(solidscatterer,paraffin, "logicscatterer");
  new G4PVPlacement(0,G4ThreeVector(0,0,0.046*m+17.0*mm),logicscatterer,"logicscatterer",logicWorld,false,0,checkOverlaps);

// Backward Scatterer

  auto solidscattererb = new G4Box("scattererb",0.5*10*cm,0.5*10*cm,0.5*5*mm);
  auto logicscattererb = new G4LogicalVolume(solidscattererb,graphite, "logicscattererb");
  new G4PVPlacement(0,G4ThreeVector(0,0,0.046*m+20*mm),logicscattererb,"logicscattererb",logicWorld,false,0,checkOverlaps);

  //
  // Target space
  //

  auto solidtarget = new G4Box("target",                    // its name
    0.5 * 5 *cm, 0.5 * 5*cm, 0.5*1*mm);  // its size

//  auto solidtarget = new G4Tubs("target", 0*mm, 35*mm, 10*mm, 0*deg, 360*deg);


  auto logictarget = new G4LogicalVolume(solidtarget, 
    cobalt,                                     // its material
    "logictarget");                                 // its name

  auto targetcolour = new G4VisAttributes();
  targetcolour->SetColour(0.5,0.2,0.8);

  logictarget->SetVisAttributes(targetcolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+10*mm+2.01*mm),          // at (0,0,0)
    logictarget,                 // its logical volume
    "logictarget",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

  fScoringVolume = logictarget;

 // Activation foil
   
  auto foil1 = new G4Box("foil1",                    // its name
    0.5 * 12 *cm, 0.5 * 12*cm, 0.5*0.0127*mm);  // its size
  auto logicfoil1 = new G4LogicalVolume(foil1,  // its solid
    molybdenum,                                     // its material
    "foil1");                                 // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+10*mm+3*mm),          // at (0,0,0)
    logicfoil1,                 // its logical volume
    "logicfoil1",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking


// Graphite
/*
  auto solidgraphite = new G4Box("graphite",                    // its name
    0.5 * 50 *cm, 0.5 * 50*cm, 0.5*7*mm);  // its size

  auto logicgraphite = new G4LogicalVolume(solidgraphite,  // its solid
    graphite,                                     // its material
    "logicgraphite");                                 // its name

  auto graphitecolour = new G4VisAttributes();
  graphitecolour->SetColour(0.2,0.1,0.1);

  logicgraphite->SetVisAttributes(graphitecolour);
*/
/*
  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.0066*m),          // at (0,0,0)
    logicgraphite,                 // its logical volume
    "logicgraphite",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/

// Backing Pb
/*
  auto solidlead = new G4Box("lead",                    // its name
    0.5 * 50 *cm, 0.5 * 50*cm, 0.5*50*mm);  // its size

  auto logiclead = new G4LogicalVolume(solidlead,  // its solid
    lead,                                     // its material
    "logiclead");                                 // its name


  logiclead->SetVisAttributes(leadcolour);
  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+100*mm+20*mm),          // at (0,0,0)
    logiclead,                 // its logical volume
    "logiclead",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/
// Pb shield
  auto leadcolour = new G4VisAttributes();
  leadcolour->SetColour(0.1,0.6,0.8);

  auto leadouter = new G4Box("leadouter",0.5*200*cm,0.5*200*cm,0.5*50*mm);
  auto leadinner = new G4Box("leadinner",0.5*10*cm,0.5*10*cm,0.5*50*mm);

  auto solidleadshield = new G4SubtractionSolid("solidleadshield",leadouter,leadinner,0,G4ThreeVector(0,0,0));

  auto logicleadshield = new G4LogicalVolume(solidleadshield,  // its solid
    lead,                                     // its material
    "logicleadshield");                                 // its name

  logicleadshield->SetVisAttributes(leadcolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+25*mm),          // at (0,0,0)
    logicleadshield,                 // its logical volume
    "logicleadshield",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

// Scattering parafin
/*
  auto solidscatter = new G4Box("scatter",                    // its name
    0.5 * 50 *cm, 0.5 * 50*cm, 0.5*300*mm);  // its size

  auto logicscatter = new G4LogicalVolume(solidscatter,  // its solid
    paraffin,                                     // its material
    "logicscatter");                                 // its name

  auto scattercolour = new G4VisAttributes();
  scattercolour->SetColour(0.7,0.2,0.8);

  logicscatter->SetVisAttributes(scattercolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+150*mm+0.5*300*mm),          // at (0,0,0)
    logicscatter,                 // its logical volume
    "logicscatter",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/
// Ti wall

  auto solidTiwall = new G4Box("Tiwall",                    // its name
    0.5 * 50 *cm, 0.5 * 50 * cm, 0.5*6*mm);  // its size

  auto logictiwall = new G4LogicalVolume(solidTiwall,  // its solid
    Ti,                                     // its material
    "logictiwall");                                 // its name

  auto tiwallcolour = new G4VisAttributes();
  tiwallcolour->SetColour(0.7,0.8,0.8);

  logictiwall->SetVisAttributes(tiwallcolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m-6*mm),          // at (0,0,0)
    logictiwall,                 // its logical volume
    "logictiwall",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

  //
  //always return the physical World
  //
  G4double beamheight = 1*m;//For now
/*
// Floor
  auto solidfloor = new G4Box("Floor",                    // its name
    0.5 * 5 *m, 0.5 * 50*mm, 0.5*5*m);  // its size

  auto logicfloor = new G4LogicalVolume(solidfloor,  // its solid
    concrete,                                     // its material
    "logicfloor");                                 // its name

  auto floorcolour = new G4VisAttributes();
  floorcolour->SetColour(0.6,0.5,0.5);

  logicfloor->SetVisAttributes(floorcolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,-beamheight,0.0),          // at (0,0,0)
    logicfloor,                 // its logical volume
    "logicfloor",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/
/// Import CAD
  G4RotationMatrix* Rotation = new G4RotationMatrix();
  Rotation->rotateX(0*deg);
  Rotation->rotateY(90*deg);
  Rotation->rotateZ(0*deg);
  G4RotationMatrix* Rotationroom = new G4RotationMatrix();
  Rotationroom->rotateX(90*deg);
  Rotationroom->rotateY(0*deg);
  Rotationroom->rotateZ(90*deg);
  G4ThreeVector targetoffset= G4ThreeVector(-13.465*m,-1.18*m,-23.6*m);//-13.5 m,-1.18 m,
  G4ThreeVector roomoffset= G4ThreeVector(-13.465*m+1.6*m,-1.18*m,-23.6*m);//-13.5 m,-1.18 m,
//  G4ThreeVector roomoffset= G4ThreeVector(13.465*m,-1.18*m,23.6*m);//-13.5 m,-1.18 m,
  G4cout<<"Importing room .stl file"<<G4endl;

// Tetrahedral mesh route -- not working with overlapping points
/*
  auto mesh = CADMesh::TetrahedralMesh::FromSTL("standard_concrete.stl");
  mesh->SetMaterial(concrete);
  auto mesh_assembly = mesh->GetAssembly();
  mesh_assembly->MakeImprint(logicWorld,roomoffset,Rotationroom);
*/

  auto mesh = CADMesh::TessellatedMesh::FromSTL("concrete.stl");
  G4VSolid *solid = mesh->GetSolid();
  auto solid_logical = new G4LogicalVolume(solid, concrete, "solid_logical", 0, 0 ,0);
  new G4PVPlacement(Rotationroom,roomoffset,solid_logical,"physical_solid",logicWorld,false, checkOverlaps );
  auto concretecolour = new G4VisAttributes();
  concretecolour->SetColour(0.6,0.5,0.5,0.5);
  solid_logical->SetVisAttributes(concretecolour);



/// Import CAD of target station
/*
  G4cout<<"Importing target .stl file"<<G4endl;
  auto mesht = CADMesh::TessellatedMesh::FromSTL("target_station2.stl");
  G4VSolid *solidt = mesht->GetSolid();
  auto solid_logicalt = new G4LogicalVolume(solidt, Ti, "solid_logicalt", 0, 0 ,0);
//  new G4PVPlacement(Rotation,targetoffset,solid_logicalt,"physical_solidt",logicWorld,false, checkOverlaps );
  solid_logicalt->SetVisAttributes(targetcolour);
*/
//  auto targetcolour = new G4VisAttributes();
//  targetcolour->SetColour(0.6,0.5,0.5);

// Detectors
  auto gammadet = new G4Tubs("gammadet", 0*mm, 2*2.54*cm, 2*2.54*cm, 0*deg, 360*deg);
  auto gamma_logical = new G4LogicalVolume(gammadet, HPGe, "gamma_logical", 0, 0, 0);
//  new G4PVPlacement(0,G4ThreeVector(-0.5*m,-0.5*m,1*m), gamma_logical, "gamma_phys", logicWorld, false, checkOverlaps);
  G4cout<<"Detector construction complete"<<G4endl;
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
