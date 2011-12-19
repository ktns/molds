#ifndef INCLUDED_MNDO
#define INCLUDED_MNDO
namespace MolDS_mndo{

/***
 *  Main Refferences for MNDO are [DT_1977, DT_1977-2, DT_1977-3]
 */
class Mndo : public MolDS_zindo::ZindoS{
public:
   Mndo();
   virtual ~Mndo();
   virtual void SetMolecule(MolDS_base::Molecule* molecule);
protected:
   std::string errorMessageGetSemiEmpiricalMultipoleInteractionBadMultipoles;
   std::string errorMessageGetSemiEmpiricalMultipoleInteractionFirstDeriBadMultipoles;
   std::string errorMessageGetNddoRepulsionIntegral;
   std::string errorMessageGetNddoRepulsionIntegralFirstDerivative;
   std::string errorMessageCalcTwoElecTwoCoreDiatomicNullMatrix;
   std::string errorMessageCalcTwoElecTwoCoreNullMatrix;
   std::string errorMessageCalcTwoElecTwoCoreDiatomicSameAtoms;
   std::string errorMessageCalcTwoElecTwoCoreDiatomicFirstDerivativesNullMatrix;
   std::string errorMessageCalcTwoElecTwoCoreDiatomicFirstDerivativesSameAtoms;
   std::string errorMessageCalcZMatrixForceEtaNull;
   virtual void SetMessages();
   virtual void SetEnableAtomTypes();
   virtual double GetDiatomCoreRepulsionEnergy(int indexAtomA, int indexAtomB) const;
   virtual double GetDiatomCoreRepulsionFirstDerivative(int atomAIndex,
                                                        int atomBIndex, 
                                                        MolDS_base::CartesianType axisA) const;
   virtual void CalcHFProperties();
   virtual void OutputHFResults(double const* const* fockMatrix, 
                                double const* energiesMO, 
                                double const* atomicElectronPopulation, 
                                const MolDS_base::Molecule& molecule) const;
   virtual double GetFockDiagElement(const MolDS_base_atoms::Atom& atomA, 
                                     int atomAIndex, 
                                     int mu, 
                                     const MolDS_base::Molecule& molecule, 
                                     double const* const* gammaAB,
                                     double const* const* orbitalElectronPopulation, 
                                     double const* atomicElectronPopulation,
                                     double const* const* const* const* const* const* twoElecTwoCore,
                                     bool isGuess) const;
   virtual double GetFockOffDiagElement(const MolDS_base_atoms::Atom& atomA, 
                                        const MolDS_base_atoms::Atom& atomB, 
                                        int atomAIndex, 
                                        int atomBIndex, 
                                        int mu, int nu, 
                                        const MolDS_base::Molecule& molecule, 
                                        double const* const* gammaAB, 
                                        double const* const* overelap,
                                        double const* const* orbitalElectronPopulation, 
                                        double const* const* const* const* const* const* twoElecTwoCore,
                                        bool isGuess) const;
   virtual void CalcDiatomicOverlapInDiatomicFrame(double** diatomicOverlap, 
                                                   MolDS_base_atoms::Atom* atomA, 
                                                   MolDS_base_atoms::Atom* atomB);
   virtual void CalcDiatomicOverlapFirstDerivativeInDiatomicFrame(
                                                double** diatomicOverlapDeri, 
                                                MolDS_base_atoms::Atom* atomA, MolDS_base_atoms::Atom* atomB);
   virtual double GetCoulombInt(MolDS_base::OrbitalType orbital1, 
                                MolDS_base::OrbitalType orbital2, 
                                const MolDS_base_atoms::Atom& atom) const; 
   virtual double GetExchangeInt(MolDS_base::OrbitalType orbital1, 
                                 MolDS_base::OrbitalType orbital2, 
                                 const MolDS_base_atoms::Atom& atom) const; 
   virtual void CalcTwoElecTwoCore(double****** twoElecTwoCore, MolDS_base::Molecule* molecule);
   virtual double GetMolecularIntegralElement(int moI, 
                                              int moJ, 
                                              int moK, 
                                              int moL, 
                                              MolDS_base::Molecule* molecule, 
                                              double** fockMatrix, 
                                              double** gammaAB);
   virtual void CalcCISMatrix(double** matrixCIS, int numberActiveOcc, int numberActiveVir);
   virtual void CalcForce(std::vector<int> elecStates);
   double GetNddoRepulsionIntegral(MolDS_base_atoms::Atom* atomA, 
                                   MolDS_base::OrbitalType mu, 
                                   MolDS_base::OrbitalType nu,
                                   MolDS_base_atoms::Atom* atomB, 
                                   MolDS_base::OrbitalType lambda, 
                                   MolDS_base::OrbitalType sigma) const;
   double GetNddoRepulsionIntegralFirstDerivative(MolDS_base_atoms::Atom* atomA, 
                                                  MolDS_base::OrbitalType mu, 
                                                  MolDS_base::OrbitalType nu,
                                                  MolDS_base_atoms::Atom* atomB, 
                                                  MolDS_base::OrbitalType lambda, 
                                                  MolDS_base::OrbitalType sigma,
                                                  MolDS_base::CartesianType axisA) const;
private:
   std::string errorMessageMultipoleA;
   std::string errorMessageMultipoleB;
   std::string messageHeatsFormation;
   std::string messageHeatsFormationTitle;
   struct MoIndexPair{int moI; int moJ; bool isMoICIMO; bool isMoJCIMO;};
   double*** zMatrixForce;
   double*** etaMatrixForce;
   int zMatrixForceElecStatesNum;
   int etaMatrixForceElecStatesNum;
   double heatsFormation;
   double GetGammaNRElement(int moI, int moJ, int moK, int moL);
   double GetGammaRElement(int moI, int moJ, int moK, int moL);
   double GetNNRElement(int moI, int moJ, int moK, int moL);
   double GetNRElement(int moI, int moJ, int moK, int moL);
   double GetKNRElement(int moI, int moJ, int moK, int moL);
   double GetKRElement(int moI, int moJ, int moK, int moL);
   double GetKRDagerElement(int moI, int moJ, int moK, int moL);
   void MallocTempMatrixForZMatrix(double** delta,
                                   double** q,
                                   double*** kNR, 
                                   double*** kRDag,
                                   double** y,
                                   double*** transposedFockMatrix,
                                   double*** xiOcc,
                                   double*** xiVir,
                                   int sizeQNR,
                                   int sizeQR);
   void FreeTempMatrixForZMatrix(double** delta,
                                 double** q,
                                 double*** kNR, 
                                 double*** kRDag,
                                 double** y,
                                 double*** transposedFockMatrix,
                                 double*** xiOcc,
                                 double*** xiVir,
                                 int sizeQNR,
                                 int sizeQR);
   void CalcDeltaVector(double* delta, int exciteState);
   double GetSmallQElement(int moI, 
                           int moP, 
                           double**xiOcc, 
                           double** xiVir,
                           double** eta);
   void CalcQVector(double* q, 
                    double* delta, 
                    double** xiOcc,
                    double** xiVir,
                    double** eta,
                    std::vector<MoIndexPair> nonRedundantQIndeces,
                    std::vector<MoIndexPair> redundantQIndeces);
   void TransposeFockMatrixMatrix(double** transposedFockMatrix);
   void CalcKNRMatrix(double** kNR, 
                      std::vector<MoIndexPair> nonRedundantQIndeces);
   void CalcKRDagerMatrix(double** kRDager, 
                          std::vector<MoIndexPair> nonRedundantQIndeces,
                          std::vector<MoIndexPair> redundantQIndeces);
   void CalcAuxiliaryVector(double* y,
                            double* q,
                            double** kRDager,
                            std::vector<MoIndexPair> nonRedundantQIndeces,
                            std::vector<MoIndexPair> redundantQIndeces);
   void CalcXiMatrices(double** xiOcc, 
                       double** xiVir, 
                       int exciteState,
                       double** transposedFockMatrix);
   double GetZMatrixForceElement(double* y,
                                 double* q,
                                 double** transposedFockMatrix,
                                 std::vector<MoIndexPair> nonRedundantQIndeces,
                                 std::vector<MoIndexPair> redundantQIndeces,
                                 int mu, 
                                 int nu);
   void CheckZMatrixForce(std::vector<int> elecStates);
   void CheckEtaMatrixForce(std::vector<int> elecStates);
   void CalcZMatrixForce(std::vector<int> elecStates);
   void CalcEtaMatrixForce(std::vector<int> elecStates);
   bool RequiresExcitedStatesForce(std::vector<int> elecStates);
   double GetCISCoefficientMOEnergy(int k, 
                                    int l, 
                                    int r, 
                                    int numberActiveVir);
   double GetCISCoefficientTwoElecIntegral(int k, 
                                           int l, 
                                           int p, 
                                           int q, 
                                           int r, 
                                           int s, 
                                           int numberActiveVir);
   void CalcActiveSetVariablesQ(std::vector<MoIndexPair>* nonRedundantQIndeces, 
                                std::vector<MoIndexPair>* redundantQIndeces);
   void CalcHeatsFormation(double* heatsFormation, MolDS_base::Molecule* molecule);
   double GetElectronCoreAttraction(int atomAIndex, 
                                    int atomBIndex, 
                                    int mu, 
                                    int nu, 
                                    double const* const* const* const* const* const* twoElecTwoCore) const;
   double GetElectronCoreAttractionFirstDerivative(int atomAIndex, 
                                                   int atomBIndex, 
                                                   int mu, 
                                                   int nu, 
                                                   double const* const* const* const* const* twoElecTwoCoreFirstDerivative,
                                                   MolDS_base::CartesianType axisA) const;
   void CalcTwoElecTwoCoreDiatomic(double**** matrix, int atomAIndex, int atomBIndex);
   void CalcTwoElecTwoCoreDiatomicFirstDerivatives(double***** matrix, 
                                                   int atomAIndex, 
                                                   int atomBIndex);
   void RotateTwoElecTwoCoreDiatomicToSpaceFramegc(double**** matrix, double** rotatingMatrix);
   void RotateTwoElecTwoCoreDiatomicFirstDerivativesToSpaceFramegc(double***** matrix, 
                                                                   double**** twoElecTwoCoreDiatomic,
                                                                   double** rotatingMatrix,
                                                                   double*** rMatDeri);
   double GetSemiEmpiricalMultipoleInteraction(MolDS_base::MultipoleType multipoleA,
                                               MolDS_base::MultipoleType multipoleB,
                                               double rhoA,
                                               double rhoB,
                                               double DA,
                                               double DB,
                                               double Rab) const;
   double GetSemiEmpiricalMultipoleInteractionFirstDerivative(
                                               MolDS_base::MultipoleType multipoleA,
                                               MolDS_base::MultipoleType multipoleB,
                                               double rhoA,
                                               double rhoB,
                                               double DA,
                                               double DB,
                                               double Rab) const;
   void FreeCalcForceTempMatrices(double**** overlapDer, 
                                  double****** twoElecTwoCoreFirstDeriv);
   void CalcForceHFElecCoreAttractionPart(double* force, 
                                          int atomAIndex,
                                          int atomBIndex,
                                          double***** twoElecTwoCoreFirstDeriv);
   void CalcForceHFOverlapPart(double* force, 
                               int atomAIndex,
                               int atomBIndex,
                               double*** overlapDer);
   void CalcForceHFTwoElecPart(double* force, 
                               int atomAIndex,
                               int atomBIndex,
                               double***** twoElecTwoCoreFirstDeriv);
   void CalcForceExcitedStaticPart(double* force, 
                                   int elecStateIndex,
                                   int atomAIndex,
                                   int atomBIndex,
                                   double***** twoElecTwoCoreFirstDeriv);
   void CalcForceExcitedElecCoreAttractionPart(double* force, 
                                               int elecStateIndex,
                                               int atomAIndex,
                                               int atomBIndex,
                                               double***** twoElecTwoCoreFirstDeriv);
   void CalcForceExcitedOverlapPart(double* force, 
                                    int elecStateIndex,
                                    int atomAIndex,
                                    int atomBIndex,
                                    double*** overlapDer);
   void CalcForceExcitedTwoElecPart(double* force, 
                                    int elecStateIndex,
                                    int atomAIndex,
                                    int atomBIndex,
                                    double***** twoElecTwoCoreFirstDeriv);

};

}
#endif



