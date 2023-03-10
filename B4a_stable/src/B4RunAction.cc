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
/// \file B4RunAction.cc
/// \brief Implementation of the B4RunAction class

#include <sstream>

#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::B4RunAction(B4DetectorConstruction* detConstruction)
 : G4UserRunAction(),
   fDetConstruction(detConstruction)
{ 
  
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);     

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in B4Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Create directories 
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple
  //
  
  // Creating histograms
  analysisManager->CreateH1("Eabs","Edep in absorber", 100, 0., 6*GeV);
  analysisManager->CreateH1("Egap","Edep in gap", 100, 0., 1*GeV);
  analysisManager->CreateH1("Labs","trackL in absorber", 100, 0., 5*m);
  analysisManager->CreateH1("Lgap","trackL in gap", 100, 0., 2*m);
  

  // Creating ntuple
  //
  analysisManager->CreateNtuple("B4", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Egap");
  analysisManager->CreateNtupleDColumn("Labs");
  analysisManager->CreateNtupleDColumn("Lgap");
  analysisManager->CreateNtupleDColumn("Event");
  analysisManager->FinishNtuple();
  
  analysisManager->CreateNtuple("Edep", "Each Part Energy Deposit");
  analysisManager->CreateNtupleDColumn("Enumber");
  analysisManager->CreateNtupleDColumn("Lnumber");
  analysisManager->CreateNtupleDColumn("TXnumber");
  analysisManager->CreateNtupleDColumn("TYnumber");
  analysisManager->CreateNtupleDColumn("GorA");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("Gap_Edep", "Detect Time in Gap");
  analysisManager->CreateNtupleDColumn("Enumber");
  analysisManager->CreateNtupleDColumn("Lnumber");
  analysisManager->CreateNtupleDColumn("TXnumber");
  analysisManager->CreateNtupleDColumn("TYnumber");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("Time");
  analysisManager->CreateNtupleDColumn("ParticlID");
  analysisManager->FinishNtuple();

  analysisManager->CreateNtuple("Event_Condition", "Event Condition");
  analysisManager->CreateNtupleDColumn("Enumber");
  analysisManager->CreateNtupleDColumn("GenPointX");
  analysisManager->CreateNtupleDColumn("GenPointY");
  analysisManager->CreateNtupleDColumn("GenPointZ");
  analysisManager->CreateNtupleDColumn("InEnergy");
  analysisManager->CreateNtupleDColumn("MomentumX");
  analysisManager->CreateNtupleDColumn("MomentumY");
  analysisManager->CreateNtupleDColumn("MomentumZ");
  analysisManager->CreateNtupleDColumn("IncPointX");
  analysisManager->CreateNtupleDColumn("IncPointY");
  analysisManager->CreateNtupleDColumn("VerPointX");
  analysisManager->CreateNtupleDColumn("VerPointY");
  analysisManager->CreateNtupleDColumn("VerPointZ");
  analysisManager->CreateNtupleDColumn("PNumber");
  analysisManager->CreateNtupleDColumn("ParticleID");
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::~B4RunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "B4";
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  //
  auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl; 
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl; 
    }
    
    G4cout << " EAbs : mean = " 
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;
    
    G4cout << " EGap : mean = " 
       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;
    
    G4cout << " LAbs : mean = " 
      << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;

    G4cout << " LGap : mean = " 
      << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length") 
      << " rms = " 
      << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") << G4endl;
  }

  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
