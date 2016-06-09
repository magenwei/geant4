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
// $Id: RemSimRunAction.hh,v 1.9 2005/09/08 06:56:18 guatelli Exp $
// GEANT4 tag $Name: geant4-08-00 $
//
// Code developed by: S.Guatelli, guatelli@ge.infn.it
//
//    ********************************************
//    *                                          *
//    *      RemSimRunAction.hh                  *
//    *                                          *
//    ********************************************
//
#ifndef RemSimRunAction_h
#define RemSimRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4RunManager.hh"
#include "globals.hh"

class G4Run;
class RemSimAnalysisManager;

class RemSimRunAction : public G4UserRunAction
{
public:
  RemSimRunAction();
  ~RemSimRunAction();

public:
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run* );
  G4int GetRunID() {return runID;};

private:
  G4int runID;
};
#endif


