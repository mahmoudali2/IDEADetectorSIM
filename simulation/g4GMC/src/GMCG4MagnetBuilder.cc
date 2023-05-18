// GMCG4MagnetBuilder.cc
// Added by Mahmoud Ali

#include <GMCG4MagnetBuilder.hh>

#include <findMaterialOrThrow.hh>
#include <GeomService.hh>
#include "manageException.hh"

#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
//#include "G4Material.hh"
//#include "G4NistManager.hh"
#include "G4Colour.hh"
//#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4ios.hh"

#include <string>
#include <vector>

void GMCG4MagnetBuilder::constructMagnet(G4LogicalVolume* motherLogV) {

 // if (motherLogV==NULL) G4cerr<<"GMCG4MagnetBuilder mother volume not defined!"<<G4endl;

  G4bool  checkOverlap, detailedCheck;

  GeomService *geoms = GeomService::Instance();
  const crd::SimpleConfig &config = geoms->getConfig();


  checkOverlap = config.getBool("g4.doSurfaceCheck",false);
  detailedCheck = checkOverlap&&config.getBool("mgnt.doDetailedSurfCheck",false);

  bool isExternal     = false;
  std::string extFile        = config.getString("mgnt.extFile");
  if ( extFile.size()>1 && ( extFile.find_last_of(".gdml") || extFile.find_last_of(".GDML") )!=0 ) isExternal = true;

  if (isExternal) {
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"This GDML file option is temporarily disabled\n";
    e.error();
  } else {
    int geomType       = config.getInt("mgnt.geomType");
    double r_in        = config.getDouble("mgnt.r_in");
    double r_out       = config.getDouble("mgnt.r_out");
    double halfLength  = config.getDouble("mgnt.halfLength");


    int nShells        = config.getInt("mgnt.nShells");

    std::vector<double> shellsThickness;
    std::vector< std::string > shellsMaterial;

    config.getVectorDouble("mgnt.ShellsThickness", shellsThickness, nShells );
    config.getVectorString("mgnt.ShellsMaterial", shellsMaterial, nShells);


    G4VisAttributes* visAtt = new G4VisAttributes(true, G4Colour::Yellow() );
    visAtt->SetForceSolid(true);
    visAtt->SetForceAuxEdgeVisible (false);
    visAtt->SetVisibility(true);
    visAtt->SetDaughtersInvisible(false);

    G4Material* matMother = gmc::findMaterialOrThrow( config.getString("mgnt.motherVolMat","G4_AIR") );
    G4Tubs *mgntShp       = new G4Tubs("Magnet", r_in, r_out,halfLength+0.001,0.0,360.0*CLHEP::degree);
    G4LogicalVolume *mgntLog = new G4LogicalVolume(mgntShp , matMother, "MagnetMother",0,0,0);
     // mgntLog->SetVisAttributes(visAtt);

    G4Material* matFe = gmc::findMaterialOrThrow( "G4_Fe" );
    G4Tubs *mgntFeShp          = new G4Tubs("MagnetFe", r_in+0.001,r_out-0.001,halfLength,0.0,360.0*CLHEP::degree);
    G4LogicalVolume *mgntFeLog = new G4LogicalVolume(mgntFeShp , matFe, "MagnetFeVol",0,0,0);
      mgntFeLog->SetVisAttributes(visAtt);

    G4PVPlacement *mgntFeVol = new G4PVPlacement(0,   // no rotation
        G4ThreeVector(0,0,0),                         // at (x,y,z)
        mgntFeLog,                                    // its logical volume
        "MagnetFeVol",                                // its name
        mgntLog,                                      // its mother  volume
        false,                                        // no boolean operations
        0,                                            // copy number
        checkOverlap);
    mgntFeVol->GetCopyNo(); //Just to remove compiling warning
    
    G4PVPlacement *mgntVol = new G4PVPlacement(0,  // no rotation
        G4ThreeVector(0,0,0),                      // at (x,y,z)
        mgntLog,                                   // its logical volume
        "MagnetMother",                            // its name
        motherLogV,                                // its mother  volume
        false,                                     // no boolean operations
        0,                                         // copy number
        checkOverlap);
    mgntVol->GetCopyNo(); //Just to remove compiling warning
  


  }

}
