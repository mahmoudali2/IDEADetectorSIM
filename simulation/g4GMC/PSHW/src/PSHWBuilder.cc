//
// PSHWtrackerBuilder builder class for the PSHW in geant4
//
// Original author G. Tassielli
//

#include <PSHWBuilder.hh>
#include "PSHWtracker.hh"
#include "Layer.hh"
#include "PSHWLadderSD.hh"

//#include "G4Hype.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"

#include "GeomService.hh"
#include "GeomHandle.hh"
#include "findMaterialOrThrow.hh"
#include <SensitiveDetectorName.hh>

using namespace std;
using namespace svx;

namespace pshw {

bool checkOverlap, detailedCheck;
string pshwName("PSHWMother");

VolumeInfo PSHWBuilder::constructTracker( G4LogicalVolume* mother/*, double zOff*/ ){

  // Master geometry for the tracker.
  GeomHandle<PSHWtracker> pshwtracker;
  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  VolumeInfo pshwInfo;

  double z0    = CLHEP::mm * pshwtracker->z0();
  G4ThreeVector trackerOffset(0.,0.,z0/*-zOff*/);

  checkOverlap = config.getBool("g4.doSurfaceCheck",false);
  detailedCheck = checkOverlap&&config.getBool("pshw.doDetailedSurfCheck",false);

  if (pshwtracker->isExternal()) {
//                throw cet::exception("GEOM") <<"This GDML file option is temporarily disabled\n";
    exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
    e<<"PSHW: This GDML file option is temporarily disabled\n";
    e.error();
  } else {

    G4VisAttributes* visAtt = new G4VisAttributes(true, G4Colour::White() );
    visAtt->SetForceSolid(true);
    visAtt->SetForceAuxEdgeVisible (false);
    visAtt->SetVisibility(false);
    visAtt->SetDaughtersInvisible(false);

    G4Material* matMother = gmc::findMaterialOrThrow( config.getString("pshw.motherVolMat","G4_AIR") );

    if (pshwtracker->nFwdLayers()>0) {
      double maxZ = pshwtracker->zPos_FW()+pshwtracker->zHalfLength_Fw();
      if ( pshwtracker->zHalfLength()>maxZ ) { maxZ = pshwtracker->zHalfLength(); }
      double maxR = pshwtracker->rOut();
      if ( pshwtracker->rOut_FW()>maxR ) { maxR = pshwtracker->rOut_FW(); }
      G4Tubs *mothOut = new G4Tubs("PreShowerOut",pshwtracker->r0_Fw()-0.001 ,maxR+0.001,maxZ+0.001,0.0,360.0*CLHEP::degree);
      G4Tubs *mothIn = new G4Tubs("PreShowerIn", pshwtracker->r0_Fw()-0.0015,pshwtracker->r0()-0.001,pshwtracker->zPos_FW()-pshwtracker->zHalfLength_Fw()-0.001,0.0,360.0*CLHEP::degree);
      pshwInfo.solid = new G4SubtractionSolid("PreShower",mothOut,mothIn);
    } else {
      pshwInfo.solid = new G4Tubs("PreShower", pshwtracker->r0()-0.001,pshwtracker->rOut()+0.001,pshwtracker->zHalfLength()+0.001,0.0,360.0*CLHEP::degree);
    }
    pshwInfo.logical = new G4LogicalVolume(pshwInfo.solid , matMother, pshwName,0,0,0);
    pshwInfo.logical->SetVisAttributes(visAtt);

    G4VisAttributes* visAttLay = new G4VisAttributes(true, G4Colour::Red() );
    visAttLay->SetForceSolid(true);
    visAttLay->SetForceAuxEdgeVisible (false);
    visAttLay->SetVisibility(true);
    visAttLay->SetDaughtersInvisible(false);

    bool debugLayer =  config.getBool("pshw.debugLayer",false);

    char shape[50], vol[50];

    if (pshwtracker->geomType()==0) {

      for (int iLy = 0; iLy < pshwtracker->nLayers(); ++iLy){

        Layer *ily = pshwtracker->getLayer(iLy);
        if ( debugLayer ) {
          cout<<"PSHW Layer: "<<ily->Id()<<endl;
        }
        VolumeInfo LayerInfo;
        sprintf(shape,"pswly-L%d",iLy);
        sprintf(vol,"pswlyvol-L%03d",iLy);

        LayerInfo.solid = new G4Tubs(shape, ily->getDetail()->InnerRadius()-0.0005,
            ily->getDetail()->OuterRadius()+0.0005,
            ily->getDetail()->halfLength()+0.0005,
            0.0,360.0*CLHEP::degree);
        LayerInfo.logical = new G4LogicalVolume(LayerInfo.solid,matMother,vol,0,0,0);
        //            if (ily->voxelizationFactor()==0.0) {
        //                    LayerInfo.logical->SetOptimisation(false);
        //            }
        //            else{
                            LayerInfo.logical->SetSmartless( 1.0/((float)(ily->nPhiSectors() * ily->nLaddersPerSector())) );
        //            }
        LayerInfo.logical->SetVisAttributes(visAttLay);

        bool isFw = (ily->getLayerZone()==Layer::forward);

        boost::shared_ptr<Ladder> ild = ily->getLadder(0);
        VolumeInfo LadderInfo = buildLadder(*ild);
        sprintf(vol,"pswldvol-L%03dLd%05ld",ild->Id().getLayer(),ild->Id().getLadder());
//        ily->nLaddersPerSector();

        for (unsigned long iLd=0; iLd < ily->nLadders(); ++iLd ){

//          boost::shared_ptr<Ladder> ild = ily->getLadder(iLd);
          ild = ily->getLadder(iLd);

          if (isFw && (ild->Id().getLadder()/100)==1 &&(ild->Id().getLadder()%100)>0 ) {
            LadderInfo = buildLadder(*ild);
            sprintf(vol,"pswldvol-L%03dLd%05ld",ild->Id().getLayer(),ild->Id().getLadder());
          }

          //if (ild->Id().getLadder()/100>1) continue;
          //if (ild->Id().getLadder()%100>1) continue;
          //if (ild->Id().getLadder()%100<4 || ild->Id().getLadder()%100>10) continue;
          //std::cout<<"iLd "<<iLd<<" Phi "<<iLd/ily->nLaddersPerSector()<<std::endl;
          //if (iLd/ily->nLaddersPerSector()!=0) continue;

          if (ild->getLadderGeomType()==Ladder::plane) {

            if (debugLayer ) {
              cout<<"Ladder "<< ild->Id()<<" geom type "<<ild->getLadderGeomType()<<" type "<<ild->getLadderType()<<" nROs "<<ild->nReadOuts()<<endl;
              cout<<"Vol name "<<vol<<endl;
              cout<<*ild<<endl;
            }

            int copyNum = iLd;
            HepGeom::Transform3D absLadTransf = ild->get3DTransfrom();
            if (isFw) {
              copyNum=ild->Id().getLadder()/100 -1;
              absLadTransf=HepGeom::TranslateZ3D(-ily->getDetail()->zPosition())*absLadTransf;
            }
//            CLHEP::HepRotation ldRot = ild->get3DTransfrom().getRotation();
            LadderInfo.physical = new G4PVPlacement(absLadTransf,//ild->get3DTransfrom(),
                LadderInfo.logical,      // its logical volume
                vol,                     // its name
                LayerInfo.logical,       // its mother  volume
                false,                   // no boolean operations
                copyNum,                 // copy number
                checkOverlap);

            if (ild->getLadderType() == Ladder::pixel) {
              G4VSensitiveDetector *sd = G4SDManager::GetSDMpointer()->FindSensitiveDetector(SensitiveDetectorName::PSHWTrackerRO());
              if(sd) {
                if (ild->getDetail()->nShells()>1) {
                  //for (int ishell=0; ishell<ild->getDetail()->nShells(); ++ishell){
                  //  LadderInfo.logical->GetDaughter(ishell)->GetLogicalVolume()->SetSensitiveDetector(sd);
                  //}
                  boost::shared_ptr<ReadOutDetail> ildROd = ild->getReadOut()->getDetail();
                  for (int ishell=0; ishell<ildROd->nShells(); ++ishell){
                    LadderInfo.logical->GetDaughter(ildROd->shellId(ishell))->GetLogicalVolume()->SetSensitiveDetector(sd);
                  }
                } else {
                  LadderInfo.logical->SetSensitiveDetector(sd);
                }
              }
            }

          } else {
            exc::exceptionG4 e("GEOM","Fatal Error in Argument",1);
            e<<"PSHW: Only plane ladder geometry is implemented yet\n";
            e.error();

          }
        } // Ladder loop

        LayerInfo.physical = new G4PVPlacement(0,               // no rotation
            G4ThreeVector(0,0,ily->getDetail()->zPosition()),         // at (x,y,z)
            LayerInfo.logical,       // its logical volume
            vol,                     // its name
            pshwInfo.logical,     // its mother  volume
            false,                   // no boolean operations
            0,                       // copy number
            checkOverlap);

      } // Layer loop

    } // geom 00


    pshwInfo.physical =  new G4PVPlacement( 0,
                    trackerOffset,
                    pshwInfo.logical,
                    pshwName,
                    mother,
                    0,
                    0,
                    checkOverlap);

    if ( checkOverlap ) { cout<<"PSHW Overlap Checking "<<pshwInfo.physical->CheckOverlaps(100000,0.0001,true)<<endl; }


  }

  return pshwInfo;

}

//VolumeInfo PSHWtrackerBuilder::buildLadder(float radius, float length, char *shapeName, char *volName, const std::vector<std::string> &materialName, const std::vector<double> &thicknesses, bool activeWireSD, bool isSense){
VolumeInfo PSHWBuilder::buildLadder(Ladder &tld){

  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  G4VisAttributes* visAttLad = new G4VisAttributes(true, G4Colour::Brown() );
  visAttLad->SetForceSolid(true);
  visAttLad->SetForceAuxEdgeVisible (false);
  visAttLad->SetVisibility(true);
  visAttLad->SetDaughtersInvisible(false);

//  G4VisAttributes* visAttLad1 = new G4VisAttributes(true, G4Colour::Blue() );
//  visAttLad1->SetForceSolid(true);
//  visAttLad1->SetForceAuxEdgeVisible (false);
//  visAttLad1->SetVisibility(true);
//  visAttLad1->SetDaughtersInvisible(false);

  VolumeInfo LadderInfo;
  char shapeName[50],volName[50];
//  sprintf(shapeName,"%ld",tld.Id().getLadder());
  sprintf(shapeName,"pswld-L%dLd%ld",tld.Id().getLayer(),tld.Id().getLadder());
  sprintf(volName,"pswldvol-L%03dLd%05ld",tld.Id().getLayer(),tld.Id().getLadder());

//  if (debugLayer ) {
//    cout<<"Ladder "<< tld.Id()<<" geom type "<<tld.getLadderGeomType()<<" type "<<tld.getLadderType()<<" nROs "<<tld.nReadOuts()<<endl;
//    cout<<"Vol name "<<volName<<endl;
//    cout<<*tld<<endl;
//  }

  if (tld.getLadderGeomType()==Ladder::plane) {
    if (tld.isWedgeType()) {
      LadderInfo.solid = new G4Trd(shapeName,tld.getDetail()->width()*0.5,
          tld.getDetail()->scndWidth()*0.5,
          tld.getDetail()->thickness()*0.5,
          tld.getDetail()->thickness()*0.5,
          tld.getDetail()->halfLength());
    } else {
      LadderInfo.solid = new G4Box(shapeName,tld.getDetail()->width()*0.5,
          tld.getDetail()->thickness()*0.5,
          tld.getDetail()->halfLength());
    }
    if (tld.getDetail()->nShells()==1) {
      LadderInfo.logical = new G4LogicalVolume(LadderInfo.solid,gmc::findMaterialOrThrow( tld.getDetail()->materialName(0).c_str() ),volName,0,0,0);
    }
    else {
      G4Material* matMother = gmc::findMaterialOrThrow( config.getString("pshw.motherVolMat","G4_AIR") );
      LadderInfo.logical = new G4LogicalVolume(LadderInfo.solid,matMother/*gmc::findMaterialOrThrow( "G4_Galactic" )*/,volName,0,0,0);
      char tShapeName[100], tVolName[100];

      double iYpos = -0.5*tld.getDetail()->thickness();

      for (int ishell=0; ishell<tld.getDetail()->nShells(); ++ishell){
        sprintf(tShapeName,"%s_sub%i",shapeName,ishell);
        sprintf(tVolName,"%s_sub%i",volName,ishell);
        double shlThckHlf = 0.5*tld.getDetail()->shellThickness(ishell);
        iYpos += shlThckHlf;
        G4VSolid *tldsh=0x0;
        if (tld.isWedgeType()) {
          tldsh = new G4Trd(tShapeName,tld.getDetail()->width()*0.5,
              tld.getDetail()->scndWidth()*0.5,
              shlThckHlf,
              shlThckHlf,
              tld.getDetail()->halfLength());
        } else {
          tldsh = new G4Box(tShapeName,tld.getDetail()->width()*0.5,
              shlThckHlf,
              tld.getDetail()->halfLength());
        }

        G4LogicalVolume *tlogicLdSh = new G4LogicalVolume(tldsh,gmc::findMaterialOrThrow(tld.getDetail()->materialName(ishell).c_str()),tVolName,0,0,0);

        G4VPhysicalVolume *tphysLdSh = new G4PVPlacement(0,
            G4ThreeVector(0,iYpos,0),
            tlogicLdSh,         // its logical volume
            tVolName,           // its name
            LadderInfo.logical, // its mother  volume
            false,              // no boolean operations
            0);                 // copy number
        tphysLdSh->GetCopyNo(); //just to remove the warning during compiling
        //if (ishell==1) tlogicLdSh->SetVisAttributes(visAttLad1);
        //else tlogicLdSh->SetVisAttributes(visAttLad);
        iYpos += shlThckHlf;
      }
    }
    LadderInfo.logical->SetVisAttributes(visAttLad);

  }
  return LadderInfo;
}

void PSHWBuilder::instantiateSensitiveDetectors( const std::string hitsCollectionName){

  crd::SimpleConfig const& config  = GeomService::Instance()->getConfig();

  G4SDManager* SDman      = G4SDManager::GetSDMpointer();

  // G4 takes ownership and will delete the detectors at the job end

  GeomHandle<PSHWtracker> pshwtracker;
  G4ThreeVector pshwPos(0.,0.,CLHEP::mm * pshwtracker->z0()/*-zOff*/);

  PSHWLadderSD* pshwtrackerSD=0x0;
  pshwtrackerSD = new PSHWLadderSD(SensitiveDetectorName::PSHWTrackerRO(), hitsCollectionName,  config);
  pshwtrackerSD->setPSHWCenterInDetSys(pshwPos);
  SDman->AddNewDetector(pshwtrackerSD);

}

} // end namespace pshw
