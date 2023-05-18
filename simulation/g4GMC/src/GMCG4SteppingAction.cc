// ** test class added by Mahmoud Ali ** 

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VTrajectoryPoint.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ios.hh"
#include "g4root.hh"

#include "GMCG4SteppingAction.hh"
#include "GMCG4EventAction.hh"
#include "GMCG4RunAction.hh"
#include "DRCaloIO.hh"
#include "RootIO.hh"
#include "GMCG4Particle.hh"
#include "GeomService.hh"
#include "GMCG4DetectorConstruction.hh"

#include "G4Geantino.hh"
#include "G4Electron.hh"
#include "G4MuonMinus.hh"
#include "G4eMultipleScattering.hh"
#include "G4ProcessManager.hh"


GMCG4SteppingAction::GMCG4SteppingAction(GMCG4EventAction *eventAction)
{
    fEventAction = eventAction;
}

GMCG4SteppingAction::~GMCG4SteppingAction()
{}

void GMCG4SteppingAction::UserSteppingAction(const G4Step *step)
{   
 
    G4double endep = step->GetTotalEnergyDeposit();
    fEventAction->AddEndep(endep);
  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


    G4TouchableHandle touch = step->GetPreStepPoint()->GetTouchableHandle();
    G4LogicalVolume* logical = touch->GetVolume()->GetLogicalVolume();

    G4String volumeName = logical->GetName();
    G4String materialName = logical->GetMaterial()->GetName();

    std::cout<<"  volumeName = "<<volumeName<<"   material = "<<materialName<<std::endl;
    if(volumeName == "World") return;


    G4ParticleDefinition* particle = step->GetTrack()->GetDefinition();
  //  if(particle == G4Geantino::Geantino())
  //  {
      //double ds = logical->GetMaterial()->GetDensity() //   check units / (g/cm3); // g/cm^3
      double rl = logical->GetMaterial()->GetRadlen() / 10.0;     // cm radlen [mm] = 10 * radlen [g cm-2] / rho [g cm-3]
      double sl = step->GetStepLength() / 10.0;            // cm              step length [mm]
      double rad = sl / rl;


      fEventAction->AddRad(rad);


      std::cout<<"**************** rl = "<<rl<<std::endl;
      std::cout<<"**************** sl = "<<sl<<std::endl;
      std::cout<<"**************** rad = "<<rad<<std::endl;

      double cosTh = (step->GetPostStepPoint()->GetPosition()).cosTheta();
      double phiAz = (step->GetPostStepPoint()->GetPosition()).phi();

      fEventAction->AddCos(cosTh);     
      fEventAction->AddPhi(phiAz);      

      std::cout<<"   cos = "<<cosTh<<std::endl; 
      std::cout<<"   phi = "<<phiAz<<std::endl;

//    }
}
