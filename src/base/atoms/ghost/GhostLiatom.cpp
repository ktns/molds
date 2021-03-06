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
#include<vector>
#include<boost/format.hpp>
#include"../../Enums.h"
#include"../../Uncopyable.h"
#include"../../PrintController.h"
#include"../../MolDSException.h"
#include"../../MallocerFreer.h"
#include"../../../mpi/MpiInt.h"
#include"../../../mpi/MpiProcess.h"
#include"../../EularAngle.h"
#include"../../Parameters.h"
#include"../../RealSphericalHarmonicsIndex.h"
#include"../Atom.h"
#include"../Liatom.h"
#include"Ghost.h"
#include"GhostLiatom.h"
using namespace std;
using namespace MolDS_base;
using namespace MolDS_base_atoms;
namespace MolDS_base_atoms_ghost{
GhostLiatom::GhostLiatom(int index) : Atom(index),Liatom(index),Ghost(index){
   this->SetAtomicParameters();
}

GhostLiatom::~GhostLiatom(){}

void GhostLiatom::SetAtomicParameters(){
   Liatom::SetAtomicParameters();
   Ghost::SetAtomicParameters();
   this->atomType = ghostLi;
}
}
