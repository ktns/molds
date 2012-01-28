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
#include"MolDSException.h"
#include"Uncopyable.h"
#include"Utilities.h"
#include"Enums.h"
#include"EularAngle.h"
#include"Parameters.h"
#include"atoms/Atom.h"
#include"atoms/Hatom.h"
#include"atoms/Liatom.h"
#include"atoms/Catom.h"
#include"atoms/Natom.h"
#include"atoms/Oatom.h"
#include"atoms/Satom.h"
#include"AtomFactory.h"
#include"Molecule.h"
#include"InputParser.h"
using namespace std;
using namespace MolDS_base_atoms;
namespace MolDS_base{

InputParser* InputParser::inputParser = NULL;

InputParser::InputParser(){
   this->SetMessages();
}

InputParser::~InputParser(){
}

InputParser* InputParser::GetInstance(){
   if(inputParser == NULL){
      inputParser = new InputParser();
   }
   return inputParser;
}

void InputParser::DeleteInstance(){
   if(inputParser != NULL){
      delete inputParser; 
   }
   inputParser = NULL;
}

void InputParser::SetMessages(){
   this->errorMessageNonValidExcitedStatesMD
      = "Error in base::InputParser::CheckMdConditions: Excited state on which MD runs or CIS condition are wrong.\n";
   this->errorMessageNonValidExcitedStatesMC
      = "Error in base::InputParser::CheckMcConditions: Excited state on which MC runs or CIS condition are wrong.\n";
   this->errorMessageNonValidExcitedStatesRPMD
      = "Error in base::InputParser::CheckRpmdConditions: Excited state on which RPMD runs or CIS condition are wrong.\n";
   this->errorMessageNonValidExcitedStatesOptimize
      = "Error in base::InputParser::CheckOptimizeConditions: Excited state on which optimization is carried out or CIS condition are wrong.\n";
   this->messageStartParseInput = "**********  START: Parse input  **********\n";
   this->messageDoneParseInput =  "**********  DONE: Parse input  ***********\n\n\n";
   this->messageTotalNumberAOs = "\tTotal number of valence AOs: ";
   this->messageTotalNumberAtoms = "\tTotal number of atoms: ";
   this->messageTotalNumberValenceElectrons = "\tTotal number of valence electrons: ";
   this->messageInputTerms = "Input terms:\n";

   // SCF
   this->messageScfConditions = "\tSCF conditions:\n";
   this->messageScfMaxIterations = "\t\tMax iterations: ";
   this->messageScfRmsDensity = "\t\tRMS density: ";
   this->messageScfDampingThresh = "\t\tDamping threshold: ";
   this->messageScfDampingWeight = "\t\tDamping weight: ";
   this->messageScfDiisNumErrorVect = "\t\tDIIS number of error vectors: ";
   this->messageScfDiisStartError = "\t\tDIIS starting error: ";
   this->messageScfDiisEndError = "\t\tDIIS ending error: ";

   // CIS
   this->messageCisConditions = "\tCIS conditions:\n";
   this->messageCisNumberActiveOcc = "\t\tNumber of active Occ.: ";
   this->messageCisNumberActiveVir = "\t\tNumber of active Vir.: ";
   this->messageCisNumberExcitedStates = "\t\tNumber of excited states: ";
   this->messageCisDavidson = "\t\tCIS-Davidson: ";
   this->messageCisNormTolerance = "\t\tNorm tolerance for the residual of the Davidson: ";
   this->messageCisMaxIterations = "\t\tMax iterations for the Davidson: ";
   this->messageCisMaxDimensions = "\t\tMax dimensions for the Davidson: ";

   // memory
   this->messageMemoryConditions = "\tMemory conditions:\n";
   this->messageMemoryLimitHeap = "\t\tHeap limit: ";
   this->messageMemoryMB = "[MB]\n";

   // MD
   this->messageMdConditions = "\tMD conditions:\n";
   this->messageMdTotalSteps = "\t\tTotal steps: ";
   this->messageMdElecState = "\t\tElectronic eigenstate: ";
   this->messageMdTimeWidth = "\t\tTime width(dt): ";

   // MC
   this->messageMcConditions = "\tMC conditions:\n";
   this->messageMcTotalSteps = "\t\tTotal steps: ";
   this->messageMcElecState = "\t\tElectronic eigenstate: ";
   this->messageMcStepWidth = "\t\tStep width: ";
   this->messageMcTemperature = "\t\tTemperature: ";
   this->messageMcSeed = "\t\tSeed: ";

   // RPMD
   this->messageRpmdConditions = "\tRPMD conditions:\n";
   this->messageRpmdTotalSteps = "\t\tTotal steps: ";
   this->messageRpmdElecState = "\t\tElectronic eigenstate: ";
   this->messageRpmdNumElecStates = "\t\tNumber of the electronic eigenstates: ";
   this->messageRpmdTimeWidth = "\t\tTime width: ";
   this->messageRpmdTemperature = "\t\tTemperature: ";
   this->messageRpmdNumBeads = "\t\tNumber of the beads in the Ring Polymer: ";
   this->messageRpmdSeed = "\t\tSeed: ";

   // Optimize (steepestDescent)
   this->messageOptimizeConditions = "\tOptimize conditions:\n";
   this->messageSteepestDescentLineReturnTimes = "\t\tLine return times:";
   this->messageSteepestDescentSteps = "\t\tSteepest Descent steps: ";
   this->messageSteepestDescentElecState = "\t\tElectronic eigenstate: ";
   this->messageSteepestDescentMaxGradient = "\t\tMax gradient: ";
   this->messageSteepestDescentRmsGradient = "\t\tRms gradient: ";
   this->messageSteepestDescentTimeWidth = "\t\tFictious time width: ";

   // MOPlot
   this->messageMOPlotConditions = "\tMO plot conditions:\n";
   this->messageMOPlotIndex = "\t\tMO index: ";
   this->messageMOPlotGridNumber = "\t\tNumber of grid(x, y, z): ";
   this->messageMOPlotFrameLength = "\t\tFrame length[angst.](x, y, z): ";
   this->messageMOPlotFilePrefix = "\t\tFile name prefix: ";

   // unit
   this->messageFs = "[fs]";
   this->messageK = "[K]";
   this->messageAngst = "[Angst.]";

   // others
   this->stringYES = "yes";
   this->stringNO = "no";
   this->stringSpace = " ";

   // theory
   this->stringCommentOut = "//";
   this->stringTheoryCNDO2 = "cndo/2";
   this->stringTheoryINDO = "indo";
   this->stringTheoryZINDOS = "zindo/s";
   this->stringTheoryMNDO = "mndo";
   this->stringTheoryAM1 = "am1";
   this->stringTheoryPM3 = "pm3";
   this->stringTheoryPM3PDDG = "pm3/pddg";
   this->stringTheory = "theory";
   this->stringTheoryEnd = "theory_end";

   // geometry
   this->stringGeometry =    "geometry";
   this->stringGeometryEnd = "geometry_end";

   // SCF
   this->stringScf = "scf";
   this->stringScfEnd = "scf_end";
   this->stringScfMaxIter = "max_iter";
   this->stringScfRmsDensity = "rms_density";
   this->stringScfDampingThresh = "damping_thresh";
   this->stringScfDampingWeight = "damping_weight";
   this->stringScfDiisNumErrorVect = "diis_num_error_vect";
   this->stringScfDiisStartError = "diis_start_error";
   this->stringScfDiisEndError = "diis_end_error";

   // MO plot
   this->stringMO = "mo";
   this->stringMOPlot = "moplot";
   this->stringMOPlotEnd = "moplot_end";
   this->stringMOPlotGridNumber = "grid_number";
   this->stringMOPlotFrameLength = "frame_length";
   this->stringMOPlotFilePrefix = "file_prefix";

   // Principal axes
   this->stringInertiaTensor = "inertia";
   this->stringInertiaTensorEnd = "inertia_end";
   this->stringInertiaTensorOrigin = "origin";

   // Rotate
   this->stringRotate = "rotate";
   this->stringRotateEnd = "rotate_end";
   this->stringRotatingOrigin = "origin";
   this->stringRotatingAxis = "axis";
   this->stringRotatingAngle = "angle";
   this->stringRotatingAngles = "angles";
   this->stringRotatingType = "type";
   this->stringRotatingTypeAxis = "axis";
   this->stringRotatingTypeEularAngle = "eular_angle";

   // Translate
   this->stringTranslate = "translate";
   this->stringTranslateEnd = "translate_end";
   this->stringTranslatingDifference = "difference";

   // CIS
   this->stringCIS = "cis";
   this->stringCISEnd = "cis_end";
   this->stringCISActiveOcc = "active_occ";
   this->stringCISActiveVir = "active_vir";
   this->stringCISNStates = "nstates";
   this->stringCISDavidson = "davidson";
   this->stringCISMaxIter = "max_iter";
   this->stringCISMaxDimensions = "max_dim";
   this->stringCISNormTolerance = "norm_tol";

   // Memory
   this->stringMemory = "memory";
   this->stringMemoryEnd = "memory_end";
   this->stringMemoryLimitHeap = "limit_heap";

   // MD
   this->stringMD = "md";
   this->stringMDEnd = "md_end";
   this->stringMDTotalSteps = "total_steps";
   this->stringMDElecState = "electronic_state";
   this->stringMDTimeWidth = "dt";

   // MC
   this->stringMC = "mc";
   this->stringMCEnd = "mc_end";
   this->stringMCTotalSteps = "total_steps";
   this->stringMCElecState = "electronic_state";
   this->stringMCStepWidth = "step_width";
   this->stringMCTemperature = "temperature";
   this->stringMCSeed = "seed";

   // RPMD
   this->stringRPMD = "rpmd";
   this->stringRPMDEnd = "rpmd_end";
   this->stringRPMDTotalSteps = "total_steps";
   this->stringRPMDElecState = "electronic_state";
   this->stringRPMDNumElecStates = "num_electronic_states";
   this->stringRPMDTimeWidth = "dt";
   this->stringRPMDTemperature = "temperature";
   this->stringRPMDNumBeads = "num_beads";
   this->stringRPMDSeed = "seed";

   // Opt
   this->stringOptimize = "optimize";
   this->stringOptimizeEnd = "optimize_end";
   this->stringSteepestDescentLineReturnTimes = "line_return_times";
   this->stringSteepestDescentSteps = "steep_step";
   this->stringSteepestDescentElecState = "electronic_state";
   this->stringSteepestDescentMaxGradient = "max_gradient";
   this->stringSteepestDescentRmsGradient = "rms_gradient";
   this->stringSteepestDescentTimeWidth = "dt";
}

vector<string> InputParser::GetInputTerms() const{

   string str;
   string inputTerm;
   vector<string> inputTerms;
   bool isPreCharSpace = false;
   while(getline(cin, str)){

      // check comment out 
      if(this->IsCommentOut(str)){
         continue;
      }

      // get input terms
      for(int i=0; i<str.length(); i++){
         if(str.data()[i] != stringSpace.data()[0]){
            // change to lower case.
            inputTerm += tolower(str.data()[i]);
            isPreCharSpace = false;
         }
         else{
            if(!isPreCharSpace){
               inputTerms.push_back(inputTerm);
               inputTerm = "";
               isPreCharSpace = true;
            }
         }
      }
      if(inputTerm.length()>0){
         inputTerms.push_back(inputTerm);
         inputTerm = "";
      }
      isPreCharSpace = true;
   }

   return inputTerms;

}

int InputParser::ParseMolecularGeometry(Molecule* molecule, vector<string>* inputTerms, int parseIndex) const{
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringGeometryEnd) != 0){
      double x = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
      double y = atof((*inputTerms)[parseIndex+2].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
      double z = atof((*inputTerms)[parseIndex+3].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
      AtomType atomType = H;
      if((*inputTerms)[parseIndex] == "h"){
        atomType = H;
      }
      else if((*inputTerms)[parseIndex] == "li"){
         atomType = Li;
      }
      else if((*inputTerms)[parseIndex] == "c"){
         atomType = C;
      }
      else if((*inputTerms)[parseIndex] == "n"){
         atomType = N;
      }
      else if((*inputTerms)[parseIndex] == "o"){
         atomType = O;
      }
      else if((*inputTerms)[parseIndex] == "s"){
         atomType = S;
      }
      Atom* atom = AtomFactory::GetInstance()->Create(atomType, x, y, z);
      molecule->GetAtomVect()->push_back(atom);
      parseIndex += 4;
   }
   return parseIndex;
}

int InputParser::ParseConditionsSCF(vector<string>* inputTerms, int parseIndex) const{
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringScfEnd) != 0){
      // max iterations
      if((*inputTerms)[parseIndex].compare(this->stringScfMaxIter) == 0){
         Parameters::GetInstance()->SetMaxIterationsSCF(atoi((*inputTerms)[parseIndex+1].c_str()));
         parseIndex++;
      }
      // RMS density 
      if((*inputTerms)[parseIndex].compare(this->stringScfRmsDensity) == 0){
         Parameters::GetInstance()->SetThresholdSCF(atof((*inputTerms)[parseIndex+1].c_str()));
         parseIndex++;
      }
      // Damping Threshold 
      if((*inputTerms)[parseIndex].compare(this->stringScfDampingThresh) == 0){
         Parameters::GetInstance()->SetDampingThreshSCF(atof((*inputTerms)[parseIndex+1].c_str()));
         parseIndex++;
      }
      // Damping Weight
      if((*inputTerms)[parseIndex].compare(this->stringScfDampingWeight) == 0){
         Parameters::GetInstance()->SetDampingWeightSCF(atof((*inputTerms)[parseIndex+1].c_str()));
         parseIndex++;
      }
      // DIIS number of stored error vectors
      if((*inputTerms)[parseIndex].compare(this->stringScfDiisNumErrorVect) == 0){
         Parameters::GetInstance()->SetDiisNumErrorVectSCF(atoi((*inputTerms)[parseIndex+1].c_str()));
         parseIndex++;
      }
      // DIIS starting error
      if((*inputTerms)[parseIndex].compare(this->stringScfDiisStartError) == 0){
         Parameters::GetInstance()->SetDiisStartErrorSCF(atof((*inputTerms)[parseIndex+1].c_str()));
         parseIndex++;
      }
      // DIIS ending error
      if((*inputTerms)[parseIndex].compare(this->stringScfDiisEndError) == 0){
         Parameters::GetInstance()->SetDiisEndErrorSCF(atof((*inputTerms)[parseIndex+1].c_str()));
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsPrincipalAxes(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetCurrentSimulation(PrincipalAxes);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringInertiaTensorEnd) != 0){
      // origin
      if((*inputTerms)[parseIndex].compare(this->stringInertiaTensorOrigin) == 0){
         double x = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double y = atof((*inputTerms)[parseIndex+2].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double z = atof((*inputTerms)[parseIndex+3].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         Parameters::GetInstance()->SetInertiaTensorOrigin(x, y, z);
         parseIndex+=3;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsTranslation(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetCurrentSimulation(Translate);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringTranslateEnd) != 0){
      // origin
      if((*inputTerms)[parseIndex].compare(this->stringTranslatingDifference) == 0){
         double x = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double y = atof((*inputTerms)[parseIndex+2].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double z = atof((*inputTerms)[parseIndex+3].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         Parameters::GetInstance()->SetTranslatingDifference(x, y, z);
         parseIndex+=3;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsRotation(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetCurrentSimulation(Rotate);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringRotateEnd) != 0){
      // origin
      if((*inputTerms)[parseIndex].compare(this->stringRotatingOrigin) == 0){
         double x = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double y = atof((*inputTerms)[parseIndex+2].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double z = atof((*inputTerms)[parseIndex+3].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         Parameters::GetInstance()->SetRotatingOrigin(x, y, z);
         parseIndex+=3;
      }
      // axis
      else if((*inputTerms)[parseIndex].compare(this->stringRotatingAxis) == 0){
         double x = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double y = atof((*inputTerms)[parseIndex+2].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         double z = atof((*inputTerms)[parseIndex+3].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         Parameters::GetInstance()->SetRotatingAxis(x, y, z);
         parseIndex+=3;
      }
      // angle 
      else if((*inputTerms)[parseIndex].compare(this->stringRotatingAngle) == 0){
         double angle = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetDegree2Radian();
         Parameters::GetInstance()->SetRotatingAngle(angle);
         parseIndex++;
      }
      // angles (EularAngle)
      else if((*inputTerms)[parseIndex].compare(this->stringRotatingAngles) == 0){
         double alpha = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetDegree2Radian();
         double beta  = atof((*inputTerms)[parseIndex+2].c_str()) * Parameters::GetInstance()->GetDegree2Radian();
         double gamma = atof((*inputTerms)[parseIndex+3].c_str()) * Parameters::GetInstance()->GetDegree2Radian();
         Parameters::GetInstance()->SetRotatingEularAngles(alpha, beta, gamma);
         parseIndex += 3;
      }
      // type
      else if((*inputTerms)[parseIndex].compare(this->stringRotatingType) == 0){
         if((*inputTerms)[parseIndex+1].compare(this->stringRotatingTypeAxis) == 0){
            Parameters::GetInstance()->SetRotatingType(Axis);
         }
         else if((*inputTerms)[parseIndex+1].compare(this->stringRotatingTypeEularAngle) == 0){
            Parameters::GetInstance()->SetRotatingType(Eular);
         }
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsMOPlot(vector<string>* inputTerms, int parseIndex) const{
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringMOPlotEnd) != 0){
      // Frame length
      if((*inputTerms)[parseIndex].compare(this->stringMOPlotFrameLength) == 0){
         double lx = atof((*inputTerms)[parseIndex+1].c_str()) 
                    *Parameters::GetInstance()->GetAngstrom2AU();
         double ly = atof((*inputTerms)[parseIndex+2].c_str()) 
                    *Parameters::GetInstance()->GetAngstrom2AU();
         double lz = atof((*inputTerms)[parseIndex+3].c_str()) 
                    *Parameters::GetInstance()->GetAngstrom2AU();
         Parameters::GetInstance()->SetFrameLengthMOPlot(lx, ly, lz);
         parseIndex += 3;
      }
      // Grid number
      if((*inputTerms)[parseIndex].compare(this->stringMOPlotGridNumber) == 0){
         int nx = atof((*inputTerms)[parseIndex+1].c_str());
         int ny = atof((*inputTerms)[parseIndex+2].c_str());
         int nz = atof((*inputTerms)[parseIndex+3].c_str());
         Parameters::GetInstance()->SetGridNumberMOPlot(nx, ny, nz);
         parseIndex += 3;
      }
      // mo index
      if((*inputTerms)[parseIndex].compare(this->stringMO) == 0){
         int moIndex = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->AddIndexMOPlot(moIndex);
         parseIndex++;
      }
      // file prefix
      if((*inputTerms)[parseIndex].compare(this->stringMOPlotFilePrefix) == 0){
         string filePrefix((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetFileNamePrefixMOPlot(filePrefix);
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsCIS(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetRequiresCIS(true);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringCISEnd) != 0){
      // number of active occupied orbitals
      if((*inputTerms)[parseIndex].compare(this->stringCISActiveOcc) == 0){
         int activeOccCIS = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetActiveOccCIS(activeOccCIS);
         parseIndex++;
      }
      // number of active virtual orbitals
      if((*inputTerms)[parseIndex].compare(this->stringCISActiveVir) == 0){
         int activeVirCIS = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetActiveVirCIS(activeVirCIS);
         parseIndex++;
      }
      // number of excited states
      if((*inputTerms)[parseIndex].compare(this->stringCISNStates) == 0){
         int nStates = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetNumberExcitedStatesCIS(nStates);
         parseIndex++;
      }
      // Davidson is used or not
      if((*inputTerms)[parseIndex].compare(this->stringCISDavidson) == 0){
         if((*inputTerms)[parseIndex+1].compare(this->stringYES) == 0){
            Parameters::GetInstance()->SetIsDavidsonCIS(true);
         }
         else{
            Parameters::GetInstance()->SetIsDavidsonCIS(false);
         }
         parseIndex++;
      }
      // max iterations for the Davidson roop
      if((*inputTerms)[parseIndex].compare(this->stringCISMaxIter) == 0){
         int maxIter = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetMaxIterationsCIS(maxIter);
         parseIndex++;
      }
      // max dimensions for the Davidson expansion
      if((*inputTerms)[parseIndex].compare(this->stringCISMaxDimensions) == 0){
         int maxDim = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetMaxDimensionsCIS(maxDim);
         parseIndex++;
      }
      // nolm tolerance for the norm of the resiudal vectors of the Davidson.
      if((*inputTerms)[parseIndex].compare(this->stringCISNormTolerance) == 0){
         double normTol = atof((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetNormToleranceCIS(normTol);
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsMC(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetCurrentSimulation(MC);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringMCEnd) != 0){
      // number of total steps 
      if((*inputTerms)[parseIndex].compare(this->stringMCTotalSteps) == 0){
         int totalSteps = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetTotalStepsMC(totalSteps);
         parseIndex++;
      }
      // index of electronic eigen state on whichi MC runs. 
      if((*inputTerms)[parseIndex].compare(this->stringMCElecState) == 0){
         int elecStateIndex = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetElectronicStateIndexMC(elecStateIndex);
         parseIndex++;
      }
      // temperature for MC.
      if((*inputTerms)[parseIndex].compare(this->stringMCTemperature) == 0){
         double temperature = atof((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetTemperatureMC(temperature);
         parseIndex++;
      }
      // step width for MC.
      if((*inputTerms)[parseIndex].compare(this->stringMCStepWidth) == 0){
         double stepWidth = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetAngstrom2AU();
         Parameters::GetInstance()->SetStepWidthMC(stepWidth);
         parseIndex++;
      }
      // seed for MC.
      if((*inputTerms)[parseIndex].compare(this->stringMCSeed) == 0){
         unsigned long seed = atol((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetSeedMC(seed);
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsMD(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetCurrentSimulation(MD);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringMDEnd) != 0){
      // number of total steps 
      if((*inputTerms)[parseIndex].compare(this->stringMDTotalSteps) == 0){
         int totalSteps = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetTotalStepsMD(totalSteps);
         parseIndex++;
      }
      // index of electronic eigen state on whichi MD runs. 
      if((*inputTerms)[parseIndex].compare(this->stringMDElecState) == 0){
         int elecStateIndex = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetElectronicStateIndexMD(elecStateIndex);
         parseIndex++;
      }
      // time width for MD.
      if((*inputTerms)[parseIndex].compare(this->stringMDTimeWidth) == 0){
         double dt = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetFs2AU();
         Parameters::GetInstance()->SetTimeWidthMD(dt);
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsRPMD(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetCurrentSimulation(RPMD);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringRPMDEnd) != 0){
      // number of total steps 
      if((*inputTerms)[parseIndex].compare(this->stringRPMDTotalSteps) == 0){
         int totalSteps = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetTotalStepsRPMD(totalSteps);
         parseIndex++;
      }
      // index of electronic eigen state on which RPMD runs. 
      if((*inputTerms)[parseIndex].compare(this->stringRPMDElecState) == 0){
         int elecStateIndex = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetElectronicStateIndexRPMD(elecStateIndex);
         parseIndex++;
      }
      // number of the electronic eigenstates for nonadiabatic RPMD.
      if((*inputTerms)[parseIndex].compare(this->stringRPMDNumElecStates) == 0){
         int numElecStates = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetNumberElectronicStatesRPMD(numElecStates);
         parseIndex++;
      }
      // temperature for RPMD.
      if((*inputTerms)[parseIndex].compare(this->stringRPMDTemperature) == 0){
         double temperature = atof((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetTemperatureRPMD(temperature);
         parseIndex++;
      }
      // time width for RPMD.
      if((*inputTerms)[parseIndex].compare(this->stringRPMDTimeWidth) == 0){
         double timeWidth = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetFs2AU();
         Parameters::GetInstance()->SetTimeWidthRPMD(timeWidth);
         parseIndex++;
      }
      // number of the beads in Ring Polymer.
      if((*inputTerms)[parseIndex].compare(this->stringRPMDNumBeads) == 0){
         int numBeads = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetNumberBeadsRPMD(numBeads);
         parseIndex++;
      }
      // seed for RPMD.
      if((*inputTerms)[parseIndex].compare(this->stringRPMDSeed) == 0){
         unsigned long seed = atol((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetSeedRPMD(seed);
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseConditionsOptimize(vector<string>* inputTerms, int parseIndex) const{
   Parameters::GetInstance()->SetCurrentSimulation(Optimize);
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringOptimizeEnd) != 0){
      // number of steps of the steepest descent
      if((*inputTerms)[parseIndex].compare(this->stringSteepestDescentLineReturnTimes) == 0){
         int lineReturnTimes = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetLineReturnTimesSteepestDescent(lineReturnTimes);
         parseIndex++;
      }
      // number of steps of the steepest descent
      if((*inputTerms)[parseIndex].compare(this->stringSteepestDescentSteps) == 0){
         int steps = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetStepsSteepestDescent(steps);
         parseIndex++;
      }
      // index of electronic eigen state on which steepest descent is carried out. 
      if((*inputTerms)[parseIndex].compare(this->stringSteepestDescentElecState) == 0){
         int elecStateIndex = atoi((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetElectronicStateIndexSteepestDescent(elecStateIndex);
         parseIndex++;
      }
      // time width for the steepest descent.
      if((*inputTerms)[parseIndex].compare(this->stringSteepestDescentTimeWidth) == 0){
         double timeWidth = atof((*inputTerms)[parseIndex+1].c_str()) * Parameters::GetInstance()->GetFs2AU();
         Parameters::GetInstance()->SetTimeWidthSteepestDescent(timeWidth);
         parseIndex++;
      }
      // max gradient for the steepest descent.
      if((*inputTerms)[parseIndex].compare(this->stringSteepestDescentMaxGradient) == 0){
         double maxGradient = atof((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetMaxGradientSteepestDescent(maxGradient);
         parseIndex++;
      }
      // rms gradient for the steepest descent.
      if((*inputTerms)[parseIndex].compare(this->stringSteepestDescentRmsGradient) == 0){
         double rmsGradient = atof((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetRmsGradientSteepestDescent(rmsGradient);
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

int InputParser::ParseTheory(vector<string>* inputTerms, int parseIndex) const{
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringTheoryEnd) != 0){
      // CNDO/2
      if((*inputTerms)[parseIndex].compare(this->stringTheoryCNDO2) == 0){
         Parameters::GetInstance()->SetCurrentTheory(CNDO2);
      }
      // INDO
      else if((*inputTerms)[parseIndex].compare(this->stringTheoryINDO) == 0){
         Parameters::GetInstance()->SetCurrentTheory(INDO);
      }
      // ZINDO/S
      else if((*inputTerms)[parseIndex].compare(this->stringTheoryZINDOS) == 0){
         Parameters::GetInstance()->SetCurrentTheory(ZINDOS);
      }
      // MNDO
      else if((*inputTerms)[parseIndex].compare(this->stringTheoryMNDO) == 0){
         Parameters::GetInstance()->SetCurrentTheory(MNDO);
      }
      // AM1
      else if((*inputTerms)[parseIndex].compare(this->stringTheoryAM1) == 0){
         Parameters::GetInstance()->SetCurrentTheory(AM1);
      }
      // PM3
      else if((*inputTerms)[parseIndex].compare(this->stringTheoryPM3) == 0){
         Parameters::GetInstance()->SetCurrentTheory(PM3);
      }
      // PM3/PDG
      else if((*inputTerms)[parseIndex].compare(this->stringTheoryPM3PDDG) == 0){
         Parameters::GetInstance()->SetCurrentTheory(PM3PDDG);
      }
      parseIndex++;
   }
   return parseIndex;
}

int InputParser::ParseConditionsMemory(vector<string>* inputTerms, int parseIndex) const{
   parseIndex++;
   while((*inputTerms)[parseIndex].compare(this->stringMemoryEnd) != 0){
      // max of heap
      if((*inputTerms)[parseIndex].compare(this->stringMemoryLimitHeap) == 0){
         double limitHeap = atof((*inputTerms)[parseIndex+1].c_str());
         Parameters::GetInstance()->SetLimitHeapMemory(limitHeap);
         parseIndex++;
      }
      parseIndex++;   
   }
   return parseIndex;
}

void InputParser::Parse(Molecule* molecule) const{

   cout << messageStartParseInput;

   // read input
   vector<string> inputTerms = this->GetInputTerms();

   // parse input
   for(int i=0; i<inputTerms.size();i++){

      // theory
      if(inputTerms[i].compare(this->stringTheory) == 0){
         i = this->ParseTheory(&inputTerms, i);
      }

      // molecular geometry
      if(inputTerms[i].compare(this->stringGeometry) == 0){
         i = this->ParseMolecularGeometry(molecule, &inputTerms, i);
      }

      // scf condition
      if(inputTerms[i].compare(this->stringScf) == 0){
         i = this->ParseConditionsSCF(&inputTerms, i);
      }
      
      // inertia tensor condition
      if(inputTerms[i].compare(this->stringInertiaTensor) == 0){
         i = this->ParseConditionsPrincipalAxes(&inputTerms, i);
      }
      
      // translating condition
      if(inputTerms[i].compare(this->stringTranslate) == 0){
         i = this->ParseConditionsTranslation(&inputTerms, i);
      }
      
      // rotating condition
      if(inputTerms[i].compare(this->stringRotate) == 0){
         i = this->ParseConditionsRotation(&inputTerms, i);
      }
      
      // mo plot condition
      if(inputTerms[i].compare(this->stringMOPlot) == 0){
         i = this->ParseConditionsMOPlot(&inputTerms, i);
      }

      // cis condition
      if(inputTerms[i].compare(this->stringCIS) == 0){
         i = this->ParseConditionsCIS(&inputTerms, i);
      }

      // Memory
      if(inputTerms[i].compare(this->stringMemory) == 0){
         i = this->ParseConditionsMemory(&inputTerms, i);
      }

      // MD condition
      if(inputTerms[i].compare(this->stringMD) == 0){
         i = this->ParseConditionsMD(&inputTerms, i);
      }

      // MC condition
      if(inputTerms[i].compare(this->stringMC) == 0){
         i = this->ParseConditionsMC(&inputTerms, i);
      }

      // RPMD condition
      if(inputTerms[i].compare(this->stringRPMD) == 0){
         i = this->ParseConditionsRPMD(&inputTerms, i);
      }

      // Optimize condition
      if(inputTerms[i].compare(this->stringOptimize) == 0){
         i = this->ParseConditionsOptimize(&inputTerms, i);
      }

   }

   // calculate basics and check conditions
   this->CalcMolecularBasics(molecule);
   if(Parameters::GetInstance()->RequiresCIS()){
      this->CheckCisConditions(*molecule);
   }
   if(Parameters::GetInstance()->GetCurrentSimulation()==MD){
      this->CheckMdConditions(*molecule);
   }
   else if(Parameters::GetInstance()->GetCurrentSimulation()==MC){
      this->CheckMcConditions(*molecule);
   }
   else if(Parameters::GetInstance()->GetCurrentSimulation()==RPMD){
      this->CheckRpmdConditions(*molecule);
   }
   else if(Parameters::GetInstance()->GetCurrentSimulation()==Optimize){
      this->CheckOptimizeConditions(*molecule);
   }

   // output conditions
   this->OutputMolecularBasics(molecule);
   this->OutputScfConditions();
   this->OutputMemoryConditions();
   if(Parameters::GetInstance()->RequiresCIS()){
      this->OutputCisConditions();
   }
   if(Parameters::GetInstance()->RequiresMOPlot()){
      this->OutputMOPlotConditions();
   }
   if(Parameters::GetInstance()->GetCurrentSimulation()==MD){
      this->OutputMdConditions();
   }
   else if(Parameters::GetInstance()->GetCurrentSimulation()==MC){
      this->OutputMcConditions();
   }
   else if(Parameters::GetInstance()->GetCurrentSimulation()==RPMD){
      this->OutputRpmdConditions();
   }
   else if(Parameters::GetInstance()->GetCurrentSimulation()==Optimize){
      this->OutputOptimizeConditions();
   }

   // output inputs
   this->OutputInputTerms(inputTerms);
   cout << messageDoneParseInput;

}

void InputParser::CalcMolecularBasics(Molecule* molecule) const{
   molecule->CalcBasics();
}

void InputParser::CheckCisConditions(const Molecule& molecule) const{

   // direct CIS
   int numberOcc = molecule.GetTotalNumberValenceElectrons()/2;
   int numberVir = molecule.GetTotalNumberAOs() - numberOcc;

   // check the number of active occupied orbitals.
   if(numberOcc < Parameters::GetInstance()->GetActiveOccCIS()){
      Parameters::GetInstance()->SetActiveOccCIS(numberOcc);
   }   

   // check the number of active virtual orbitals.
   if(numberVir < Parameters::GetInstance()->GetActiveVirCIS()){
      Parameters::GetInstance()->SetActiveVirCIS(numberVir);
   }   

   // check the number of calculated excited states.
   int numberSlaterDeterminants = Parameters::GetInstance()->GetActiveOccCIS() 
                                 *Parameters::GetInstance()->GetActiveVirCIS();
   if(!Parameters::GetInstance()->IsDavidsonCIS()){
      Parameters::GetInstance()->SetNumberExcitedStatesCIS(numberSlaterDeterminants);
   }
   else{
      if(numberSlaterDeterminants < Parameters::GetInstance()->GetNumberExcitedStatesCIS()){
         Parameters::GetInstance()->SetNumberExcitedStatesCIS(numberSlaterDeterminants);
      }
      if(numberSlaterDeterminants < Parameters::GetInstance()->GetMaxDimensionsCIS()){
         Parameters::GetInstance()->SetMaxDimensionsCIS(numberSlaterDeterminants);
      }
   }
   
}

void InputParser::CheckMdConditions(const Molecule& molecule) const{
   int groundStateIndex = 0;
   // ZINDO does not support excited states force.
   if(Parameters::GetInstance()->GetCurrentTheory() == ZINDOS){
      Parameters::GetInstance()->SetElectronicStateIndexMD(groundStateIndex);
   }
   // check for the excited states dynamics
   int targetStateIndex = Parameters::GetInstance()->GetElectronicStateIndexMD();
   if(groundStateIndex < targetStateIndex && !Parameters::GetInstance()->RequiresCIS()){
      Parameters::GetInstance()->SetNumberExcitedStatesCIS(targetStateIndex);
      Parameters::GetInstance()->SetRequiresCIS(true);
      this->CheckCisConditions(molecule);
   }
   int numberExcitedStatesCIS = Parameters::GetInstance()->GetNumberExcitedStatesCIS();
   if(numberExcitedStatesCIS < targetStateIndex){
      stringstream ss;
      ss << this->errorMessageNonValidExcitedStatesMD;
      throw MolDSException(ss.str());
   } 
}

void InputParser::CheckMcConditions(const Molecule& molecule) const{
   int groundStateIndex = 0;
   // CNDO2 and INDO do not support excited states.
   if(Parameters::GetInstance()->GetCurrentTheory() == CNDO2 || 
      Parameters::GetInstance()->GetCurrentTheory() == INDO){
      Parameters::GetInstance()->SetElectronicStateIndexMC(groundStateIndex);
   }
   // check for the excited states dynamics
   int targetStateIndex = Parameters::GetInstance()->GetElectronicStateIndexMC();
   if(groundStateIndex < targetStateIndex && !Parameters::GetInstance()->RequiresCIS()){
      Parameters::GetInstance()->SetNumberExcitedStatesCIS(targetStateIndex);
      Parameters::GetInstance()->SetRequiresCIS(true);
      this->CheckCisConditions(molecule);
   }
   int numberExcitedStatesCIS = Parameters::GetInstance()->GetNumberExcitedStatesCIS();
   if(numberExcitedStatesCIS < targetStateIndex){
      stringstream ss;
      ss << this->errorMessageNonValidExcitedStatesMC;
      throw MolDSException(ss.str());
   }
}

void InputParser::CheckRpmdConditions(const Molecule& molecule) const{
   // ToDo: check rpmd conditions
}

void InputParser::CheckOptimizeConditions(const Molecule& molecule) const{
   // ToDo: check optimization (steepest descent) conditions
}

void InputParser::OutputMolecularBasics(Molecule* molecule) const{
   molecule->OutputTotalNumberAtomsAOsValenceelectrons();
   molecule->OutputConfiguration();
   molecule->OutputXyzCOM();
   molecule->OutputXyzCOC();
}

void InputParser::OutputScfConditions() const{
   cout << this->messageScfConditions;
   printf("%s%d\n",this->messageScfMaxIterations.c_str(),Parameters::GetInstance()->GetMaxIterationsSCF());
   printf("%s%e\n",this->messageScfRmsDensity.c_str(),Parameters::GetInstance()->GetThresholdSCF());
   printf("%s%e\n",this->messageScfDampingThresh.c_str(),Parameters::GetInstance()->GetDampingThreshSCF());
   printf("%s%e\n",this->messageScfDampingWeight.c_str(),Parameters::GetInstance()->GetDampingWeightSCF());
   printf("%s%d\n",this->messageScfDiisNumErrorVect.c_str(),Parameters::GetInstance()->GetDiisNumErrorVectSCF());
   printf("%s%e\n",this->messageScfDiisStartError.c_str(),Parameters::GetInstance()->GetDiisStartErrorSCF());
   printf("%s%e\n",this->messageScfDiisEndError.c_str(),Parameters::GetInstance()->GetDiisEndErrorSCF());
   cout << "\n";
}

void InputParser::OutputMemoryConditions() const{
   cout << this->messageMemoryConditions;
   printf("%s%e%s",this->messageMemoryLimitHeap.c_str(), Parameters::GetInstance()->GetLimitHeapMemory(), this->messageMemoryMB.c_str());
   cout << "\n";
}

void InputParser::OutputCisConditions() const{
   cout << this->messageCisConditions;

   printf("%s%d\n",this->messageCisNumberActiveOcc.c_str(),Parameters::GetInstance()->GetActiveOccCIS());
   printf("%s%d\n",this->messageCisNumberActiveVir.c_str(),Parameters::GetInstance()->GetActiveVirCIS());
   printf("%s%d\n",this->messageCisNumberExcitedStates.c_str(),Parameters::GetInstance()->GetNumberExcitedStatesCIS());
   printf("%s",this->messageCisDavidson.c_str());
   if(Parameters::GetInstance()->IsDavidsonCIS()){
      printf("%s\n",this->stringYES.c_str());
      printf("%s%d\n",this->messageCisMaxIterations.c_str(),Parameters::GetInstance()->GetMaxIterationsCIS());
      printf("%s%d\n",this->messageCisMaxDimensions.c_str(),Parameters::GetInstance()->GetMaxDimensionsCIS());
      printf("%s%e\n",this->messageCisNormTolerance.c_str(),Parameters::GetInstance()->GetNormToleranceCIS());
   }
   else{
      printf("%s\n",this->stringNO.c_str());
   }

   cout << "\n";
}

void InputParser::OutputMdConditions() const{
   cout << this->messageMdConditions;

   printf("%s%d\n",this->messageMdElecState.c_str(),Parameters::GetInstance()->GetElectronicStateIndexMD());
   printf("%s%d\n",this->messageMdTotalSteps.c_str(),Parameters::GetInstance()->GetTotalStepsMD());
   printf("%s%lf%s\n",this->messageMdTimeWidth.c_str(),Parameters::GetInstance()->GetTimeWidthMD()/Parameters::GetInstance()->GetFs2AU(),this->messageFs.c_str());

   cout << "\n";
}

void InputParser::OutputMcConditions() const{
   cout << this->messageMcConditions;

   printf("%s%d\n",this->messageMcElecState.c_str(),Parameters::GetInstance()->GetElectronicStateIndexMC());
   printf("%s%d\n",this->messageMcTotalSteps.c_str(),Parameters::GetInstance()->GetTotalStepsMC());
   printf("%s%lf%s\n",this->messageMcTemperature.c_str(),Parameters::GetInstance()->GetTemperatureMC(),this->messageK.c_str());
   printf("%s%lf%s\n",this->messageMcStepWidth.c_str(),Parameters::GetInstance()->GetStepWidthMC()/Parameters::GetInstance()->GetAngstrom2AU(),this->messageAngst.c_str());
   printf("%s%lu\n",this->messageMcSeed.c_str(),Parameters::GetInstance()->GetSeedMC());

   cout << "\n";
}

void InputParser::OutputRpmdConditions() const{
   cout << this->messageRpmdConditions;

   printf("%s%d\n",this->messageRpmdElecState.c_str(),Parameters::GetInstance()->GetElectronicStateIndexRPMD());
   printf("%s%d\n",this->messageRpmdNumElecStates.c_str(),Parameters::GetInstance()->GetNumberElectronicStatesRPMD());
   printf("%s%d\n",this->messageRpmdTotalSteps.c_str(),Parameters::GetInstance()->GetTotalStepsRPMD());
   printf("%s%lf%s\n",this->messageRpmdTemperature.c_str(),Parameters::GetInstance()->GetTemperatureRPMD(),this->messageK.c_str());
   printf("%s%lf%s\n",this->messageRpmdTimeWidth.c_str(),Parameters::GetInstance()->GetTimeWidthRPMD()/Parameters::GetInstance()->GetFs2AU(),this->messageFs.c_str());
   printf("%s%d\n",this->messageRpmdNumBeads.c_str(),Parameters::GetInstance()->GetNumberBeadsRPMD());
   printf("%s%lu\n",this->messageRpmdSeed.c_str(),Parameters::GetInstance()->GetSeedRPMD());

   cout << "\n";
}

void InputParser::OutputOptimizeConditions() const{
   cout << this->messageOptimizeConditions;

   printf("%s%d\n",this->messageSteepestDescentLineReturnTimes.c_str(),Parameters::GetInstance()->GetLineReturnTimesSteepestDescent());
   printf("%s%d\n",this->messageSteepestDescentSteps.c_str(),Parameters::GetInstance()->GetStepsSteepestDescent());
   printf("%s%d\n",this->messageSteepestDescentElecState.c_str(),Parameters::GetInstance()->GetElectronicStateIndexSteepestDescent());
   printf("%s%lf\n",this->messageSteepestDescentMaxGradient.c_str(),Parameters::GetInstance()->GetMaxGradientSteepestDescent());
   printf("%s%lf\n",this->messageSteepestDescentRmsGradient.c_str(),Parameters::GetInstance()->GetRmsGradientSteepestDescent());
   printf("%s%lf%s\n",this->messageSteepestDescentTimeWidth.c_str(),Parameters::GetInstance()->GetTimeWidthSteepestDescent()/Parameters::GetInstance()->GetFs2AU(),this->messageFs.c_str());

   cout << "\n";
}

void InputParser::OutputMOPlotConditions() const{
   cout << this->messageMOPlotConditions;
   vector<int>* moIndeces = Parameters::GetInstance()->GetIndecesMOPlot();
   for(int i=0; i<moIndeces->size(); i++){
      printf("%s%d\n", this->messageMOPlotIndex.c_str(), (*moIndeces)[i]);
   }
   int* gridNum = Parameters::GetInstance()->GetGridNumberMOPlot();
   printf("%s%d %d %d\n", this->messageMOPlotGridNumber.c_str(), 
                              gridNum[XAxis], 
                              gridNum[YAxis],
                              gridNum[ZAxis]);
   double* frameLength = Parameters::GetInstance()->GetFrameLengthMOPlot();
   double ang2AU = Parameters::GetInstance()->GetAngstrom2AU();
   printf("%s%e %e %e\n", this->messageMOPlotFrameLength.c_str(), 
                              frameLength[XAxis]/ang2AU, 
                              frameLength[YAxis]/ang2AU,
                              frameLength[ZAxis]/ang2AU);
   printf("%s%s\n", this->messageMOPlotFilePrefix.c_str(),
                      Parameters::GetInstance()->GetFileNamePrefixMOPlot().c_str());

   cout << endl;
}

void InputParser::OutputInputTerms(vector<string> inputTerms) const{
   // output input terms
   cout << this->messageInputTerms;
   for(int i=0; i<inputTerms.size();i++){
      cout << inputTerms[i] << " | ";
      if(i%10 == 9){
         cout << "\n";
      }
   }
   cout << endl << endl;
}

/****
 *
 *  # or // are treated as comment out 
 *
 ****/
bool InputParser::IsCommentOut(string tempStr) const{
   string str = TrimString(tempStr);

   string commentPrefix1 = "#";
   string prefix1;
   if(str.length()>=1){
      prefix1 += str.data()[0];
   }

   string commentPrefix2 = "//";
   string prefix2;
   if(str.length()>=2){
      prefix2 += str.data()[0];
      prefix2 += str.data()[1];
   }

   return 0==prefix1.compare(commentPrefix1) || 0==prefix2.compare(commentPrefix2) ;
}

}





