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
/// \file B4aSteppingAction.cc
/// \brief Implementation of the B4aSteppingAction class

#include "B4aSteppingAction.hh"
#include "B4aEventAction.hh"
#include "B4DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::B4aSteppingAction(
                      const B4DetectorConstruction* detectorConstruction,
                      B4aEventAction* eventAction)
  : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction),
    fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::~B4aSteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aSteppingAction::UserSteppingAction(const G4Step* step)
{
// Collect energy and track length step by step
  WorldEdgeZ = fDetConstruction->fWorldEdgeZ;
  HCalorEdgeZ = fDetConstruction->fHCalorEdgeZ;
  G4int nofModuleX = fDetConstruction->fNModuleX;
  G4int nofModuleY = fDetConstruction->fNModuleY;


  //get Track
  auto track = step->GetTrack();

  // get volume of the current step
  auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // get detect time(Global Time)
  auto time = track->GetGlobalTime();

  // get particle ID
  G4int particleID = track->GetDynamicParticle()->GetPDGcode();

  // get track ID
  G4int trackID = track->GetTrackID();

  // get parent ID
  G4int parentID = track->GetParentID();
  
  // step length
  G4double stepLength = 0.;
  if ( track->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  // get Habsorber id
  if ( volume->GetName() == fDetConstruction->GetHAbsorberPV()->GetName() ) {
    G4int lyrid = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(1);//get layer number (replica Number)
    fEventAction->AddAbs(edep, stepLength, lyrid);
  }

  // get Hgap id
  if ( volume->GetName() == fDetConstruction->GetHGapPV()->GetName() ) {
    G4int lyrid = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(1);//get layer number (replica Number)
    G4int copy = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(0);
    G4int tilex = static_cast<int>(copy/(9*nofModuleY));
    G4int tiley = static_cast<int>(copy%(9*nofModuleY));
    fEventAction->AddGap(edep, stepLength, lyrid, tilex, tiley);
    if ( edep > 0 ) {
      fEventAction->AddTime(edep, time, particleID, lyrid, tilex, tiley);
    }
  }

  // get PreStepPoint
  auto prepoint = step->GetPreStepPoint()->GetPosition();

  // get PrePostPoint
  auto postpoint = step->GetPostStepPoint()->GetPosition();

  // get TrackID=1 infomation
  
  // get event condition
  if ( fEventAction->EventInitialInfo == 0 && trackID == 1) {
    auto vertex = track->GetVertexPosition();
    auto energy = track->GetDynamicParticle()->GetKineticEnergy();
    auto momentum = track->GetMomentum();
    G4double vertexx = vertex.x();
    G4double vertexy = vertex.y();
    G4double vertexz = vertex.z();
    G4double momentumx = momentum.x();
    G4double momentumy = momentum.y();
    G4double momentumz = momentum.z();
    G4cout << "--Initial Condition" << G4endl;
    G4cout << "Initial Point:{" << vertexx << " , " << vertexy << " , " << vertexz << "}" << G4endl;
    G4cout << "Initial Energy:" << energy << G4endl;
    G4cout << "Initial Momentum:{" << momentumx << " , " << momentumy << " , " << momentumz << "}" << G4endl;
    fEventAction->AddCondition(vertexx, vertexy, vertexz, energy, momentumx, momentumy, momentumz);
  }

  // get incident point
  if ( prepoint.z() == HCalorEdgeZ && ( trackID == 1 || parentID == 1) ) {
    G4double prepointx = prepoint.x();
    G4double prepointy = prepoint.y();
    G4cout << "--Incident to Calorimeter" << G4endl;
    G4cout << "ParticleName:" << track->GetDynamicParticle()->GetParticleDefinition()->GetParticleName() << G4endl;
    G4cout << "ParticleID:" << particleID << G4endl;
    G4cout << "TrackID:" << trackID << G4endl;
    G4cout << "ParentID:" << parentID << G4endl;
    G4cout << "Particle Incident:{" << prepointx << " , " << prepointy << " , " << prepoint.z() << "}" << G4endl;
    fEventAction->AddIncident(prepointx, prepointy, particleID);
  }

  // get ID=1 particle decay point
  if ( trackID == 1 ) {
    G4double postpointx = postpoint.x();
    G4double postpointy = postpoint.y();
    G4double postpointz = postpoint.z();
    fEventAction->AddVertex(postpointx, postpointy, postpointz, time);
  }

  // // get ParentID=1
  // if ( parentID == 1 ) {
  //   auto vertex = track->GetVertexPosition();
  //   G4cout << "Particle Name and ID:" << track->GetDynamicParticle()->GetParticleDefinition()->GetParticleName() << "," << particleID
  //   << "VertexPoint:{" << vertex.x() << " , " << vertex.y() << " , " << vertex.z() << "}"
  //   << G4endl;
  // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
