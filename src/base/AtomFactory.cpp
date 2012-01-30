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
#include<vector>
#include<stdexcept>
#include"MolDSException.h"
#include"Uncopyable.h"
#include"Enums.h"
#include"atoms/Atom.h"
#include"atoms/Hatom.h"
#include"atoms/Liatom.h"
#include"atoms/Catom.h"
#include"atoms/Natom.h"
#include"atoms/Oatom.h"
#include"atoms/Satom.h"
#include"AtomFactory.h"
using namespace std;
using namespace MolDS_base_atoms;
namespace MolDS_base{

AtomFactory* AtomFactory::atomFactory = NULL;

AtomFactory::AtomFactory(){
   this->errorMessageNotEnableAtom = "Error in base::AtomFactory::Create: Not Enable AtomType is set.";
   this->errorMessageAtomType = "\tatom type = ";
}

AtomFactory::~AtomFactory(){
}

AtomFactory* AtomFactory::GetInstance(){
   if(atomFactory == NULL){
      atomFactory = new AtomFactory();
   }
   return atomFactory;
}

void AtomFactory::DeleteInstance(){
   if(atomFactory != NULL){
      delete atomFactory; 
   }
   atomFactory = NULL;
}

Atom* AtomFactory::Create(AtomType atomType, double x, double y, double z, double px, double py, double pz){
   Atom* atom=NULL;
   if(atomType == H){
      atom = new Hatom();
   }
   else if(atomType == Li){
      atom = new Liatom();
   }
   else if(atomType == C){
      atom = new Catom();
   }
   else if(atomType == N){
      atom = new Natom();
   }
   else if(atomType == O){
      atom = new Oatom();
   }
   else if(atomType == S){
      atom = new Satom();
   }
   else{
      stringstream ss;
      ss << this->errorMessageNotEnableAtom << endl;
      ss << this->errorMessageAtomType << AtomTypeStr(atomType) << endl;
      throw MolDSException(ss.str());
   }
   atom->SetXyz(x, y, z);
   atom->SetPxyz(px, py, pz);
   return atom;
}

Atom* AtomFactory::Create(AtomType atomType, double x, double y, double z){
   double px=0.0;
   double py=0.0;
   double pz=0.0;
   return AtomFactory::Create(atomType, x, y, z, px, py, pz);
}
}




