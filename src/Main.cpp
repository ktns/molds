//************************************************************************//
// Copyright (C) 2011-2013 Mikiya Fujii                                   //
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
#include<vector>
#include<stdexcept>
#include<boost/shared_ptr.hpp>
#include<boost/format.hpp>
#include"base/Uncopyable.h"
#include"mpi/MpiProcess.h"
#include"base/PrintController.h"
#include"base/MolDSException.h"
#include"base/Enums.h"
#include"base/EularAngle.h"
#include"base/atoms/Atom.h"
#include"base/Molecule.h"
#include"base/MolDS.h"
#include<sstream>
#include<boost/archive/text_iarchive.hpp>
#include<boost/archive/text_oarchive.hpp>
using namespace std;
using namespace MolDS_base;
int main(int argc, char *argv[]){
   try{
      stringstream ss;
      try{
         MolDSException e("hoge");
         e.SetKeyValue<int>(0,2);
         throw e;
      }
      catch(MolDSException(e)){
         e.Serialize(ss);
      }
      cout << "begin_dump" << endl;
      cout << ss.str() << endl;
      cout << "end_dump" << endl;
      MolDSException e = MolDSException::Deserialize(ss);
      throw e;
   }
   catch(MolDSException ex){
      cout << ex.what();
   }
   catch(exception& ex){
      cout << ex.what();
   }
   return 0;
}

