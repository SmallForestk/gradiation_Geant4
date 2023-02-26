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
/// \file B4aEventAction.cc
/// \brief Implementation of the B4aEventAction class

#include "B4aEventAction.hh"
#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::B4aEventAction(B4DetectorConstruction* detConstruction)
 : G4UserEventAction(),
   fDetConstruction(detConstruction),
   fEnergyAbs(0.),
   fEnergyGap(0.),
   fTrackLAbs(0.),
   fTrackLGap(0.)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::~B4aEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aEventAction::BeginOfEventAction(const G4Event* /*event*/)
{  
  // initialisation per event
  fEnergyAbs = 0.;
  fEnergyGap = 0.;
  fTrackLAbs = 0.;
  fTrackLGap = 0.;
  for (G4int l=0; l<48; ++l) {
    fEnergyAbsbyLyr[l] = 0.;
    for (G4int ix=0; ix<100; ++ix) {
      for (G4int iy=0; iy<100; ++iy) {
        fEnergyGapbyLyr[l][ix][iy] = 0.;
      }
    }
  }

  vertextime = 0;
  fParticleNumber = 1;

  EventInitialInfo = 0;

  //vector initialization
  fDetectLayer.clear();
  fDetectTileX.clear();
  fDetectTileY.clear();
  fDetectEnergy.clear();
  fDetectTime.clear();
  fDetectPartileID.clear();

  fDetectLayer.shrink_to_fit();
  fDetectTileX.shrink_to_fit();
  fDetectTileY.shrink_to_fit();
  fDetectEnergy.shrink_to_fit();
  fDetectTime.shrink_to_fit();
  fDetectPartileID.shrink_to_fit();

  fIncidentPointX.clear();
  fIncidentPointY.clear();
  fIncidentID.clear();

  fIncidentPointX.shrink_to_fit();
  fIncidentPointY.shrink_to_fit();
  fIncidentID.shrink_to_fit();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aEventAction::EndOfEventAction(const G4Event* event)
{
  G4int nofModuleX = fDetConstruction->fNModuleX;
  G4int nofModuleY = fDetConstruction->fNModuleY;
  // Accumulate statistics
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill histograms
  analysisManager->FillH1(0, fEnergyAbs);
  analysisManager->FillH1(1, fEnergyGap);
  analysisManager->FillH1(2, fTrackLAbs);
  analysisManager->FillH1(3, fTrackLGap);

  auto eventID = event->GetEventID();

  // fill ntuple  
  analysisManager->FillNtupleDColumn(0, 0, fEnergyAbs);
  analysisManager->FillNtupleDColumn(0, 1, fEnergyGap);
  analysisManager->FillNtupleDColumn(0, 2, fTrackLAbs);
  analysisManager->FillNtupleDColumn(0, 3, fTrackLGap);
  analysisManager->FillNtupleDColumn(0, 4, eventID);
  analysisManager->AddNtupleRow(0);  
  
  // fill ntuple2
  for (G4int l = 0; l < 48; l++) {
    if (fEnergyAbsbyLyr[l] != 0) {
      analysisManager->FillNtupleDColumn(1, 0, eventID);
      analysisManager->FillNtupleDColumn(1, 1, l);
      analysisManager->FillNtupleDColumn(1, 2, 0);
      analysisManager->FillNtupleDColumn(1, 3, 0);
      analysisManager->FillNtupleDColumn(1, 4, 0);
      analysisManager->FillNtupleDColumn(1, 5, fEnergyAbsbyLyr[l]);
      analysisManager->AddNtupleRow(1);
    }
    for (G4int ix = 0; ix < nofModuleX*9; ix++) {
      for (G4int iy = 0; iy < nofModuleY*9; iy++) {
        if (fEnergyGapbyLyr[l][ix][iy] != 0) {
          analysisManager->FillNtupleDColumn(1, 0, eventID);
          analysisManager->FillNtupleDColumn(1, 1, l);
          analysisManager->FillNtupleDColumn(1, 2, ix);
          analysisManager->FillNtupleDColumn(1, 3, iy);
          analysisManager->FillNtupleDColumn(1, 4, 1);
          analysisManager->FillNtupleDColumn(1, 5, fEnergyGapbyLyr[l][ix][iy]);
          analysisManager->AddNtupleRow(1);
        }
      }
    }
  }

  //fill ntuple3
  for (std::vector<double>::iterator i = fDetectTime.begin(); i != fDetectTime.end(); i++) {
    int read = std::distance(fDetectTime.begin(), i);
    analysisManager->FillNtupleDColumn(2, 0, eventID);
    analysisManager->FillNtupleDColumn(2, 1, fDetectLayer[read]);
    analysisManager->FillNtupleDColumn(2, 2, fDetectTileX[read]);
    analysisManager->FillNtupleDColumn(2, 3, fDetectTileY[read]);
    analysisManager->FillNtupleDColumn(2, 4, fDetectEnergy[read]);
    analysisManager->FillNtupleDColumn(2, 5, fDetectTime[read]);
    analysisManager->FillNtupleDColumn(2, 6, fDetectPartileID[read]);
    analysisManager->AddNtupleRow(2);
    // G4cout << "Save {Event:" << eventID << ", GorA:" << fDetectGorA[read] << ", Layer:" << fDetectLayer[read] << ", X:" << ix <<
    //   ", Y:" << iy << ", Edep:" << fDetectEnergy[read] << ", Time:" << fDetectTime[read] << "}" << G4endl;
  }

  if (fIncidentPointX.empty()) {
    analysisManager->FillNtupleDColumn(3, 0, eventID);
    analysisManager->FillNtupleDColumn(3, 1, fGenerationPointX);
    analysisManager->FillNtupleDColumn(3, 2, fGenerationPointY);
    analysisManager->FillNtupleDColumn(3, 3, fGenerationPointZ);
    analysisManager->FillNtupleDColumn(3, 4, fInitialEnergy);
    analysisManager->FillNtupleDColumn(3, 5, fMomentumX);
    analysisManager->FillNtupleDColumn(3, 6, fMomentumY);
    analysisManager->FillNtupleDColumn(3, 7, fMomentumZ);
    analysisManager->FillNtupleDColumn(3, 8, 0);
    analysisManager->FillNtupleDColumn(3, 9, 0);
    analysisManager->FillNtupleDColumn(3, 10, fVertexX);
    analysisManager->FillNtupleDColumn(3, 11, fVertexY);
    analysisManager->FillNtupleDColumn(3, 12, fVertexZ);
    analysisManager->FillNtupleDColumn(3, 13, -fParticleNumber);
    analysisManager->FillNtupleDColumn(3, 14, 0);
    analysisManager->AddNtupleRow(3);
  } else {
    for (std::vector<double>::iterator i = fIncidentPointX.begin(); i != fIncidentPointX.end(); i++) {
      int read = std::distance(fIncidentPointX.begin(), i);
      analysisManager->FillNtupleDColumn(3, 0, eventID);
      analysisManager->FillNtupleDColumn(3, 1, fGenerationPointX);
      analysisManager->FillNtupleDColumn(3, 2, fGenerationPointY);
      analysisManager->FillNtupleDColumn(3, 3, fGenerationPointZ);
      analysisManager->FillNtupleDColumn(3, 4, fInitialEnergy);
      analysisManager->FillNtupleDColumn(3, 5, fMomentumX);
      analysisManager->FillNtupleDColumn(3, 6, fMomentumY);
      analysisManager->FillNtupleDColumn(3, 7, fMomentumZ);
      analysisManager->FillNtupleDColumn(3, 8, fIncidentPointX[read]);
      analysisManager->FillNtupleDColumn(3, 9, fIncidentPointY[read]);
      analysisManager->FillNtupleDColumn(3, 10, fVertexX);
      analysisManager->FillNtupleDColumn(3, 11, fVertexY);
      analysisManager->FillNtupleDColumn(3, 12, fVertexZ);
      analysisManager->FillNtupleDColumn(3, 13, fParticleNumber);
      analysisManager->FillNtupleDColumn(3, 14, fIncidentID[read]);
      analysisManager->AddNtupleRow(3);
      fParticleNumber++;
    }
  }
  
  // Print per event (modulo n)
  //
  
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;     

    G4cout
       << "   Absorber: total energy: " << std::setw(7)
                                        << G4BestUnit(fEnergyAbs,"Energy")
       << "       total track length: " << std::setw(7)
                                        << G4BestUnit(fTrackLAbs,"Length")
       << G4endl
       << "        Gap: total energy: " << std::setw(7)
                                        << G4BestUnit(fEnergyGap,"Energy")
       << "       total track length: " << std::setw(7)
                                        << G4BestUnit(fTrackLGap,"Length")
       << G4endl;
  }
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
