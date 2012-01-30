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
#include<boost/random.hpp>
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
#include"MC.h"
using namespace std;
using namespace MolDS_base;
using namespace MolDS_base_atoms;

namespace MolDS_mc{
MC::MC(){
   this->molecule = NULL;
   this->SetMessages();
   //cout << "MC created \n";
}

MC::~MC(){
   //cout << "MC deleted\n";
}

void MC::SetMolecule(Molecule* molecule){
   this->molecule = molecule;
}

void MC::SetMessages(){
   this->messageStartMC = "**********  START: Monte Carlo  **********\n";
   this->messageEndMC = "**********  DONE: Monte Carlo  **********\n";
   this->messageinitialConditionMC = "\n\t========= Initial conditions \n";
   this->messageStartStepMC = "\n\t========== START: MC step ";
   this->messageEndStepMC =     "\t========== DONE: MC step ";
   this->messageEnergies = "\tEnergies:\n";
   this->messageEnergiesTitle = "\t\t|\tkind\t\t\t| [a.u.] | [eV] | \n";
   this->messageCoreRepulsionEnergy = "Core repulsion   ";
   this->messageElectronicEnergy = "Electronic\n\t\t(inc. core rep.)";
   this->messageTransitionRate = "\tTransition Rate: ";
}

void MC::DoMC(){
   int totalSteps = Parameters::GetInstance()->GetTotalStepsMC();
   int elecState = Parameters::GetInstance()->GetElectronicStateIndexMC();
   double temperature = Parameters::GetInstance()->GetTemperatureMC();
   double stepWidth = Parameters::GetInstance()->GetStepWidthMC();
   unsigned long seed = Parameters::GetInstance()->GetSeedMC();
   this->DoMC(totalSteps, elecState, temperature, stepWidth, seed);
}

void MC::DoMC(int totalSteps, int elecState, double temperature, double stepWidth, unsigned long seed){
   if(this->PrintsLogs()){
      cout << this->messageStartMC;
   }
   double transitionRate = 0.0;
   // create real random generator
	boost::mt19937 realGenerator(seed);
	boost::uniform_real<> range(0, 1);
	boost::variate_generator<boost::mt19937&, boost::uniform_real<> > realRand( realGenerator, range );

   // prepare trial molecule and electronic structure pointa
   Molecule trialMolecule(*this->molecule);
   boost::shared_ptr<ElectronicStructure> electronicStructure2(ElectronicStructureFactory::GetInstance()->Create());
   ElectronicStructure* trialES = electronicStructure2.get();
   trialES->SetMolecule(&trialMolecule);
   trialES->SetPrintsLogs(this->PrintsLogs());

   // initial calculation
   boost::shared_ptr<ElectronicStructure> electronicStructure1(ElectronicStructureFactory::GetInstance()->Create());
   ElectronicStructure* currentES = electronicStructure1.get();
   currentES->SetMolecule(this->molecule);
   currentES->SetPrintsLogs(this->PrintsLogs());
   currentES->DoSCF();
   if(Parameters::GetInstance()->RequiresCIS()){
      currentES->DoCIS();
   }
   if(this->PrintsLogs()){
      cout << this->messageinitialConditionMC;
   }
   this->OutputMolecule(*currentES, *this->molecule, elecState);

   // Monte Carlo roop 
   for(int s=0; s<totalSteps; s++){
      if(this->PrintsLogs()){
         cout << this->messageStartStepMC << s+1 << endl << endl;
      }
      // create trial molecule
      this->CreateTrialConfiguration(&trialMolecule, *this->molecule, &realRand, stepWidth);
     
      // calculate trilal electronic structure
      bool requireGuess = (s==0) ? true : false;
      trialES->DoSCF(requireGuess);
      if(Parameters::GetInstance()->RequiresCIS()){
         trialES->DoCIS();
      }

      // which Electronic Structure is used?
      if(UsesTrial(*currentES, *trialES, elecState, &realRand, temperature)){
         this->SynchronousMolecularConfiguration(this->molecule, trialMolecule);
         swap(currentES, trialES);
         currentES->SetMolecule(this->molecule);
         trialES->SetMolecule(&trialMolecule);
         transitionRate += 1.0;
      }
      else{
         this->SynchronousMolecularConfiguration(&trialMolecule, *this->molecule);
      }
      
      // output molecular states
      this->OutputMolecule(*currentES, *this->molecule, elecState);
      if(this->PrintsLogs()){
         cout << this->messageEndStepMC << s+1 << endl;
      }
   }
   if(this->PrintsLogs()){
      cout << this->messageTransitionRate << transitionRate/(double)totalSteps << endl << endl;
      cout << this->messageEndMC << endl;
   }
}

void MC::CreateTrialConfiguration(Molecule* trial,
                                  const Molecule& current,
                                  boost::random::variate_generator<
                                     boost::random::mt19937&,
                                     boost::uniform_real<>
                                  > (*realRand),
                                  double stepWidth) const{
   // disturb an atom in trial molecule
   int movedAtomIndex = (int)((*realRand)()*this->molecule->GetAtomVect()->size());
   const Atom& reffAtom = *(*current.GetAtomVect())[movedAtomIndex];
   Atom* trialAtom = (*trial->GetAtomVect())[movedAtomIndex];
   double dr[CartesianType_end] = {0.0, 0.0, 0.0};
   for(int i=0; i<CartesianType_end; i++){
      dr[i] = stepWidth*(2.0*(*realRand)() -1.0);
      trialAtom->GetXyz()[i] = reffAtom.GetXyz()[i] + dr[i];
   }

   // shift all atoms in trial molecule, namely shift the center of core.
   double trialAtomCoreMass = reffAtom.GetCoreMass();
   double totalCoreMass = current.GetTotalCoreMass();
   double coreCenterShift[CartesianType_end] = {0.0, 0.0, 0.0};
   for(int i=0; i<CartesianType_end; i++){
      coreCenterShift[i] = dr[i]*trialAtomCoreMass/totalCoreMass;
   }
   for(int a=0; a<current.GetAtomVect()->size(); a++){
      Atom* trialAtom = (*trial->GetAtomVect())[a];
      for(int i=0; i<CartesianType_end; i++){
         trialAtom->GetXyz()[i] -= coreCenterShift[i];
      }
   }
   trial->CalcXyzCOC();
   trial->CalcXyzCOM();
}

bool MC::UsesTrial(const ElectronicStructure& currentES, 
                   const ElectronicStructure& trialES,
                   int elecState,
                   boost::random::variate_generator<
                     boost::random::mt19937&,
                     boost::uniform_real<>
                   > (*realRand),
                   double temperature) const{
   double currentElecEne = currentES.GetElectronicEnergy(elecState);
   double trialElecEne = trialES.GetElectronicEnergy(elecState);
   double deltaElecEne = trialElecEne - currentElecEne;
   if(deltaElecEne <= 0.0){
      return true;
   }
   else{
      double kB = Parameters::GetInstance()->GetBoltzmann();
      double p = exp(-1.0*deltaElecEne/(kB*temperature));
      double random = (*realRand)();
      if(p>random){
         return true;
      }
      else{
         return false;
      }
   }
}

void MC::SynchronousMolecularConfiguration(Molecule* target, 
                                           const Molecule& refference) const{
   for(int a=0; a<target->GetAtomVect()->size(); a++){
      Atom* targetAtom = (*target->GetAtomVect())[a];
      const Atom& refferenceAtom = *(*refference.GetAtomVect())[a];
      for(int i=0; i<CartesianType_end; i++){
         targetAtom->GetXyz()[i] = refferenceAtom.GetXyz()[i];
      }
   }
   target->CalcXyzCOC();
   target->CalcXyzCOM();
}

void MC::OutputMolecule(const ElectronicStructure& electronicStructure,
                        const Molecule& molecule,
                        int elecState) const{
   this->OutputEnergies(electronicStructure, elecState);
   if(this->PrintsLogs()){
      molecule.OutputConfiguration();
      molecule.OutputXyzCOC();
   }
}

void MC::OutputEnergies(const MolDS_base::ElectronicStructure& electronicStructure,
                        int elecState) const{
   if(this->PrintsLogs()){
      cout << this->messageEnergies;
      cout << this->messageEnergiesTitle;
      printf("\t\t%s\t%e\t%e\n",this->messageCoreRepulsionEnergy.c_str(), 
                                electronicStructure.GetCoreRepulsionEnergy(),
                                electronicStructure.GetCoreRepulsionEnergy()
                                /Parameters::GetInstance()->GetEV2AU());
      printf("\t\t%s\t%e\t%e\n",this->messageElectronicEnergy.c_str(), 
                                electronicStructure.GetElectronicEnergy(elecState),
                                electronicStructure.GetElectronicEnergy(elecState)
                                /Parameters::GetInstance()->GetEV2AU());
      cout << endl;
   }
}

}
