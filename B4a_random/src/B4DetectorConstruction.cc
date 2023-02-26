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
/// \file B4DetectorConstruction.cc
/// \brief Implementation of the B4DetectorConstruction class

#include "B4DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* B4DetectorConstruction::fMagFieldMessenger = nullptr; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::B4DetectorConstruction()
 : G4VUserDetectorConstruction(),
   fHAbsorberPV(nullptr),
   fHGapPV(nullptr),
   fCheckOverlaps(true)
{
  fNModuleX = 10;
  fNModuleY = 10; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::~B4DetectorConstruction()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4DetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::DefineMaterials()
{ 
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_W");
  nistManager->FindOrBuildMaterial("G4_Fe");
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  
  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4DetectorConstruction::DefineVolumes()
{

  // ScECAL geometry parameters
  G4double egapLength = 45.*mm;

  // AHCAL geometry parameters
  G4int nofHLayers = 48;
  G4double habsThickness = 20.*mm;
  G4double hgapThickness = 3.*mm;
  G4double hgapSideLength = 1.*cm;
  auto hcalorSizeZ = (habsThickness+hgapThickness)*nofHLayers;

  // Geometry parameters
  auto calorSizeX = egapLength*fNModuleX*2;
  auto calorSizeY = egapLength*fNModuleY*2;
  G4double IPdistance = 2000.*mm;
  G4double marginZ = 10.*cm;
  auto HcalorCenterZ = (IPdistance-marginZ)/2;

  auto worldSizeX = 1.2*calorSizeX;
  G4cout << "worldSizeX =" << worldSizeX << G4endl;
  auto worldSizeY = 1.2*calorSizeY;
  G4cout << "worldSizeY =" << worldSizeY << G4endl;
  auto worldSizeZ  = IPdistance+hcalorSizeZ+marginZ;
  G4cout << "worldSizeZ =" << worldSizeZ << G4endl;

  fWorldEdgeZ = -worldSizeZ/2;
  fHCalorEdgeZ = IPdistance+fWorldEdgeZ;
  
  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto EabsorberMaterial = G4Material::GetMaterial("G4_W");
  auto EgapMaterial = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  auto HabsorberMaterial = G4Material::GetMaterial("G4_Fe");
  auto HgapMaterial = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  
  if ( ! defaultMaterial || ! EabsorberMaterial || ! EgapMaterial || ! HabsorberMaterial || ! HgapMaterial) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("B4DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }  
   
  //     
  // World
  //
  auto worldS 
    = new G4Box("World",           // its name
                 worldSizeX/2, worldSizeY/2, worldSizeZ/2); // its size
                         
  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name
                                   
  auto worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume                         
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 
  

  //
  //AHCAL
  //
  auto HcalorimeterS
    = new G4Box("AHCAL",      // its name
                 calorSizeX/2, calorSizeY/2, hcalorSizeZ/2); // its size
                         
  auto HcalorLV
    = new G4LogicalVolume(
                 HcalorimeterS,    // its solid
                 defaultMaterial,  // its material
                 "AHCAL");         // its name
                                   
  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(0, 0, HcalorCenterZ),  // at (0,0,EFcalorCenterZ)
                 HcalorLV,         // its logical volume                         
                 "AHCAL",          // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps 

  //
  //AHCAL Layer
  //
  auto HlayerS 
    = new G4Box("HLayer",          // its name
                 calorSizeX/2, calorSizeY/2, (habsThickness+hgapThickness)/2); // its size
                         
  auto HlayerLV
    = new G4LogicalVolume(
                 HlayerS,          // its solid
                 defaultMaterial,  // its material
                 "HLayer");        // its name

  new G4PVReplica(
                 "HLayer",         // its name
                 HlayerLV,         // its logical volume
                 HcalorLV,         // its mother
                 kZAxis,           // axis of replication
                 nofHLayers,       // number of replica
                 (habsThickness+hgapThickness));// witdth of replica

  //
  //AHCAL Absorber
  //
  auto HabsorberS 
    = new G4Box("HAbso",           // its name
                 calorSizeX/2, calorSizeY/2, habsThickness/2); // its size
                         
  auto HabsorberLV
    = new G4LogicalVolume(
                 HabsorberS,       // its solid
                 HabsorberMaterial,// its material
                 "HAbso");         // its name
  
  fHAbsorberPV
    = new G4PVPlacement(
                  0,               // no rotation
                  G4ThreeVector(0, 0, -hgapThickness/2), // its position
                  HabsorberLV,     // its logical volume                         
                  "HAbso",         // its name
                  HlayerLV,        // its mother  volume
                  false,           // no boolean operation
                  0,               // copy number
                  fCheckOverlaps); // checking overlaps

  //                               
  //AHCAL Gap
  //
  G4int hgapnxdiv = static_cast<int>(calorSizeX/hgapSideLength);
  G4int hgapnydiv = static_cast<int>(calorSizeY/hgapSideLength);
  auto HgapS 
    = new G4Box("HGap",             // its name
                 hgapSideLength/2, hgapSideLength/2, hgapThickness/2); // its size
                         
  auto HgapLV
    = new G4LogicalVolume(
                 HgapS,             // its solid
                 HgapMaterial,      // its material
                 "HGap");           // its name
                                   
  for (G4int ix = 0; ix < hgapnxdiv; ++ix) {
    for (G4int iy = 0; iy < hgapnydiv; ++iy) {
      fHGapPV
        = new G4PVPlacement(
                     0,             // no rotation
                     G4ThreeVector(-calorSizeX/2+(ix+0.5)*hgapSideLength,
                                   -calorSizeY/2+(iy+0.5)*hgapSideLength,
                                   habsThickness/2), // its position
                     HgapLV,            // its logical volume
                     "HGap",            // its name
                     HlayerLV,          // its mother  volume
                     false,            // no boolean operation
                     ix*hgapnydiv+iy,         // copy number
                     fCheckOverlaps);  // checking overlaps
    }
  }
  
  //
  // print parameters
  //
  G4cout
    << G4endl 
    << "------------------------------------------------------------" << G4endl
    << "---> The AHCAL is " << nofHLayers << " layers of: [ "
    << habsThickness/mm << "mm of " << HabsorberMaterial->GetName() 
    << " + "
    << hgapThickness/mm << "mm of " << HgapMaterial->GetName() << " ] " << G4endl
    << "--> AHCAL Tile number is " << hgapnxdiv << "," << hgapnydiv << G4endl
    << "------------------------------------------------------------" << G4endl;
  
  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  HcalorLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::ConstructSDandField()
{ 
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
