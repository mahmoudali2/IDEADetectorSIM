// **test class Added by Mahmoud Ali**

#ifndef GMCG4SteppingAction_h
#define GMCG4SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4Step.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"

#include "GMCG4EventAction.hh"
#include "GMCG4DetectorConstruction.hh"
#include "GMCG4RunAction.hh"

//class GMCG4EventAction;

class GMCG4SteppingAction : public G4UserSteppingAction
{
 public:
    GMCG4SteppingAction(GMCG4EventAction* eventAction);
     ~GMCG4SteppingAction();


     virtual void UserSteppingAction(const G4Step*);
    
 private:
    GMCG4EventAction *fEventAction;

};


#endif
