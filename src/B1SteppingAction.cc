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
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(B1EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume0(0),
  fScoringVolume1(0),
  fScoringVolume2(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume0) {
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume0 = detectorConstruction->GetScoringVolume(0);
  }

  if (!fScoringVolume1) {
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume1 = detectorConstruction->GetScoringVolume(1);
  }

  if (!fScoringVolume2) {
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume2 = detectorConstruction->GetScoringVolume(2);
  }

  // get volume of the current step
  G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  // check if we are in one of the scoring volumes (ALPIDE1, ALPIDE2 or converter foil)




  // // Get volume of preStepPointVolume
  // G4LogicalVolume* pre = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  //
  // // Get volume of postStepPointVolume
  // G4LogicalVolume* post = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  //
  //
  // if ((post!=fScoringVolume) && (pre==fScoringVolume) ){return;}
  //{G4cout << "pre: "<<pre->GetName()<<", post: "<<post->GetName()<<G4endl;return;}
  //if ((pre!=fScoringVolume) ) return;
  // collect energy deposited in this step
  // G4double eKin = step->GetTrack()->GetKineticEnergy ();
  // G4double edepStep = step->GetTotalEnergyDeposit();
  // fEventAction->AddEdep(edepStep);


  if ((volume!=fScoringVolume0)) return; //track inside converter foil
  //if ((volume!=fScoringVolume1) && (volume!=fScoringVolume2) ) return; // track ALPIDE1 and ALPIDE2
  G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  if(particleName== "e-" || particleName=="gamma"){ //|| particleName == "neutron"){

    // Converter foil
    G4double x = step->GetTrack()->GetVertexPosition().getX();
    G4double y = step->GetTrack()->GetVertexPosition().getY();
    G4double z = step->GetTrack()->GetVertexPosition().getZ();

    // ALPIDES
    // G4double x = step->GetTrack()->GetPosition().getX();
    // G4double y = step->GetTrack()->GetPosition().getY();
    // G4double z = step->GetTrack()->GetPosition().getZ();

    G4double eKin = step->GetTrack()->GetKineticEnergy();
    G4double track_id = (G4double)step->GetTrack()->GetTrackID();
    G4double parent_id = (G4double)step->GetTrack()->GetParentID();
    G4String volumeAtVertex_ = step->GetTrack()->GetLogicalVolumeAtVertex()->GetName();
    G4String processName_ = step->GetTrack()->GetCreatorProcess()->GetProcessName();

    // MY CODE - Start
    // production process: compt, nCapture, phot, conv
    G4double processName = 0.0;     // default
    if (processName_== "compt")     {processName = 1;}
    if (processName_== "nCapture")  {processName = 2;}
    if (processName_== "phot")      {processName = 3;}
    if (processName_== "conv")      {processName = 4;}

    // production volumes: gd, AlpideSens1, al_1, AlpideSens2, al_2
    G4double volumeAtVertex = 0.0;
    if (volumeAtVertex_== "gd")               {volumeAtVertex =  1;}
    if (volumeAtVertex_== "AlpideSens1")      {volumeAtVertex =  2;}
    if (volumeAtVertex_== "al_1")             {volumeAtVertex =  3;}
    if (volumeAtVertex_== "AlpideSens2")      {volumeAtVertex =  4;}
    if (volumeAtVertex_== "al_2")             {volumeAtVertex =  5;}

    // particle ID: 1: electron, 2: gamma
    G4double particle_id = 0.0; // default
    if (particleName == "neutron"){particle_id = 0;}
    if (particleName == "e-")     {particle_id = 1;}
    if (particleName == "gamma")  {particle_id = 2;}
    //G4cout<< "trackID: "<<trackID << ", parentID: "<<parentID <<", S: "<<currentStepNumber <<",z: "<<z << G4endl;
    //G4cout<< "trackID: "<<track_id << ", parentID: "<<parent_id <<G4endl;
    fEventAction->StoreKineticEnergy(eKin);
    fEventAction->StoreX(x);
    fEventAction->StoreY(y);
    fEventAction->StoreZ(z);
    fEventAction->StoreProductionVolume(volumeAtVertex);
    fEventAction->StoreCreatorProcess(processName);
    fEventAction->StoreTrackID(track_id);
    fEventAction->StoreParentID(parent_id);
    fEventAction->StoreParticleID(particle_id);


    // MY CODE - End
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......