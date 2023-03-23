/*
 * GMCG4MagnetBuilder.hh
 *
 */

#ifndef G4GMC_GMCG4MAGNETBUILDER_HH_
#define G4GMC_GMCG4MAGNETBUILDER_HH_

#include "G4LogicalVolume.hh"

#include <cstddef>

class GMCG4MagnetBuilder {
public:
  static void constructMagnet(G4LogicalVolume* motherLogV );
};

#endif /* G4GMC_GMCG4MAGNETBUILDER_HH_ */
