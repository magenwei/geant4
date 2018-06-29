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
// $Id: G4eplusTo3GammaOKVIModel.cc 101193 2016-11-08 18:02:50Z vnivanch $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:   G4eplusTo3GammaOKVIModel
//
// Author:      Vladimir Ivanchenko and Omrame Kadri
//
// Creation date: 29.03.2018
//
// -------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4eplusTo3GammaOKVIModel.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4EmParameters.hh"
#include "G4TrackStatus.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "Randomize.hh"
#include "G4ParticleChangeForGamma.hh"
#include "G4Log.hh"
#include "G4Exp.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

using namespace std;

G4eplusTo3GammaOKVIModel::G4eplusTo3GammaOKVIModel(const G4ParticleDefinition*,
                                                   const G4String& nam)
  : G4VEmModel(nam),
    pi_rcl2(pi*classic_electr_radius*classic_electr_radius),
    energyTh(10*MeV)
{
  theGamma = G4Gamma::Gamma();
  fParticleChange = nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4eplusTo3GammaOKVIModel::~G4eplusTo3GammaOKVIModel()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4eplusTo3GammaOKVIModel::Initialise(const G4ParticleDefinition*,
					  const G4DataVector&)
{
  energyTh = G4EmParameters::Instance()->LowestTripletEnergy();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double 
G4eplusTo3GammaOKVIModel::ComputeCrossSectionPerElectron(G4double kineticEnergy)
{
  // Calculates the cross section per electron of annihilation into two photons
  // from the Heilter formula.

  G4double ekin  = std::max(eV,kineticEnergy);   

  G4double tau   = ekin/electron_mass_c2;
  G4double gam   = tau + 1.0;
  G4double gamma2= gam*gam;
  G4double bg2   = tau * (tau+2.0);
  G4double bg    = sqrt(bg2);

  G4double cross = pi_rcl2*((gamma2+4*gam+1.)*G4Log(gam+bg) - (gam+3.)*bg)
                 / (bg2*(gam+1.));
  return cross;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4eplusTo3GammaOKVIModel::ComputeCrossSectionPerAtom(
                                    const G4ParticleDefinition*,
                                    G4double kineticEnergy, G4double Z,
				    G4double, G4double, G4double)
{
  // Calculates the cross section per atom of annihilation into two photons
  
  G4double cross = Z*ComputeCrossSectionPerElectron(kineticEnergy);
  return cross;  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4eplusTo3GammaOKVIModel::CrossSectionPerVolume(
					const G4Material* material,
					const G4ParticleDefinition*,
					      G4double kineticEnergy,
					      G4double, G4double)
{
  // Calculates the cross section per volume of annihilation into two photons
  
  G4double eDensity = material->GetElectronDensity();
  G4double cross = eDensity*ComputeCrossSectionPerElectron(kineticEnergy);
  return cross;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Polarisation of gamma according to M.H.L.Pryce and J.C.Ward, 
// Nature 4065 (1947) 435.

void 
G4eplusTo3GammaOKVIModel::SampleSecondaries(vector<G4DynamicParticle*>* vdp,
					    const G4MaterialCutsCouple*,
					    const G4DynamicParticle* dp,
					    G4double, G4double)
{
  G4double posiKinEnergy = dp->GetKineticEnergy();
  G4DynamicParticle *aGamma1, *aGamma2, *aGamma3;

  CLHEP::HepRandomEngine* rndmEngine = G4Random::getTheEngine();
   
  // Case at rest
  if(posiKinEnergy == 0.0) {
    G4double cost = 2.*rndmEngine->flat()-1.;
    G4double sint = sqrt((1. - cost)*(1. + cost));
    G4double phi  = twopi * rndmEngine->flat();
    G4ThreeVector dir(sint*cos(phi), sint*sin(phi), cost);
    phi = twopi * rndmEngine->flat();
    G4double cosphi = cos(phi);
    G4double sinphi = sin(phi);
    G4ThreeVector pol(cosphi, sinphi, 0.0);
    pol.rotateUz(dir);
    aGamma1 = new G4DynamicParticle(theGamma, dir, electron_mass_c2);
    aGamma1->SetPolarization(pol.x(),pol.y(),pol.z());
    aGamma2 = new G4DynamicParticle(theGamma,-dir, electron_mass_c2);
    pol.set(-sinphi, cosphi, 0.0);
    pol.rotateUz(dir);
    aGamma2->SetPolarization(pol.x(),pol.y(),pol.z());

  } else {

    G4ThreeVector posiDirection = dp->GetMomentumDirection();

    G4double tau     = posiKinEnergy/electron_mass_c2;
    G4double gam     = tau + 1.0;
    G4double tau2    = tau + 2.0;
    G4double sqgrate = sqrt(tau/tau2)*0.5;
    G4double sqg2m1  = sqrt(tau*tau2);

    // limits of the energy sampling
    G4double epsilmin = 0.5 - sqgrate;
    G4double epsilmax = 0.5 + sqgrate;
    G4double epsilqot = epsilmax/epsilmin;

    //
    // sample the energy rate of the created gammas
    //
    G4double epsil, greject;

    do {
      epsil = epsilmin*G4Exp(G4Log(epsilqot)*rndmEngine->flat());
      greject = 1. - epsil + (2.*gam*epsil-1.)/(epsil*tau2*tau2);
      // Loop checking, 03-Aug-2015, Vladimir Ivanchenko
    } while( greject < rndmEngine->flat());

    //
    // scattered Gamma angles. ( Z - axis along the parent positron)
    //

    G4double cost = (epsil*tau2-1.)/(epsil*sqg2m1);
    if(std::abs(cost) > 1.0) {
      G4cout << "### G4eplusTo3GammaOKVIModel WARNING cost= " << cost
	     << " positron Ekin(MeV)= " << posiKinEnergy
	     << " gamma epsil= " << epsil
	     << G4endl;
      if(cost > 1.0) cost = 1.0;
      else cost = -1.0; 
    }
    G4double sint = sqrt((1.+cost)*(1.-cost));
    G4double phi  = twopi * rndmEngine->flat();

    //
    // kinematic of the created pair
    //

    G4double TotalAvailableEnergy = posiKinEnergy + 2.0*electron_mass_c2;
    G4double phot1Energy = epsil*TotalAvailableEnergy;

    G4ThreeVector phot1Direction(sint*cos(phi), sint*sin(phi), cost);
    phot1Direction.rotateUz(posiDirection);
    aGamma1 = new G4DynamicParticle (theGamma,phot1Direction, phot1Energy);
    phi = twopi * rndmEngine->flat();
    G4double cosphi = cos(phi);
    G4double sinphi = sin(phi);
    G4ThreeVector pol(cosphi, sinphi, 0.0);
    pol.rotateUz(phot1Direction);
    aGamma1->SetPolarization(pol.x(),pol.y(),pol.z());

    G4double phot2Energy =(1.-epsil)*TotalAvailableEnergy;
    G4double posiP= sqrt(posiKinEnergy*(posiKinEnergy+2.*electron_mass_c2));
    G4ThreeVector dir = posiDirection*posiP - phot1Direction*phot1Energy;
    G4ThreeVector phot2Direction = dir.unit();

    // create G4DynamicParticle object for the particle2
    aGamma2 = new G4DynamicParticle (theGamma,phot2Direction, phot2Energy);

    //!!! likely problematic direction to be checked
    pol.set(-sinphi, cosphi, 0.0);
    pol.rotateUz(phot1Direction);
    cost = pol*phot2Direction;
    pol -= cost*phot2Direction;
    pol = pol.unit();
    aGamma2->SetPolarization(pol.x(),pol.y(),pol.z());
  }
  /*
    G4cout << "Annihilation in fly: e0= " << posiKinEnergy
    << " m= " << electron_mass_c2
    << " e1= " << phot1Energy 
    << " e2= " << phot2Energy << " dir= " <<  dir 
    << " -> " << phot1Direction << " " 
    << phot2Direction << G4endl;
  */
 
  vdp->push_back(aGamma1);
  vdp->push_back(aGamma2);
  vdp->push_back(aGamma3);

  // kill primary positron
  fParticleChange->SetProposedKineticEnergy(0.0);
  fParticleChange->ProposeTrackStatus(fStopAndKill);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....