// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/utilities/CCopasiMethod.cpp,v $
//   $Revision: 1.53.2.2 $
//   $Name:  $
//   $Author: nsimus $
//   $Date: 2008/11/13 12:40:37 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/**
 *  CCopasiMethod class.
 *  This class is used to describe a task in COPASI. This class is
 *  intended to be used as the parent class for all methods whithin COPASI.
 *
 *  Created for Copasi by Stefan Hoops 2003
 */

#include "copasi.h"

#include "CCopasiMethod.h"
#include "CCopasiMessage.h"
#include "CCopasiProblem.h"
#include "model/CModel.h"

const std::string CCopasiMethod::SubTypeName[] =
  {
    "Not set",
    "Random Search",
    "Random Search (PVM)",
    "Simulated Annealing",
    "Genetic Algorithm",
    "Evolutionary Programming",
    "Steepest Descent",
    "Hybrid GA/SA",
    "Genetic Algorithm SR",
    "Hooke & Jeeves",
    "Levenberg - Marquardt",
    "Nelder - Mead",
    "Evolution Strategy (SRES)",
    "Current Solution Statistics",
    "Particle Swarm",
    "Praxis",
    "Truncated Newton",
    "Enhanced Newton",
    "Deterministic (LSODA)",
    "Deterministic (LSODAR)",
    "Stochastic (Gibson + Bruck)",
    "Hybrid (Runge-Kutta)",
    "Hybrid (LSODA)",
#ifdef COPASI_TSSA
    "ILDM (LSODA,Deuflhard)",
    "ILDM (LSODA,Modified)",
    "CSP (LSODA)",
#endif // COPASI_TSSA
    "Stochastic (\xcf\x84-Leap)",
    "MCA Method (Reder)",
    "Scan Framework",
    "Wolf Method",
#ifdef COPASI_TSS
    "Time Scale Separation Method",
#endif // COPASI_TSS
    "Sensitivities Method",
#ifdef COPASI_SSA
    "Stoichiometric Stability Analysis",
#endif // COPASI_SSA
    "EFM Algorithm",
    "Householder Reduction",
    ""
  };

const char* CCopasiMethod::XMLSubType[] =
  {
    "NotSet",
    "RandomSearch",
    "RandomSearch(PVM)",
    "SimulatedAnnealing",
    "GeneticAlgorithm",
    "EvolutionaryProgram",
    "SteepestDescent",
    "HybridGASA",
    "GeneticAlgorithmSR",
    "HookeJeeves",
    "LevenbergMarquardt",
    "NelderMead",
    "EvolutionaryStrategySR",
    "CurrentSolutionStatistics",
    "ParticleSwarm",
    "Praxis",
    "TruncatedNewton",
    "EnhancedNewton",
    "Deterministic(LSODA)",
    "Deterministic(LSODAR)",
    "Stochastic",
    "Hybrid",
    "Hybrid (LSODA)",
#ifdef COPASI_TSSA
    "TimeScaleSeparation(ILDM,Deuflhard)",
    "TimeScaleSeparation(ILDM,Modified)",
    "TimeScaleSeparation(CSP)",
#endif // COPASI_DEBUG
    "TauLeap",
    "MCAMethod(Reder)",
    "ScanFramework",
    "WolfMethod",
#ifdef COPASI_TSS
    "TimeScaleSeparationMethod",
#endif // COPASI_TSS
    "SensitivitiesMethod",
#ifdef COPASI_SSA
    "StoichiometricStabilityAnalysis",
#endif // COPASI_SSA
    "EFMAlgorithm",
    "Householder",
    NULL
  };

//    std::string mType;

CCopasiMethod::SubType CCopasiMethod::TypeNameToEnum(const std::string & subTypeName)
{
  unsigned C_INT32 i = 0;
  while (SubTypeName[i] != subTypeName && SubTypeName[i] != "")
    i++;

  if (CCopasiMethod::SubTypeName[i] != "") return (CCopasiMethod::SubType) i;
  else return CCopasiMethod::unset;
}

CCopasiMethod::CCopasiMethod():
    CCopasiParameterGroup("NoName", NULL, "Method"),
    mType(CCopasiTask::unset),
    mSubType(unset),
    mpCallBack(NULL)
    //mpReport(NULL)
{setObjectName(SubTypeName[mType]);}

CCopasiMethod::CCopasiMethod(const CCopasiTask::Type & type,
                             const CCopasiMethod::SubType & subType,
                             const CCopasiContainer * pParent):
    CCopasiParameterGroup(CCopasiTask::TypeName[type], pParent, "Method"),
    mType(type),
    mSubType(subType),
    mpCallBack(NULL)
    //mpReport(NULL)
{setObjectName(SubTypeName[mSubType]);}

CCopasiMethod::CCopasiMethod(const CCopasiMethod & src,
                             const CCopasiContainer * pParent):
    CCopasiParameterGroup(src, pParent),
    mType(src.mType),
    mSubType(src.mSubType),
    mpCallBack(src.mpCallBack)
    //mpReport(src.mpReport)
{}

CCopasiMethod::~CCopasiMethod() {}

bool CCopasiMethod::setCallBack(CProcessReport * pCallBack)
{
  mpCallBack = pCallBack;
  return true;
}

const CCopasiTask::Type & CCopasiMethod::getType() const {return mType;}

// void CCopasiMethod::setType(const CCopasiTask::Type & type) {mType = type;}

const CCopasiMethod::SubType & CCopasiMethod::getSubType() const
  {return mSubType;}

// void CCopasiMethod::setSubType(const CCopasiMethod::SubType & subType)
// {mSubType = subType;}

//virtual
bool CCopasiMethod::isValidProblem(const CCopasiProblem * pProblem)
{
  if (!pProblem)
    {
      //no problem
      CCopasiMessage(CCopasiMessage::EXCEPTION, MCCopasiMethod + 2);
      return false;
    }

  if (! pProblem->getModel())
    {
      //no model
      CCopasiMessage(CCopasiMessage::EXCEPTION, MCCopasiMethod + 3);
      return false;
    }

  if (pProblem->getModel()->getEvents().size())
    {
      if (mType == CCopasiTask::steadyState)
        {
          CCopasiMessage(CCopasiMessage::WARNING, MCCopasiMethod + 4, "Steady-State");
          return false;
        }

      if (mType == CCopasiTask::timeCourse && mSubType == CCopasiMethod::deterministic)
        {
          CCopasiMessage(CCopasiMessage::ERRoR, MCCopasiMethod + 4, "Time Course with deterministic method (LSODA)");
          return false;
        }

      if (mType == CCopasiTask::mca)
        {
          CCopasiMessage(CCopasiMessage::WARNING, MCCopasiMethod + 4, "Metabolic Control Analysis");
          return false;
        }

      if (mType == CCopasiTask::lyap)
        {
          CCopasiMessage(CCopasiMessage::ERRoR, MCCopasiMethod + 4, "Lyapunov Exponents");
          return false;
        }

#ifdef COPASI_TSSA
      if (mType == CCopasiTask::tssAnalysis)
        {
          CCopasiMessage(CCopasiMessage::ERRoR, MCCopasiMethod + 4, "Time Scale Separation Analysis");
          return false;
        }
#endif // COPASI_TSSA
    }

  return true;
}

void CCopasiMethod::load(CReadConfig & /* configBuffer */,
                         CReadConfig::Mode /* mode */)
{fatalError();}

void CCopasiMethod::print(std::ostream * ostream) const
  {*ostream << *this;}

std::ostream &operator<<(std::ostream &os, const CCopasiMethod & o)
{
  os << "Method: " << o.getObjectName() << std::endl;

  CCopasiParameterGroup::parameterGroup::const_iterator it =
    o.CCopasiParameter::getValue().pGROUP->begin();
  CCopasiParameterGroup::parameterGroup::const_iterator end =
    o.CCopasiParameter::getValue().pGROUP->end();

  for (; it != end; ++it)
    {
      (*it)->print(&os);
      os << std::endl;
    }

  return os;
}

void CCopasiMethod::printResult(std::ostream * ostream) const
  {*ostream << " Not implemented.";}
