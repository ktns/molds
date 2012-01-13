//************************************************************************//
// Copyright (C) 2011-2012 Mikiya Fujii                                   // 
//                                                                        // 
// This file is part of MolDS.                                            // 
//                                                                        // 
// MolDS is free software: you can redistribute it and/or modify          // 
// it under the terms of the GNU General Public License as published by   // 
// the Free Software Foundation, either version 3 of the License, or      // 
// (at your option) any later version.                                    // 
//                                                                        // 
// MolDS is distributed in the hope that it will be useful,               // 
// but WITHOUT ANY WARRANTY; without even the implied warranty of         // 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          // 
// GNU General Public License for more details.                           // 
//                                                                        // 
// You should have received a copy of the GNU General Public License      // 
// along with MolDS.  If not, see <http://www.gnu.org/licenses/>.         // 
//************************************************************************//
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<sstream>
#include<math.h>
#include<string>
#include<vector>
#include"../base/Uncopyable.h"
#include"../base/Enums.h"
#include"../base/EularAngle.h"
#include"../base/Parameters.h"
#include"../base/atoms/Atom.h"
#include"../base/atoms/Hatom.h"
#include"../base/atoms/Liatom.h"
#include"../base/atoms/Catom.h"
#include"../base/atoms/Natom.h"
#include"../base/atoms/Oatom.h"
#include"../base/atoms/Satom.h"
#include"../base/Molecule.h"
#include"../base/ElectronicStructure.h"
#include"../cndo/Cndo2.h"
#include"../zindo/ZindoS.h"
#include"../mndo/Mndo.h"
#include"../am1/Am1.h"
#include"Pm3.h"
#include"Pm3Pddg.h"
using namespace std;
using namespace MolDS_base;
using namespace MolDS_base_atoms;

namespace MolDS_pm3{

/***
 *  Main Refferences for PM3/PDDG are [RCJ_2002, BGRJ_2003, and BGJ_2003]
 */
Pm3Pddg::Pm3Pddg() : MolDS_pm3::Pm3(){
   this->theory = PM3PDDG;
   this->SetMessages();
   this->SetEnableAtomTypes();
   //cout << "Pm3Pddg created\n";
}

Pm3Pddg::~Pm3Pddg(){
   //cout << "Pm3Pddg deleted\n";
}

void Pm3Pddg::SetMessages(){
   this->errorMessageSCFNotConverged 
      = "Error in pm3::Pm3Pddg::DoSCF: SCF did not met convergence criterion. maxIterationsSCF=";
   this->errorMessageMoleculeNotSet 
      = "Error in pm3::Pm3Pddg::DoSCF: A molecule is not set.\n";
   this->errorMessageOddTotalValenceElectrions 
      = "Error in pm3::Pm3Pddg::SetMolecule: Total number of valence electrons is odd. totalNumberValenceElectrons=";
   this->errorMessageNotEnebleAtomType  
      = "Error in pm3::Pm3Pddg::CheckEnableAtomType: Non available atom is contained.\n";
   this->errorMessageCoulombInt = "Error in base_pm3::Pm3Pddg::GetCoulombInt: Invalid orbitalType.\n";
   this->errorMessageExchangeInt = "Error in base_pm3::Pm3Pddg::GetExchangeInt: Invalid orbitalType.\n";
   this->errorMessageCalcCISMatrix
      = "Error in pm3::Pm3Pddg::CalcCISMatrix: Non available orbital is contained.\n";
   this->errorMessageDavidsonNotConverged =  "Error in pm3::Pm3Pddg::DoCISDavidson: Davidson did not met convergence criterion. \n";
   this->errorMessageGetSemiEmpiricalMultipoleInteractionBadMultipoles
      = "Error in pm3:: Pm3Pddg::GetSemiEmpiricalMultipoleInteraction: Bad multipole combintaion is set\n";
   this->errorMessageGetSemiEmpiricalMultipoleInteractionFirstDeriBadMultipoles
      = "Error in pm3:: Pm3Pddg::GetSemiEmpiricalMultipoleInteractionFirstDerivative: Bad multipole combintaion is set\n";
   this->errorMessageGetNddoRepulsionIntegral 
      = "Error in pm3::Pm3Pddg::GetNddoRepulsionIntegral: Bad orbital is set.\n";
   this->errorMessageGetNddoRepulsionIntegralFirstDerivative 
      = "Error in pm3::Pm3Pddg::GetNddoRepulsionIntegralFirstDerivative: Bad orbital is set.\n";
   this->errorMessageCalcTwoElecTwoCoreNullMatrix 
      = "Error in pm3::Pm3Pddg::CalcTwoElecTwoCore: The two elec two core matrix is NULL.\n"; 
   this->errorMessageCalcTwoElecTwoCoreDiatomicNullMatrix 
      = "Error in pm3::Pm3Pddg::CalcTwoElecTwoCoreDiatomic: The two elec two core diatomic matrix is NULL.\n"; 
   this->errorMessageCalcTwoElecTwoCoreDiatomicSameAtoms
      = "Error in pm3::Pm3Pddg::CalcTwoElecTwoCoreDiatomic: Atom A and B is same.\n"; 
   this->errorMessageCalcTwoElecTwoCoreDiatomicFirstDerivativesNullMatrix
      = "Error in pm3::Pm3Pddg::CalcTwoElecTwoCoreDiatomicFirstDerivatives: The two elec two core diatomic matrix is NULL.\n"; 
   this->errorMessageCalcTwoElecTwoCoreDiatomicFirstDerivativesSameAtoms
      = "Error in pm3::Pm3Pddg::CalcTwoElecTwoCoreDiatomicFirstDerivatives: Atom A and B is same.\n"; 
   this->errorMessageGetElectronicEnergyEnergyNotCalculated
      = "Error in pm3::Pm3Pddg::GetElectronicEnergy: Set electronic state is not calculated by CIS.\n";
   this->errorMessageGetElectronicEnergyNULLCISEnergy 
      = "Error in pm3::Pm3Pddg::GetElectronicEnergy: excitedEnergies is NULL\n";
   this->messageSCFMetConvergence = "\n\n\n\t\tPM3/PDDG-SCF met convergence criterion(^^b\n\n\n";
   this->messageStartSCF = "**********  START: PM3/PDDG-SCF  **********\n";
   this->messageDoneSCF = "**********  DONE: PM3/PDDG-SCF  **********\n\n\n";
   this->messageStartCIS = "**********  START: PM3/PDDG-CIS  **********\n";
   this->messageDoneCIS = "**********  DONE: PM3/PDDG-CIS  **********\n\n\n";
   this->messageDavidsonConverge = "\n\n\t\tDavidson for PM3/PDDG-CIS met convergence criterion(^^b\n\n\n";
}

void Pm3Pddg::SetEnableAtomTypes(){
   this->enableAtomTypes.clear();
   this->enableAtomTypes.push_back(H);
   this->enableAtomTypes.push_back(C);
   this->enableAtomTypes.push_back(N);
   this->enableAtomTypes.push_back(O);
   this->enableAtomTypes.push_back(S);
}

double Pm3Pddg::GetDiatomCoreRepulsionEnergy(int indexAtomA, int indexAtomB) const{
   double energy = Pm3::GetDiatomCoreRepulsionEnergy(indexAtomA, indexAtomB);
   const Atom& atomA = *(*this->molecule->GetAtomVect())[indexAtomA];
   const Atom& atomB = *(*this->molecule->GetAtomVect())[indexAtomB];
   double na = (double)(atomA.GetNumberValenceElectrons());
   double nb = (double)(atomB.GetNumberValenceElectrons());
   double distance = this->molecule->GetDistanceAtoms(indexAtomA, indexAtomB);
   double temp = 0.0;
   for(int i=0; i<2; i++){
      double pa = atomA.GetPm3PddgParameterPa(i);
      double da = atomA.GetPm3PddgParameterDa(i);
      for(int j=0; j<2; j++){
         double pb = atomB.GetPm3PddgParameterPa(j);
         double db = atomB.GetPm3PddgParameterDa(j);
         temp += (na*pa +nb*pb)*exp(-10.0*pow((distance-da-db),2.0));
      }
   }
   energy += temp/(na+nb);
   return energy;
}

// First derivative of diatomic core repulsion energy.
// This derivative is related to the coordinate of atomA.
double Pm3Pddg::GetDiatomCoreRepulsionFirstDerivative(int atomAIndex,
                                                      int atomBIndex, 
                                                      CartesianType axisA) const{
   double value = Pm3::GetDiatomCoreRepulsionFirstDerivative(atomAIndex,
                                                             atomBIndex,
                                                             axisA);
   const Atom& atomA = *(*this->molecule->GetAtomVect())[atomAIndex];
   const Atom& atomB = *(*this->molecule->GetAtomVect())[atomBIndex];
   double Rab = this->molecule->GetDistanceAtoms(atomAIndex, atomBIndex);
   double dRabDa = (atomA.GetXyz()[axisA] - atomB.GetXyz()[axisA])/Rab;
   double na = (double)(atomA.GetNumberValenceElectrons());
   double nb = (double)(atomB.GetNumberValenceElectrons());
   double temp = 0.0;
   for(int i=0; i<2; i++){
      double pa = atomA.GetPm3PddgParameterPa(i);
      double da = atomA.GetPm3PddgParameterDa(i);
      for(int j=0; j<2; j++){
         double pb = atomB.GetPm3PddgParameterPa(j);
         double db = atomB.GetPm3PddgParameterDa(j);
         temp += (na*pa +nb*pb)*exp(-10.0*pow((Rab-da-db),2.0))
                *(-20.0*(Rab-da-db));
      }
   }
   value += temp*dRabDa/(na+nb);
   return value;
}


}



