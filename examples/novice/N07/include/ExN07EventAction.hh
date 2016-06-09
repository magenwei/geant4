//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: ExN07EventAction.hh,v 1.2 2003/04/08 15:47:00 asaim Exp $
// GEANT4 tag $Name: geant4-05-02 $
//

#ifndef ExN07EventAction_h
#define ExN07EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class ExN07EventAction : public G4UserEventAction
{
  public:
    ExN07EventAction();
    virtual ~ExN07EventAction();

  public:
    virtual void   BeginOfEventAction(const G4Event*);
    virtual void   EndOfEventAction(const G4Event*);
    
  private:
    G4int          calorimeterCollID[6];                
    static G4int   verboseLevel;

  public:
    static void SetVerboseLevel(G4int i)
    { verboseLevel = i; }
    static G4int GetVerboseLevel()
    { return verboseLevel; }
};


#endif

    