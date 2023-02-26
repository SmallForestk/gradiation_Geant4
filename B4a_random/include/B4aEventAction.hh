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
/// \file B4aEventAction.hh
/// \brief Definition of the B4aEventAction class

#ifndef B4aEventAction_h
#define B4aEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include "B4DetectorConstruction.hh"

#include <vector>

/// Event action class
///
/// It defines data members to hold the energy deposit and track lengths
/// of charged particles in Absober and Gap layers:
/// - fEnergyAbs, fEnergyGap, fTrackLAbs, fTrackLGap
/// which are collected step by step via the functions
/// - AddAbs(), AddGap()

class B4aEventAction : public G4UserEventAction
{
  public:
    B4aEventAction(B4DetectorConstruction* detConstruction);
    virtual ~B4aEventAction();

    virtual void  BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);
    
    void AddAbs(G4double de, G4double dl, G4int lyr);
    void AddGap(G4double de, G4double dl, G4int lyr, G4int tilex, G4int tiley);
    void AddTime(G4double de, G4double time, G4int particle, G4int lyr, G4int tilex, G4int tiley);
    void AddCondition(
      G4double genpointx, G4double genpointy, G4double genpointz, 
      G4double kinenergy,
      G4double momentumx, G4double momentumy, G4double momentumz);
    void AddVertex(G4double vertexx, G4double vertexy, G4double vertexz, G4double detecttime);
    void AddIncident(G4double incpointx, G4double incpointy, G4int particleID);

    G4double EventInitialInfo;
    
  private:
    G4double  fEnergyAbs;
    G4double  fEnergyGap;
    G4double  fTrackLAbs; 
    G4double  fTrackLGap;
    G4double  fAnlge;
    G4double  fEnergyAbsbyLyr[48];//[layer]
    G4double  fEnergyGapbyLyr[48][100][100];//[layer][xtile][ytile]
    B4DetectorConstruction* fDetConstruction;

    std::vector<int> fDetectLayer;
    std::vector<int> fDetectTileX;
    std::vector<int> fDetectTileY;
    std::vector<double> fDetectTime;
    std::vector<double> fDetectEnergy;
    std::vector<double> fDetectPartileID;

    G4double fGenerationPointX;
    G4double fGenerationPointY;
    G4double fGenerationPointZ;
    G4double fInitialEnergy;
    G4double fMomentumX;
    G4double fMomentumY;
    G4double fMomentumZ;
    std::vector<double> fIncidentPointX;
    std::vector<double> fIncidentPointY;
    std::vector<double> fIncidentID;
    G4double fVertexX;
    G4double fVertexY;
    G4double fVertexZ;
    G4int fParticleNumber;

    G4double vertextime;
};

// inline functions

inline void B4aEventAction::AddAbs(G4double de, G4double dl, G4int lyr = -1) {
  fEnergyAbs += de; 
  fTrackLAbs += dl;
  if (lyr != -1) fEnergyAbsbyLyr[lyr] += de;
}

inline void B4aEventAction::AddGap(G4double de, G4double dl, G4int lyr = -1, G4int tilex = -1, G4int tiley = -1) {
  fEnergyGap += de; 
  fTrackLGap += dl;
  if (lyr != -1 && tilex != -1 && tiley != -1) fEnergyGapbyLyr[lyr][tilex][tiley] += de;
}

inline void B4aEventAction::AddTime(G4double de, G4double time, G4int particle, G4int lyr = -1, G4int tilex = -1, G4int tiley = -1) {
  if (lyr != -1 && tilex != -1 && tiley != -1) {
    fDetectEnergy.push_back(de);
    fDetectTime.push_back(time);
    fDetectLayer.push_back(lyr);
    fDetectTileX.push_back(tilex);
    fDetectTileY.push_back(tiley);
    fDetectPartileID.push_back(particle);
  }
}

inline void B4aEventAction::AddCondition(
  G4double genpointx, G4double genpointy, G4double genpointz, 
  G4double kinenergy,
  G4double momentumx, G4double momentumy, G4double momentumz) {
  fGenerationPointX = genpointx;
  fGenerationPointY = genpointy;
  fGenerationPointZ = genpointz;
  fInitialEnergy = kinenergy;
  fMomentumX = momentumx;
  fMomentumY = momentumy;
  fMomentumZ = momentumz;
  EventInitialInfo++;
}

inline void B4aEventAction::AddVertex(G4double vertexx, G4double vertexy, G4double vertexz, G4double detecttime) {
  if ( detecttime >= vertextime ) {
    fVertexX = vertexx;
    fVertexY = vertexy;
    fVertexZ = vertexz;
  }
}

inline void B4aEventAction::AddIncident(G4double incpointx, G4double incpointy, G4int particleID) {
  fIncidentPointX.push_back(incpointx);
  fIncidentPointY.push_back(incpointy);
  fIncidentID.push_back(particleID);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
