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
// -------------------------------------------------------------------
// $Id: MicrobeamSteppingAction.cc,v 1.8 2007/08/22 13:58:33 sincerti Exp $
// -------------------------------------------------------------------

#include "G4SteppingManager.hh"

#include "MicrobeamSteppingAction.hh"
#include "MicrobeamRunAction.hh"
#include "MicrobeamDetectorConstruction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicrobeamSteppingAction::MicrobeamSteppingAction(MicrobeamRunAction* run,MicrobeamDetectorConstruction* det)
:Run(run),Detector(det)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

MicrobeamSteppingAction::~MicrobeamSteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void MicrobeamSteppingAction::UserSteppingAction(const G4Step* aStep)
  
{ 

// COUNT GAS DETECTOR HITS

if (       ((aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "_CollDet_yoke_")
        &&  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "Isobutane")
        &&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "alpha"))
	
        || 
	   ((aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "_CollDet_gap4_")
        &&  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "Isobutane")
	&&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "alpha"))

	||

    	   ((aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "_CollDet_gap5_")
        &&  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "Isobutane")
	&&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "alpha"))

    )
	{
	  Run->AddNbOfHitsGas();	
	}
	
// STOPPING POWER AND BEAM SPOT SIZE AT CELL ENTRANCE

if (       ((aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "Polyprop")
        &&  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "KGM")
        &&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "alpha"))
	
        || 
	   ((aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "Polyprop")
        &&  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "physicalCytoplasm")
	&&  (aStep->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "alpha"))
    )
	{
	
	if(aStep->GetTotalEnergyDeposit()>0) 
	 {
	  FILE *myFile;
	  myFile=fopen("stoppingPower.txt","a");	
	  fprintf(myFile,"%e \n",
	  (aStep->GetTotalEnergyDeposit()/keV)/(aStep->GetStepLength()/micrometer));	
	  fclose (myFile);
	 }

         // Average dE over step syggested by Michel Maire

	 G4StepPoint* p1 = aStep->GetPreStepPoint();
         G4ThreeVector coord1 = p1->GetPosition();
         const G4AffineTransform transformation1 = p1->GetTouchable()->GetHistory()->GetTopTransform();
         G4ThreeVector localPosition1 = transformation1.TransformPoint(coord1);

	 G4StepPoint* p2 = aStep->GetPostStepPoint();
         G4ThreeVector coord2 = p2->GetPosition();
         const G4AffineTransform transformation2 = p2->GetTouchable()->GetHistory()->GetTopTransform();
         G4ThreeVector localPosition2 = transformation2.TransformPoint(coord2);

         G4ThreeVector localPosition = localPosition1 + G4UniformRand()*(localPosition2-localPosition1);
	 
	 // end

	 FILE *myFile;
	 myFile=fopen("beamPosition.txt","a");
	 fprintf 
	 ( 
	  myFile,"%e %e \n",
	  localPosition.x()/micrometer,
	  localPosition.y()/micrometer
	 );
	 fclose (myFile);				

	}

// ALPHA RANGE

if (

	(aStep->GetTrack()->GetDynamicParticle()->GetDefinition() ->GetParticleName() == "alpha")
	
	&&
	
	(aStep->GetTrack()->GetKineticEnergy()<1e-6)
	
	&&
			
          ( (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "physicalCytoplasm")
	||  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "KGM")	
	||  (aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "physicalNucleus") )	
		
   )
	
	{		
	 FILE *myFile;
	 myFile=fopen("range.txt","a");
	 fprintf 
	 ( myFile,"%e %e %e\n",
	  (aStep->GetTrack()->GetPosition().x())/micrometer,
	  (aStep->GetTrack()->GetPosition().y())/micrometer,
	  (aStep->GetTrack()->GetPosition().z())/micrometer
	 );
	 fclose (myFile);				
 	}

// TOTAL DOSE DEPOSIT AND DOSE DEPOSIT WITHIN A PHANTOM VOXEL

if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()  == "physicalNucleus")

	{ 
   	 G4double dose = (e_SI*(aStep->GetTotalEnergyDeposit()/eV))/(Run->GetMassNucleus());
   	 Run->AddDoseN(dose);

	 G4ThreeVector v;
    	 Run->AddDoseBox(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(),
	  aStep->GetTotalEnergyDeposit()/eV);
	}


if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()  == "physicalCytoplasm")

	{ 
   	 G4double dose = (e_SI*(aStep->GetTotalEnergyDeposit()/eV))/(Run->GetMassCytoplasm());
   	 Run->AddDoseC(dose);

	 G4ThreeVector v;
    	 Run->AddDoseBox(aStep->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber(),
	  aStep->GetTotalEnergyDeposit()/eV);
 	}
}