//
// SIWRPtrackerBuilder builder class for the SIWRP in geant4
//
// Original author G. Tassielli
// Modified by Mahmoud Ali

#ifndef SIWRPrackerBuilder_hh
#define SIWRPrackerBuilder_hh

#include <VolumeInfo.hh>
#include "SimpleConfig.hh"

// G4 includes
#include "G4LogicalVolume.hh"

namespace svx {
class Ladder;
}

namespace siwrp {

  class SIWRPBuilder {
  public:
    static VolumeInfo constructTracker( G4LogicalVolume* mother/*, double zOff*/ );
    static void constructStepLimiters();
    static void instantiateSensitiveDetectors(const std::string hitsCollectionName="SIWRPHitsCollection");
//    static void constructRadiator( G4LogicalVolume* siwrpmother/*, double zOff*/ );
  private:
    static VolumeInfo buildLadder(svx::Ladder &tld);
  };

} //namespace siwrp

#endif /* SIWRPrackerBuilder_hh */
