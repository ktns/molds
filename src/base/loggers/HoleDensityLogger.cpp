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
#include<fstream>
#include<string>
#include<string.h>
#include<math.h>
#include<vector>
#include<stdexcept>
#include<omp.h>
#include<boost/format.hpp>
#include"../PrintController.h"
#include"../MolDSException.h"
#include"../Uncopyable.h"
#include"../Utilities.h"
#include"../Enums.h"
#include"../EularAngle.h"
#include"../Parameters.h"
#include"../atoms/Atom.h"
#include"../Molecule.h"
#include"DensityLogger.h"
#include"HoleDensityLogger.h"
using namespace std;
using namespace MolDS_base;
using namespace MolDS_base_atoms;
namespace MolDS_base_loggers{

HoleDensityLogger::HoleDensityLogger(const Molecule& molecule, 
                                     double const* const* fockMatrix, 
                                     double const* const* cisMatrix, 
                                     TheoryType theory) 
                                     : DensityLogger(molecule, fockMatrix, cisMatrix, theory){
   this->SetMessages();
   //this->OutputLog("Hole density logger is created.\n");
}

HoleDensityLogger::HoleDensityLogger(){
   //this->OutputLog("Hole density logger is created.\n");
}

HoleDensityLogger::~HoleDensityLogger(){
   //this->OutputLog("Hole density logger is deleted.\n");
}

void HoleDensityLogger::SetMessages(){
   DensityLogger::SetMessages();
   this->errorMessageCISMatrixNULL
      = "Error in base::logger::HoleDensityPlot::DrawDensity: CIS Matrix is NULL.\n";
   this->errorMessageFockMatrixNULL
      = "Error in base::logger::HoleDensityPlot::DrawDensity: Fock Matrix is NULL.\n";
   this->messageCubeHeaderComment1 = "MolDS cube file (in atomic units) for Hole density.\n";
   this->messageStartDensityPlot = "\t== START: Hole density plot ==\n";
   this->messageEndDensityPlot = "\t== DONE: Hole density plot ==\n\n";
   this->messageOmpElapsedTimeDensityPlot = "\t\tElapsed time(omp) for the Hole density plot = ";
}

double HoleDensityLogger::GetDensityValue(int elecStateIndex, 
                                          const MolDS_base::Molecule& molecule, 
                                          double const* const* cisMatrix, 
                                          double x, 
                                          double y, 
                                          double z)  const{
   double density = 0.0;
   int excitedStateIndex = elecStateIndex-1;
   int numberActiveOcc = Parameters::GetInstance()->GetActiveOccCIS();
   int numberActiveVir = Parameters::GetInstance()->GetActiveVirCIS();
   int numberOcc = molecule.GetTotalNumberValenceElectrons()/2;
   stringstream ompErrors;
   #pragma omp parallel for schedule(auto) reduction(+:density)
   for(int i=0; i<numberActiveOcc; i++){
      try{
         int moI = numberOcc - (i+1);
         for(int j=0; j<numberActiveOcc; j++){
            int moJ = numberOcc - (j+1);
            for(int a=0; a<numberActiveVir; a++){
               int slaterDeterminatIndexIA = i*numberActiveVir + a;
               int slaterDeterminatIndexJA = j*numberActiveVir + a;
               double moIValue = this->GetMOValue(moI, molecule, x, y, z);
               double moJValue = this->GetMOValue(moJ, molecule, x, y, z);
               density += moIValue*cisMatrix[excitedStateIndex][slaterDeterminatIndexIA]
                         *moJValue*cisMatrix[excitedStateIndex][slaterDeterminatIndexJA];
            }
         }
      }
      catch(MolDSException ex){
         #pragma omp critical
         ompErrors << ex.what() << endl ;
      }  
   }
   // Exception throwing for omp-region
   if(!ompErrors.str().empty()){
      throw MolDSException(ompErrors.str());
   }
   return density;
}

string HoleDensityLogger::GetFileName(int elecStateIndex, int digit) const{
   stringstream fileName;
   fileName << Parameters::GetInstance()->GetFileNamePrefixHolePlot();
   fileName << Utilities::Num2String(elecStateIndex,digit);
   fileName << this->stringCubeExtension;
   return fileName.str();
}


}