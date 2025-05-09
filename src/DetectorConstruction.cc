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
#include "G4GDMLParser.hh"
namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
//  G4GDMLParser fParser;
//  fParser.Read("room.gdml");
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  //
  G4double env_sizeXY = 110*cm, env_sizeZ = 300*um;//Target size

  G4Material* lithium = nist->FindOrBuildMaterial("G4_Li");
  G4Material* Si = nist->FindOrBuildMaterial("G4_Si");
  G4Material* copper = nist->FindOrBuildMaterial("G4_Cu");
  G4Material* aluminium = nist->FindOrBuildMaterial("G4_Al");
  G4Material* berylium = nist->FindOrBuildMaterial("G4_Be");
  G4Material* iron = nist->FindOrBuildMaterial("G4_Fe");
  G4Material* cobalt = nist->FindOrBuildMaterial("G4_Co");
  G4Material* HPGe = nist->FindOrBuildMaterial("G4_Ge");
  G4Material* Ti = nist->FindOrBuildMaterial("G4_Ti");
  G4Material* lead = nist->FindOrBuildMaterial("G4_Pb");
  G4Material* cadmium = nist->FindOrBuildMaterial("G4_Cd");
  G4Material* molybdenum = nist->FindOrBuildMaterial("G4_Mo");
  G4Material* concrete = nist->FindOrBuildMaterial("G4_CONCRETE");
  G4Material* paraffin = nist->FindOrBuildMaterial("G4_PARAFFIN");
  G4Material* graphite = nist->FindOrBuildMaterial("G4_GRAPHITE");
  G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* water = nist->FindOrBuildMaterial("G4_WATER");
  G4Material* poly = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
  G4Material* lowpoly = new G4Material("lowpoly",0.09*g/cm3,1);
  lowpoly->AddMaterial(poly,100*perCent);
  G4Material* boron = nist->FindOrBuildMaterial("G4_B");
  G4Material* bpoly =  new G4Material("BoratedPoly",1.04*g/cm3,2);
  bpoly->AddMaterial(poly, 95*perCent);
  bpoly->AddMaterial(boron, 5*perCent);
  G4Material* elCs = nist->FindOrBuildMaterial("G4_Cs");
  G4Material* elLa = nist->FindOrBuildMaterial("G4_La");
  G4Material* elBr = nist->FindOrBuildMaterial("G4_Br");
  G4Material* elCl = nist->FindOrBuildMaterial("G4_Cl");
  G4int ncomp, nAtoms, z, a;
  G4double abundance;
  G4Material* enrichedLi = new G4Material("enrichedLi",0.534*g/cm3,ncomp=2);
//  G4Element *Li6 = new G4Element("Li6",z=3,a=6,6.015122*g/mole);
//  G4Element *Li7 = new G4Element("Li7",z=3,a=7,7.016004*g/mole);
  G4Element *Li6 = new G4Element("Li6","Li",3,6.015122*g/mole);
  G4Element *Li7 = new G4Element("Li7","Li",3,7.016004*g/mole);
  G4cout<<"Enriching lithium"<<G4endl;
  enrichedLi->AddElement(Li6,abundance=95*perCent);
  enrichedLi->AddElement(Li7,abundance=5*perCent);
  G4Material* copperwater = new G4Material("copperwater",8.164*g/cm2,2);//10% water (by volume) inside 90% copper
  copperwater->AddMaterial(copper,0.988);
  copperwater->AddMaterial(water,0.012);//By weight
  G4Material* CLLBC = new G4Material("CLLBC",4.08*g/cm3,5);
  G4double wtotal = (2*132.9+1*6.941+1*138.9+4.8*79.904+1.2*35.453);
  G4double wCs =(2*132.9)/wtotal;//Weightsfraction
  G4double wLi =(6.941)/wtotal;
  G4double wLa =(138.9)/wtotal;
  G4double wBr =(4.8*79.904)/wtotal;
  G4double wCl =(1.2*35.453)/wtotal;
  CLLBC->AddMaterial(elCs, wCs);
  CLLBC->AddMaterial(enrichedLi, wLi);
  CLLBC->AddMaterial(elLa, wLa);
  CLLBC->AddMaterial(elBr, wBr);
  CLLBC->AddMaterial(elCl, wCl);
  // Option to switch on/off checking of volumes overlaps
  //
  G4cout<<"Built CLLBC"<<G4endl;
  G4bool checkOverlaps = true;

  //
  // World
  //
     G4double world_sizeXZ = 50.*m;
     G4double world_sizeY  = 50.*m;
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

//  G4VPhysicalVolume *physWorld = fParser.GetWorldVolume();
//  G4cout<<"Return GDML world"<<G4endl;

     auto physWorld = new G4PVPlacement(nullptr,  // no rotation
        G4ThreeVector(),                           // at (0,0,0)
       logicWorld,                                // its logical volume
       "World",                                   // its name
       nullptr,                                   // its mother  volume
       false,                                     // no boolean operation
       0,                                         // copy number
       checkOverlaps);                            // overlaps checking

  G4double vertical_offset = -1.22*m+1.13*m+1.11*m*0.5;//1.22 to floor, 1.13 to lip of Ti, 1.11*0.5 to mid
  G4RotationMatrix* Rotationroom = new G4RotationMatrix();
  Rotationroom->rotateX(90*deg);
  Rotationroom->rotateZ(0*deg);

  G4ThreeVector targetoffset= G4ThreeVector(-13.465*m,-1.18*m,-23.6*m);//-13.5 m,-1.18 m,
  G4ThreeVector roomoffset= G4ThreeVector(-13.465*m+1.6*m,-1.18*m,-23.6*m);//-13.5 m,-1.18 m,
  G4RotationMatrix* Rotation = new G4RotationMatrix();
  Rotation->rotateX(0*deg);
  Rotation->rotateY(90*deg);
  Rotation->rotateZ(0*deg);

  G4cout<<"Importing room .stl files"<<G4endl;
  G4ThreeVector CADoffset = G4ThreeVector(0*m,43.5*cm,0*cm);
  auto mesh_bwalls = CADMesh::TessellatedMesh::FromSTL("CAD/borated_walls.stl");
  G4VSolid *solid_bwalls = mesh_bwalls->GetSolid();
  auto solid_boronwalls = new G4LogicalVolume(solid_bwalls,bpoly,"solid_boronwalls",0,0,0);
  auto blackcolour = new G4VisAttributes();
  blackcolour->SetColour(0.1,0.1,0.1);
  solid_boronwalls->SetVisAttributes(blackcolour);
  new G4PVPlacement(Rotationroom,CADoffset,solid_boronwalls,"physical_boronwalls",logicWorld,false,checkOverlaps);

  auto translucentcolour = new G4VisAttributes();
  translucentcolour->SetColour(0.9,0.9,0.9,0.1);

  auto mesh_walls = CADMesh::TessellatedMesh::FromSTL("CAD/concrete_walls.stl");
  G4VSolid *solid_cwalls = mesh_walls->GetSolid();
  auto solid_walls = new G4LogicalVolume(solid_cwalls,concrete,"solid_walls",0,0,0);
  solid_walls->SetVisAttributes(translucentcolour);
  new G4PVPlacement(Rotationroom,CADoffset,solid_walls,"physical_walls",logicWorld,false,checkOverlaps);

  auto mesh_ceiling = CADMesh::TessellatedMesh::FromSTL("CAD/ceiling.stl");
  G4VSolid *solid_cceiling = mesh_ceiling->GetSolid();
  auto solid_ceiling = new G4LogicalVolume(solid_cceiling,concrete,"solid_ceiling",0,0,0);
  solid_ceiling->SetVisAttributes(translucentcolour);
  new G4PVPlacement(Rotationroom,CADoffset,solid_ceiling,"physical_ceiling",logicWorld,false,checkOverlaps);

  auto mesh_floor = CADMesh::TessellatedMesh::FromSTL("CAD/floor.stl");
  G4VSolid *solid_cfloor = mesh_floor->GetSolid();
  auto solid_floor = new G4LogicalVolume(solid_cfloor,concrete,"solid_floor",0,0,0);
  solid_floor->SetVisAttributes(translucentcolour);
  new G4PVPlacement(Rotationroom,CADoffset,solid_floor,"physical_floor",logicWorld,false,checkOverlaps);

  auto leadcolour = new G4VisAttributes();
  leadcolour->SetColour(0.3,0.3,0.5,0.9);


  auto mesh_leadshield = CADMesh::TessellatedMesh::FromSTL("CAD/lead.stl");
  G4VSolid *solid_cleadshield = mesh_leadshield->GetSolid();
  auto solid_leadshield = new G4LogicalVolume(solid_cleadshield,lead,"solid_leadshield",0,0,0);
  solid_leadshield->SetVisAttributes(leadcolour);
  new G4PVPlacement(Rotationroom,CADoffset,solid_leadshield,"physical_leadshield",logicWorld,false,checkOverlaps);

  auto mesh_concretestack = CADMesh::TessellatedMesh::FromSTL("CAD/concrete_stack.stl");
  G4VSolid *solid_cconcretestack = mesh_concretestack->GetSolid();
  auto solid_concretestack = new G4LogicalVolume(solid_cconcretestack,concrete,"solid_concretestack",0,0,0);
  new G4PVPlacement(Rotationroom,CADoffset,solid_concretestack,"physical_concretestack",logicWorld,false,checkOverlaps);

  auto mesh_blocker_1 = CADMesh::TessellatedMesh::FromSTL("CAD/blocker_1.stl");
  G4VSolid *solid_cblocker_1 = mesh_blocker_1->GetSolid();
  auto solid_blocker_1 = new G4LogicalVolume(solid_cblocker_1,concrete,"solid_blocker_1",0,0,0);
  new G4PVPlacement(Rotationroom,CADoffset,solid_blocker_1,"physical_blocker_1",logicWorld,false,checkOverlaps);

  auto mesh_blocker_2 = CADMesh::TessellatedMesh::FromSTL("CAD/blocker_2.stl");
  G4VSolid *solid_cblocker_2 = mesh_blocker_2->GetSolid();
  auto solid_blocker_2 = new G4LogicalVolume(solid_cblocker_2,concrete,"solid_blocker_2",0,0,0);
  new G4PVPlacement(Rotationroom,CADoffset,solid_blocker_2,"physical_blocker_2",logicWorld,false,checkOverlaps);

  auto mesh_blocker_3 = CADMesh::TessellatedMesh::FromSTL("CAD/blocker_3.stl");
  G4VSolid *solid_cblocker_3 = mesh_blocker_3->GetSolid();
  auto solid_blocker_3 = new G4LogicalVolume(solid_cblocker_3,concrete,"solid_blocker_3",0,0,0);
  new G4PVPlacement(Rotationroom,CADoffset,solid_blocker_3,"physical_blocker_3",logicWorld,false,checkOverlaps);

  auto mesh_blocker_4 = CADMesh::TessellatedMesh::FromSTL("CAD/blocker_4.stl");
  G4VSolid *solid_cblocker_4 = mesh_blocker_4->GetSolid();
  auto solid_blocker_4 = new G4LogicalVolume(solid_cblocker_4,concrete,"solid_blocker_4",0,0,0);
  new G4PVPlacement(Rotationroom,CADoffset,solid_blocker_4,"physical_blocker_4",logicWorld,false,checkOverlaps);
  //
  // Lithium
  //
/*								3mm
|<---->|        |<-->|  |<-->|                          |<------------>|
+------+        +----+  +----+                          +-------------+
|  Ti  |        | G2 |  | G1 |                          | Cu petal w/ |
|6 mm  |        |    |  |    |                          | 10% water   |
|      |        |    |  |    |                          | by volume   |
+------+        +----+  +----+                          +-------------+

<------------------->                                  <------------->
     Layer stack 1                                          Layer stack 2

<------------------------------------------------------------->
*/
  G4double li_thickness = 300*um;
  G4double cu_thickness = 3*mm;
  G4double li_to_cu_gap = 20.5*mm;
  G4double graphite1_thickness = 3.5*mm;
  G4double g1_to_g2_gap = 4.5*mm;
  G4double graphite2_thickness = 3.5*mm;
  G4double g2_to_ti_gap = 4*mm;
  G4double ti_thickness = 6*mm;
  auto solidEnv = new G4Box("Lithium",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * li_thickness);  // its size

  auto logicEnv = new G4LogicalVolume(solidEnv,  // its solid
    lithium,                                     // its material
    "Lithium");                                 // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0*m,vertical_offset,0*m),          // at (0,0,0) is the centre of the lithium petal
    logicEnv,                 // its logical volume
    "Lithium",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
  //
  // Copper
  //

  auto solidback = new G4Box("Copper",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5*cu_thickness);  // its size

  auto logicback = new G4LogicalVolume(solidback,  // its solid
    copperwater,                                     // its material
    "Copper");                                 // its name

  auto coppercolour = new G4VisAttributes();
  coppercolour->SetColour(1,0.2,0.2);

  logicback->SetVisAttributes(coppercolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,vertical_offset,0.5*cu_thickness+li_thickness),          // at (0,0,0)
    logicback,                 // its logical volume
    "Copper",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

  //
  // Graphite x2
  //

  auto solidgraphite1 = new G4Box("Graphite1",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5*graphite1_thickness);  // its size

  auto logicgraphite1 = new G4LogicalVolume(solidgraphite1,  // its solid
    graphite,                                     // its material
    "Graphite1");                                 // its name

  auto solidgraphite2 = new G4Box("Graphite2",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5*graphite2_thickness);  // its size


  auto logicgraphite2 = new G4LogicalVolume(solidgraphite2,  // its solid
    graphite,                                     // its material
    "Graphite2");                                 // its name

  auto graphitecolour = new G4VisAttributes();
  graphitecolour->SetColour(0.2,0.2,0.2);

  logicgraphite1->SetVisAttributes(graphitecolour);
  logicgraphite2->SetVisAttributes(graphitecolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,vertical_offset,li_thickness+cu_thickness+li_to_cu_gap+0.5*graphite1_thickness),          // at (0,0,0)
    logicgraphite1,                 // its logical volume
    "Graphite1",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking


  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,vertical_offset,li_thickness+cu_thickness+li_to_cu_gap+graphite1_thickness+g1_to_g2_gap+0.5*graphite2_thickness),          // at (0,0,0)
    logicgraphite2,                 // its logical volume
    "Graphite2",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
  //
  // Titanium flange
  //
  auto solidtitanium = new G4Box("titanium",                    // its name
    0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5*ti_thickness);  // its size

  auto logictitanium = new G4LogicalVolume(solidtitanium,  // its solid
    Ti,                                     // its material
    "titanium");                                 // its name

  auto titaniumcolour = new G4VisAttributes();
  titaniumcolour->SetColour(0.5,0.5,0.2);

  logictitanium->SetVisAttributes(titaniumcolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,vertical_offset,li_thickness+cu_thickness+li_to_cu_gap+graphite1_thickness+g1_to_g2_gap+graphite2_thickness+g2_to_ti_gap+0.5*ti_thickness),
    logictitanium,                 // its logical volume
    "titanium",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

//Back side
//  new G4PVPlacement(nullptr,  // no rotation
//    G4ThreeVector(0,vertical_offset,-1*cm+0.5*6*mm),          // Back edge at 42 mm due to 6 mm thickness of Ti
//    logictitanium,                 // its logical volume
//    "titanium",               // its name
//    logicWorld,               // its mother  volume
//    false,                    // no boolean operation
//    0,                        // copy number
//    checkOverlaps);           // overlaps checking

//Optional scatterers ------------------------->>>
/*
// Forward Scatterer

  auto solidscatterer = new G4Box("scatterer",0.5*10*cm,0.5*10*cm,0.5*5*mm);
  auto logicscatterer = new G4LogicalVolume(solidscatterer,cadmium, "logic_cadmium");
//  new G4PVPlacement(0,G4ThreeVector(0,0,0.046*m+5*mm),logicscatterer,"logic_cadmium",logicWorld,false,0,checkOverlaps);

// Backward Scatterer

  auto solidscattererb = new G4Box("scattererb",0.5*10*cm,0.5*10*cm,0.5*5*mm);
  auto logicscattererb = new G4LogicalVolume(solidscattererb,cadmium, "logicscattererb");
//  new G4PVPlacement(0,G4ThreeVector(0,0,0.046*m+15*mm),logicscattererb,"logicscattererb",logicWorld,false,0,checkOverlaps);

  auto solidscattererc = new G4Box("scattererc",0.5*10*cm,0.5*10*cm,0.5*25*mm);
  auto logicscattererc = new G4LogicalVolume(solidscattererc,bpoly, "logicscattererc");
//  new G4PVPlacement(0,G4ThreeVector(0,0,0.046*m+55*mm+25*mm),logicscattererc,"logicscattererc",logicWorld,false,0,checkOverlaps);
*/
  //
  // Target space
  //
  G4double foil_size = 50*mm;
  G4double foil_thickness = 1*mm;
  G4double ti_to_foil_gap = 1*mm;
  G4double outer_distance = li_thickness+cu_thickness+li_to_cu_gap+graphite1_thickness+g1_to_g2_gap+graphite2_thickness+g2_to_ti_gap+ti_thickness;
  auto solidtarget = new G4Box("target",                    // its name
    0.5 * foil_size, 0.5 * foil_size, 0.5*foil_thickness*mm);  // its size

//  auto solidtarget = new G4Tubs("target", 0*mm, 6*mm, 0.14*mm, 0*deg, 360*deg);//Gold


  auto logictarget = new G4LogicalVolume(solidtarget, 
    iron,                                     // its material
    "logictarget");                                 // its name

  auto targetcolour = new G4VisAttributes();
  targetcolour->SetColour(0.5,0.2,0.8);

  logictarget->SetVisAttributes(targetcolour);


  G4cout<<"Target placed at "<<(outer_distance+ti_to_foil_gap+0.5*foil_thickness)/mm<<" mm from target"<<G4endl;
  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,outer_distance+ti_to_foil_gap+0.5*foil_thickness),          // at (0,0,0)
    logictarget,                 // its logical volume
    "logictarget",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking

  fScoringVolume = logictarget;
//Optional CLLBC detectors
/*
  // CLLBC

  auto solidCLLBC = new G4Tubs("solidCLLBC", 0*mm, 1*2.54*cm,1*2.54*cm , 0*deg, 360*deg);//pname, rmin, rmax, dz (half lenght), start phi, segment angle phi
  auto logicCLLBC = new G4LogicalVolume(solidCLLBC,CLLBC,"logicCLLBC");
//  new G4PVPlacement(nullptr, G4ThreeVector(0,0,0.1*m), logicCLLBC, "CLLBC", logicWorld, false, 0, checkOverlaps); //CLLBC 1 metre from beam
*/
 // Activation foils
   
  auto foil1 = new G4Box("foil1",                    // its name
    0.5 * 5 *cm, 0.5 * 5*cm, 0.5*1*mm);  // its size
  auto logicfoil1 = new G4LogicalVolume(foil1,  // its solid
    iron,                                     // its material
    "logicfoil");                                 // its name

  auto foil2 = new G4Box("foil2",                    // its name
    0.5 * 5 *cm, 0.5 * 5*cm, 0.5*1*mm);  // its size

  auto logicbacking = new G4LogicalVolume(foil2,  // its solid
    poly,                                     // its material
    "backing");                                 // its name

/*

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+9*mm),          // at (0,0,0)
    logicfoil1,                 // its logical volume
    "logicfoil1",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+0.501*mm),          // at (0,0,0)
    logicbacking,                 // its logical volume
    "backing",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/
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
//  auto leadcolour = new G4VisAttributes();
//  leadcolour->SetColour(0.1,0.6,0.8);
/*
  auto leadouter = new G4Box("leadouter",0.5*200*cm,0.5*200*cm,0.5*100*mm);
  auto leadinner = new G4Box("leadinner",0.5*10*cm,0.5*10*cm,0.5*100*mm);

  auto solidleadshield = new G4SubtractionSolid("solidleadshield",leadouter,leadinner,0,G4ThreeVector(0,0,0));

  auto logicleadshield = new G4LogicalVolume(solidleadshield,  // its solid
    lead,                                     // its material
    "logicleadshield");                                 // its name

  logicleadshield->SetVisAttributes(leadcolour);

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(0,0,0.046*m+50*mm),          // at (0,0,0)
    logicleadshield,                 // its logical volume
    "logicleadshield",               // its name
    logicWorld,               // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    checkOverlaps);           // overlaps checking
*/
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
  G4double beamheight = 1.21*m;//For now
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
//  G4ThreeVector roomoffset= G4ThreeVector(13.465*m,-1.18*m,23.6*m);//-13.5 m,-1.18 m,

// Tetrahedral mesh route -- not working with overlapping points
/*
  auto mesh = CADMesh::TetrahedralMesh::FromSTL("standard_concrete.stl");
  mesh->SetMaterial(concrete);
  auto mesh_assembly = mesh->GetAssembly();
  mesh_assembly->MakeImprint(logicWorld,roomoffset,Rotationroom);
*/
/*
  auto mesh = CADMesh::TessellatedMesh::FromSTL("concrete.stl");
  G4VSolid *solid = mesh->GetSolid();
  auto solid_logical = new G4LogicalVolume(solid, concrete, "solid_room", 0, 0 ,0);
  new G4PVPlacement(Rotationroom,roomoffset,solid_logical,"physical_room",logicWorld,false, checkOverlaps );
  auto concretecolour = new G4VisAttributes();
  concretecolour->SetColour(0.6,0.5,0.5,0.5);
  solid_logical->SetVisAttributes(concretecolour);
*/
/*
  auto concrete_floor = new G4Box("floor",5*0.5*m,0.5*m,5*0.5*m);
  auto solid_concrete_floor = new G4LogicalVolume(concrete_floor, concrete, "concrete_floor", 0, 0 , 0);
  new G4PVPlacement(nullptr, G4ThreeVector(-1*m, -1.22*m-0.5*m,2*m), solid_concrete_floor, "solid_floor", logicWorld, false, checkOverlaps);

  auto concrete_ceil = new G4Box("ceil",5*0.5*m,0.5*m,5*0.5*m);
  auto solid_concrete_ceil = new G4LogicalVolume(concrete_ceil, concrete, "concrete_ceil", 0, 0 , 0);
  new G4PVPlacement(nullptr, G4ThreeVector(-1*m, -1.22*m+0.5*m+3*m,2*m), solid_concrete_ceil, "solid_ceil", logicWorld, false, checkOverlaps);

  auto concrete_back = new G4Box("back",5*0.5*m,5*0.5*m,0.5*m);
  auto solid_concrete_back = new G4LogicalVolume(concrete_back, concrete, "concrete_back", 0, 0 , 0);
  new G4PVPlacement(nullptr, G4ThreeVector(-1*m, -1.22*m+1.5*m,-1*m), solid_concrete_back, "solid_back", logicWorld, false, checkOverlaps);

  auto concrete_forw = new G4Box("forw",5*0.5*m,5*0.5*m,0.5*m);
  auto solid_concrete_forw = new G4LogicalVolume(concrete_forw, concrete, "concrete_forw", 0, 0 , 0);
  new G4PVPlacement(nullptr, G4ThreeVector(-1*m, -1.22*m+1.5*m,-1*m+5*m), solid_concrete_forw, "solid_forw", logicWorld, false, checkOverlaps);

  auto concrete_far = new G4Box("far",0.5*m,5*0.5*m,5*0.5*m);
  auto solid_concrete_far = new G4LogicalVolume(concrete_far, concrete, "concrete_far", 0, 0 , 0);
  new G4PVPlacement(nullptr, G4ThreeVector(1.5*m+0.5*m, -1.22*m+2.5*m,-1*m+2.5*m), solid_concrete_far, "solid_far", logicWorld, false, checkOverlaps);



  auto concrete_blocker = new G4Box("blocker",0.1*m,1*m,1*m);
  auto solid_concrete_blocker = new G4LogicalVolume(concrete_blocker, concrete, "concrete_blocker", 0, 0 , 0);
  new G4PVPlacement(nullptr, G4ThreeVector(-1.5*m, -1.22*m+0.5*m,0*m), solid_concrete_blocker, "solid_blocker1", logicWorld, false, checkOverlaps);
  new G4PVPlacement(nullptr, G4ThreeVector(1.5*m, -1.22*m+0.5*m,0*m), solid_concrete_blocker, "solid_blocker2", logicWorld, false, checkOverlaps);
*/

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
