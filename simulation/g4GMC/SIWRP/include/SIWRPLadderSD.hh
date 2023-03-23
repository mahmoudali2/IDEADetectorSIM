#ifndef SIWRPLadderSD_hh
#define SIWRPLadderSD_hh
//
// Defines a generic SIWRPtracker sensitive detector
//
// Original author G. Tassielli
// 

// SIWRP includes
#include <SensitiveDetector.hh>
#include "SIWRPtracker.hh"

// GMCG4 includes
#include "GeomHandle.hh"

namespace siwrp {

  class SIWRPLadderSD : public SensitiveDetector {

  public:
    SIWRPLadderSD(const G4String &name, const G4String& hitsCollectionName, const crd::SimpleConfig& config);
    virtual ~SIWRPLadderSD();

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

    void setSIWRPCenterInDetSys(const G4ThreeVector &origin) {
            _siwrpDetCenter = origin + _systOrigin;
    }

  protected:

    int _layer;
    int _phiSec;
    int _ladder;
    int _roSubShell;
    unsigned long _roIdFstSide;
    unsigned long _roIdSndSide;
//    double _Dphi;

    GeomHandle<SIWRPtracker> siwrptracker;
    //SIWRPtracker::GeomType _ittype;

    // SIWRP point of origin in ref. System
    G4ThreeVector _siwrpDetCenter;

  };

} // namespace siwrp

#endif /* SIWRPLadderSD_hh */
