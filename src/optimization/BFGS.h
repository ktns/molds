//************************************************************************//
// Copyright (C) 2011-2014 Mikiya Fujii                                   //
// Copyright (C) 2012-2014 Katsuhiko Nishimra                             //
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
#ifndef INCLUDED_BFGS
#define INCLUDED_BFGS
namespace MolDS_optimization{

class BFGS : public MolDS_optimization::Optimizer{
private:
   class BFGSState: public OptimizerState{
   protected:
      double** matrixHessian;
      double** matrixOldForce;
      double** matrixStep;
      double** matrixOldCoordinates;
      double*  vectorOldCoordinates;
      double** matrixDisplacement;
      size_t numAtoms;
   private:
      template<class vector>
      vector Matrix2Vector(vector const* matrix){return matrix == NULL ? NULL : &matrix[0][0];}
   public:
      BFGSState(MolDS_base::Molecule& molecule);
      virtual ~BFGSState();
      double const* GetVectorForce         (){return this->Matrix2Vector(this->matrixForce);}
      double*       GetVectorOldForce      (){return this->Matrix2Vector(this->matrixOldForce);}
      double*       GetVectorStep          (){return this->Matrix2Vector(this->matrixStep);}
      double const* GetVectorOldCoordinates(){return this->Matrix2Vector(this->matrixOldCoordinates);}
      double**  GetMatrixHessian()          {return this->matrixHessian;}
      double**  GetMatrixOldForce()         {return this->matrixOldForce;}
      double**  GetMatrixStep()             {return this->matrixStep;}
      double**  GetMatrixOldCoordinates()   {return this->matrixOldCoordinates;}
      double**& GetMatrixOldCoordinatesRef(){return this->matrixOldCoordinates;}
      double**  GetMatrixDisplacement()     {return this->matrixDisplacement;}
   };
public:
   BFGS();
   ~BFGS();
protected:
   void SetMessages();

   std::string errorMessageNaNInRFOStep;

   std::string messageStartBFGSStep;
   std::string messageHillClimbing;
   std::string messageRecalculateRFOStep;
   std::string messageRawHessianEigenvalues;
   std::string messageShiftedHessianEigenvalues;

   std::string formatEnergyChangeComparison;
   std::string formatLowestHessianEigenvalue;
   std::string format2ndLowestHessianEigenvalue;
   std::string format3rdLowestHessianEigenvalue;
   std::string formatRFOStepSize;
   std::string formatTrustRadiusIs;
   std::string formatIncreaseScalingFactor;

private:
   const std::string& OptimizationStepMessage() const{
      return this->messageStartBFGSStep;
   }
   virtual void SearchMinimum(boost::shared_ptr<MolDS_base::ElectronicStructure> electronicStructure,
                              MolDS_base::Molecule& molecule,
                              double* lineSearchedEnergy,
                              bool* obainesOptimizedStructure) const;

protected:
   void InitializeState(OptimizerState &state, const MolDS_base::Molecule& molecule) const;
   void CalcRFOStep(double* vectorStep,
                    double const* const* matrixHessian,
                    double const* vectorForce,
                    const double maxNormStep,
                    const int dimension) const;
   void CalcDisplacement(double      *      * matrixDisplacement,
                         double const* const* matrixOldCoordinates,
                         const MolDS_base::Molecule& molecule)const;
   void UpdateHessian(double**      matrixHessian,
                      const int     dimension,
                      double const* vectorForce,
                      double const* vectorOldForce,
                      double const* vectorDisplacement) const;
   void ShiftHessianRedundantMode(double** matrixHessian,
                                  const MolDS_base::Molecule& molecule) const;
   double ApproximateEnergyChange(int dimension,
                                  double const* const* matrixHessian,
                                  double const* vectorForce,
                                  double const* vectorStep) const;
   void UpdateTrustRadius(double &trustRadius,
                          double approximateEnergyChange,
                          double currentEnergy,
                          double initialEnergy)const;
   void StoreMolecularGeometry(double **& matrixCoordinates,     
                               const MolDS_base::Molecule& molecule)const;
   void RollbackMolecularGeometry(MolDS_base::Molecule& molecule,
                                  double const* const* matrixOldCoordinates) const;
};

}
#endif
