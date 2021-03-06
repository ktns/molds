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
#ifndef INCLUDED_PM3D
#define INCLUDED_PM3D
namespace MolDS_pm3{

/***
 *  Main References for PM3-D are [MH_2007, MMHBV_2007]
 */
class Pm3D : public MolDS_pm3::Pm3{
public:
   Pm3D();
   virtual ~Pm3D();
protected:
   virtual void SetMessages();
   virtual void SetEnableAtomTypes();
private:
};

}
#endif



