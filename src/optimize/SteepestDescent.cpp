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
#include<stdexcept>
#include<boost/shared_ptr.hpp>
#include<boost/format.hpp>
#include"../base/PrintController.h"
#include"../base/MolDSException.h"
#include"../base/Uncopyable.h"
#include"../base/Enums.h"
#include"../base/EularAngle.h"
#include"../base/Parameters.h"
#include"../base/atoms/Atom.h"
#include"../base/Molecule.h"
#include"../base/ElectronicStructure.h"
#include"../base/ElectronicStructureFactory.h"
#include"SteepestDescent.h"
using namespace std;
using namespace MolDS_base;
using namespace MolDS_base_atoms;

namespace MolDS_optimize{
SteepestDescent::SteepestDescent(){
   this->SetMessages();
   this->SetEnableTheoryTypes();
   //printf("%s\n", "SteepestDescent created");
}

SteepestDescent::~SteepestDescent(){
   //printf("%s\n", "SteepestDescent deleted");
}

void SteepestDescent::Optimize(Molecule& molecule){
   if(this->PrintsLogs()){
      printf("%s",this->messageStartGeometryOptimization.c_str());
   }
   this->ClearMolecularMomenta(molecule);

   // malloc electornic structure
   TheoryType theory = Parameters::GetInstance()->GetCurrentTheory();
   this->CheckEnableTheoryType(theory);
   boost::shared_ptr<ElectronicStructure> electronicStructure(ElectronicStructureFactory::GetInstance()->Create());
   electronicStructure->SetMolecule(&molecule);
   electronicStructure->SetPrintsLogs(this->PrintsLogs());

   // Search Minimum
   double lineSearchedEnergy = 0.0;
   bool obtainesOptimizedStructure = false;
   this->LineSearch(electronicStructure, molecule, &lineSearchedEnergy, &obtainesOptimizedStructure);
   if(!obtainesOptimizedStructure){
      this->SteepestDescentSearch(electronicStructure, molecule, lineSearchedEnergy, &obtainesOptimizedStructure);
   }
  
   // Not converged
   if(!obtainesOptimizedStructure){
      int lineSearchTimes = Parameters::GetInstance()->GetLineSearchTimesSteepestDescent();
      int steepSteps = Parameters::GetInstance()->GetStepsSteepestDescent();
      stringstream ss;
      ss << this->errorMessageGeometyrOptimizationNotConverged;
      ss << this->errorMessageLineSearchTimes << lineSearchTimes << endl;
      ss << this->errorMessageSteepestDescentSteps << steepSteps << endl;
      throw MolDSException(ss.str());
   }
   if(this->PrintsLogs()){
      printf("%s",this->messageEndGeometryOptimization.c_str());
   }
}

void SteepestDescent::SetMessages(){
   this->errorMessageTheoryType = "\ttheory type = ";
   this->errorMessageNotEnebleTheoryType  
      = "Error in optimize::SteepestDescent::CheckEnableTheoryType: Non available theory is set.\n";
   this->errorMessageGeometyrOptimizationNotConverged 
      = "Error in optimize::SteepestDescent::Optimize: Optimization did not met convergence criterion.\n";
   this->errorMessageLineSearchTimes = "\tLine search times = ";
   this->errorMessageSteepestDescentSteps = "\tSteepest descent steps = ";
   this->messageGeometyrOptimizationMetConvergence 
      = "\t\tGeometry otimization met convergence criterion(^^b\n\n\n";
   this->messageStartGeometryOptimization = "**********  START: Geometry optimization  **********\n";
   this->messageEndGeometryOptimization =   "**********  DONE: Geometry optimization  **********\n";
   this->messageStartSteepestDescent = "**********  START: Steepest descent  **********\n";
   this->messageEndSteepestDescent =   "**********  DONE: Steepest descent  **********\n";
   this->messageStartStepSteepestDescent = "==========  START: Steepest descent step ";
   this->messageStartLineSearch = "**********  START: Line search  **********\n";
   this->messageEndLineSearch =   "**********  DONE: Line search  **********\n";
   this->messageStartLineSearchTimes = "\n==========  START: Line return times ";
   this->messageLineSearchSteps = "Number of steps in this Line search: ";
   this->messageOptimizationLog = "====== Optimization Logs ======\n";
   this->messageEnergyDifference = "Energy difference: ";
   this->messageMaxGradient = "Max gradient: ";
   this->messageRmsGradient = "Rms gradient: ";
   this->messageAu = "[a.u.]";
}

void SteepestDescent::SetEnableTheoryTypes(){
   this->enableTheoryTypes.clear();
   this->enableTheoryTypes.push_back(ZINDOS);
   this->enableTheoryTypes.push_back(MNDO);
   this->enableTheoryTypes.push_back(AM1);
   this->enableTheoryTypes.push_back(PM3);
   this->enableTheoryTypes.push_back(PM3PDDG);
}

void SteepestDescent::CheckEnableTheoryType(TheoryType theoryType) const{
   bool isEnable = false;
   for(int i=0; i<this->enableTheoryTypes.size();i++){
      if(theoryType == this->enableTheoryTypes[i]){
         isEnable = true;
         break;
      }
   }
   if(!isEnable){
      stringstream ss;
      ss << this->errorMessageNotEnebleTheoryType;
      ss << this->errorMessageTheoryType << TheoryTypeStr(theoryType) << endl;
      throw MolDSException(ss.str());
   }
}

void SteepestDescent::ClearMolecularMomenta(Molecule& molecule) const{
   for(int a=0; a<molecule.GetAtomVect()->size(); a++){
      const Atom* atom = (*molecule.GetAtomVect())[a];
      atom->SetPxyz(0.0, 0.0, 0.0);
   }
}

void SteepestDescent::UpdateMolecularCoordinates(Molecule& molecule, double** matrixForce, double dt) const{
   for(int a=0; a<molecule.GetAtomVect()->size(); a++){
      const Atom* atom = (*molecule.GetAtomVect())[a];
      double coreMass = atom->GetAtomicMass() - (double)atom->GetNumberValenceElectrons();
      for(int i=0; i<CartesianType_end; i++){
         atom->GetXyz()[i] += dt*matrixForce[a][i]/coreMass;
      }
   }
   molecule.CalcXyzCOM();
   molecule.CalcXyzCOC();
}

void SteepestDescent::UpdateElectronicStructure(boost::shared_ptr<ElectronicStructure> electronicStructure, 
                                                bool requireGuess, 
                                                bool printsLogs) const{
   electronicStructure->SetPrintsLogs(printsLogs);
   electronicStructure->DoSCF(requireGuess);
   if(Parameters::GetInstance()->RequiresCIS()){
      electronicStructure->DoCIS();
   }
}

void SteepestDescent::LineSearch(boost::shared_ptr<ElectronicStructure> electronicStructure, 
                                 Molecule& molecule,
                                 double* lineSearchedEnergy,
                                 bool* obtainesOptimizedStructure) const{
   if(this->PrintsLogs()){
      printf("%s",this->messageStartLineSearch.c_str());
   }
   int elecState = Parameters::GetInstance()->GetElectronicStateIndexSteepestDescent();
   double dt = Parameters::GetInstance()->GetTimeWidthSteepestDescent();
   int lineSearchTimes = Parameters::GetInstance()->GetLineSearchTimesSteepestDescent();
   double maxGradientThreshold = Parameters::GetInstance()->GetMaxGradientSteepestDescent();
   double rmsGradientThreshold = Parameters::GetInstance()->GetRmsGradientSteepestDescent();
   double lineSearchCurrentEnergy = 0.0;
   double lineSearchInitialEnergy = 0.0;
   double** matrixForce = NULL;

   // initial calculation
   bool requireGuess = true;
   this->UpdateElectronicStructure(electronicStructure, requireGuess, this->PrintsLogs());
   lineSearchCurrentEnergy = electronicStructure->GetElectronicEnergy(elecState);

   if(0<lineSearchTimes){
      requireGuess = false;
      matrixForce = electronicStructure->GetForce(elecState);
      for(int s=0; s<lineSearchTimes; s++){
         if(this->PrintsLogs()){
            printf("%s%d\n",this->messageStartLineSearchTimes.c_str(),s);
         }

         lineSearchInitialEnergy = lineSearchCurrentEnergy;

         // line search roop
         int lineSearchSteps = 0;
         double lineSearchOldEnergy = lineSearchCurrentEnergy;
         while(lineSearchCurrentEnergy <= lineSearchOldEnergy){
            this->UpdateMolecularCoordinates(molecule, matrixForce, dt);
            bool tempPrintsLogs = false;
            this->UpdateElectronicStructure(electronicStructure, requireGuess, tempPrintsLogs);
            lineSearchOldEnergy = lineSearchCurrentEnergy;
            lineSearchCurrentEnergy = electronicStructure->GetElectronicEnergy(elecState);
            lineSearchSteps++;
         }

         // final state of line search
         this->UpdateElectronicStructure(electronicStructure, requireGuess, this->PrintsLogs());
         matrixForce = electronicStructure->GetForce(elecState);
         if(this->PrintsLogs()){
            molecule.OutputConfiguration();
            molecule.OutputXyzCOC();
         }
         lineSearchCurrentEnergy = electronicStructure->GetElectronicEnergy(elecState);
         if(this->PrintsLogs()){
            printf("\t%s%d\n",this->messageLineSearchSteps.c_str(), lineSearchSteps);
         }

         // check convergence
         if(this->SatisfiesConvergenceCriterion(matrixForce, 
                                                molecule,
                                                lineSearchInitialEnergy, 
                                                lineSearchCurrentEnergy,
                                                maxGradientThreshold, 
                                                rmsGradientThreshold)){
            *obtainesOptimizedStructure = true;
            break;
         }

      }
   }
   *lineSearchedEnergy = lineSearchCurrentEnergy;
   if(this->PrintsLogs()){
      printf("%s", this->messageEndLineSearch.c_str());
   }
}


// This method should be called after the LineSearch-method.
void SteepestDescent::SteepestDescentSearch(boost::shared_ptr<ElectronicStructure> electronicStructure, 
                                            Molecule& molecule,
                                            double lineSearchedEnergy,
                                            bool* obtainesOptimizedStructure) const{
   int elecState = Parameters::GetInstance()->GetElectronicStateIndexSteepestDescent();
   double dt = Parameters::GetInstance()->GetTimeWidthSteepestDescent();
   int steepSteps = Parameters::GetInstance()->GetStepsSteepestDescent();
   double maxGradientThreshold = Parameters::GetInstance()->GetMaxGradientSteepestDescent();
   double rmsGradientThreshold = Parameters::GetInstance()->GetRmsGradientSteepestDescent();
   double currentEnergy = lineSearchedEnergy;
   double oldEnergy = 0.0;
   double** matrixForce = NULL;
   double** oldMatrixForce = NULL;
   bool requireGuess = false;
   if(this->PrintsLogs()){
      printf("%s",this->messageStartSteepestDescent.c_str());
   }
   matrixForce = electronicStructure->GetForce(elecState);

   // steepest descent roop
   for(int s=0; s<steepSteps; s++){
      if(this->PrintsLogs()){
         printf("%s %d\n",this->messageStartStepSteepestDescent.c_str(), s+1);
      }
      this->UpdateMolecularCoordinates(molecule, matrixForce, dt);
      this->UpdateElectronicStructure(electronicStructure, requireGuess, this->PrintsLogs());
      oldEnergy = currentEnergy;
      currentEnergy = electronicStructure->GetElectronicEnergy(elecState);
      matrixForce = electronicStructure->GetForce(elecState);
      if(this->SatisfiesConvergenceCriterion(matrixForce, 
                                             molecule, 
                                             oldEnergy, 
                                             currentEnergy, 
                                             maxGradientThreshold, 
                                             rmsGradientThreshold)){
         *obtainesOptimizedStructure = true;
         break;
      }
      if(oldEnergy < currentEnergy){
         dt *= 0.1;
         if(this->PrintsLogs()){
            printf("%s %e\n","dt is reduced to ", dt);
         }
      }
   }

   if(this->PrintsLogs()){
      printf("%s",this->messageEndSteepestDescent.c_str());
   }
}

bool SteepestDescent::SatisfiesConvergenceCriterion(double** matrixForce, 
                                                    const MolDS_base::Molecule& molecule,
                                                    double oldEnergy,
                                                    double currentEnergy,
                                                    double maxGradientThreshold,
                                                    double rmsGradientThreshold) const{
   bool satisfies = false;
   double maxGradient = 0.0;
   double sumSqureGradient = 0.0;
   double energyDifference = currentEnergy - oldEnergy;
   for(int a=0; a<molecule.GetAtomVect()->size(); a++){
      for(int i=0; i<CartesianType_end; i++){
         if(maxGradient<fabs(matrixForce[a][i])){
            maxGradient = fabs(matrixForce[a][i]);
         }
         sumSqureGradient += pow(matrixForce[a][i],2.0);
      }
   }
   sumSqureGradient /= (double)(molecule.GetAtomVect()->size()*CartesianType_end);
   double rmsGradient = sqrt(sumSqureGradient);
   if(this->PrintsLogs()){
      printf("\n");
      printf("\t%s", this->messageOptimizationLog.c_str());
      printf("\t%s %e%s\n", this->messageEnergyDifference.c_str(), energyDifference, this->messageAu.c_str());
      printf("\t%s %e%s\n", this->messageMaxGradient.c_str(), maxGradient, this->messageAu.c_str());
      printf("\t%s %e%s\n", this->messageRmsGradient.c_str(), rmsGradient, this->messageAu.c_str());
      printf("\n\n");
      //cout << (boost::format("\t%s %e%s") % this->messageEnergyDifference.c_str() % energyDifference % this->messageAu.c_str()).str();
   }
   if(maxGradient < maxGradientThreshold && rmsGradient < rmsGradientThreshold && energyDifference < 0){
      printf("%s", this->messageGeometyrOptimizationMetConvergence.c_str());
      satisfies = true;
   }
   return satisfies;
}

}



