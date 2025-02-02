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
// $Id: GMCG4EventAction.cc 75214 2013-10-29 16:04:42Z gcosmo $
//
/// \file GMCG4EventAction.cc
/// \brief Implementation of the GMCG4EventAction class

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VTrajectoryPoint.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleDefinition.hh"

#include "G4ios.hh"

// GMC includes
#include "GMCG4EventAction.hh"

#include "DRCaloIO.hh"
#include "RootIO.hh"
#include "GMCG4Particle.hh"
#include "GeomService.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GMCG4EventAction::GMCG4EventAction(GMCG4RunAction*)
: G4UserEventAction() {
  fEndep = 0.;
  frad = 0.;
  cos = 0.;
  phi = 0.;

	GeomService *geoms = GeomService::Instance();
	_hasDRFPIC = geoms->getConfig().getBool("hasDRFPIC",false);
	_wrtASCIIDR = geoms->getConfig().getBool("drc.writeASCII",false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GMCG4EventAction::~GMCG4EventAction() {

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4EventAction::BeginOfEventAction(const G4Event*) {
    fEndep = 0.;
    frad = 0.;
    cos = 0.;
    phi = 0.;

	  if (_hasDRFPIC){
		  drc::DRCaloIO::GetInstance()->newEvent( G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID(), _wrtASCIIDR );
	  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void GMCG4EventAction::EndOfEventAction(const G4Event* event) {
  
  //  filling the ntuple columns
  G4cout << "Energy deposition: " << fEndep << G4endl;
  G4cout << "Number of Radiation lengths: " << frad << G4endl;
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  man->FillNtupleDColumn(0, 0, fEndep);
  man->AddNtupleRow(0);

  man->FillNtupleDColumn(1, 0, frad);
  man->FillNtupleDColumn(1, 1, cos);
  man->FillNtupleDColumn(1, 2, phi);
  man->AddNtupleRow(1);


  // get number of stored trajectories

  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();

  G4int n_trajectories = 0;
  if (trajectoryContainer) {
  
    n_trajectories = trajectoryContainer->entries();

    std::vector<GMCG4Particle*> cnttracks;
    //TClonesArray  cnttracks("GMCG4Particle",100000);
    for (G4int i=0;i<n_trajectories;i++) {
      G4VTrajectory *tmpTrk = (*trajectoryContainer)[i];
      cnttracks.push_back( new GMCG4Particle( tmpTrk->GetTrackID(),
                                     tmpTrk->GetParentID(),
                                     tmpTrk->GetParticleName(),
                                     tmpTrk->GetCharge(),
                                     tmpTrk->GetPDGEncoding(),
                                     tmpTrk->GetPoint(0)->GetPosition(),
                                     tmpTrk->GetPoint(tmpTrk->GetPointEntries()-1)->GetPosition(),
                                     tmpTrk->GetInitialMomentum() ) );
    }
    
    RootIO::GetInstance()->PutMCTracks(cnttracks);

  }

  RootIO::GetInstance()->FillEvent();

  if (_hasDRFPIC){
    drc::DRCaloIO::GetInstance()->writePodioTruthPrimaryVertex(event);
    drc::DRCaloIO::GetInstance()->writePodio( G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() );
  }
  // periodic printing

  G4int eventID = event->GetEventID();
  if ( /*eventID < 100 ||*/ eventID % 100 == 0) {
    G4cout << ">>> Event: " << eventID  << G4endl;
    if ( trajectoryContainer ) {
      G4cout << "    " << n_trajectories
             << " trajectories stored in this event." << G4endl;
    }
    G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
    G4cout << "    "  
	   << hc->GetSize() << " hits stored in this event" << G4endl;
  }
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
