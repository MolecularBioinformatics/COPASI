/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/sbml/SBMLImporter.cpp,v $
   $Revision: 1.12 $
   $Name:  $
   $Author: gauges $ 
   $Date: 2004/06/16 14:39:06 $
   End CVS Header */

#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include "model/CModel.h"
#include "model/CCompartment.h"
#include "model/CMetab.h"
#include "model/CReaction.h"
#include "utilities/CGlobals.h"
#include "copasi.h"
#include "function/CNodeK.h"
#include "function/CFunctionDB.h"

#include "sbml/SBMLReader.hpp"
#include "sbml/SBMLDocument.hpp"
#include "sbml/Compartment.hpp"
#include "sbml/Species.hpp"
#include "sbml/Reaction.hpp"
#include "sbml/FormulaFormatter.h"
#include "sbml/Model.hpp"
#include "sbml/UnitKind.h"

#include "SBMLImporter.h"
#include "ConverterASTNode.h"

/**
 * Creates and returns a Copasi CModel from the SBMLDocument given as argument.
 */
CModel* SBMLImporter::createCModelFromSBMLDocument(SBMLDocument* sbmlDocument) throw(StdException)
{
  Model* sbmlModel = sbmlDocument->getModel();
  /* Create an empty model and set the title. */
  //std::cerr << "Setting units." << std::endl;
  CModel* copasiModel = new CModel();
  copasiModel->setVolumeUnit(CModel::l);
  copasiModel->setTimeUnit(CModel::s);
  copasiModel->setQuantityUnit(CModel::Mol);

  if (sbmlModel->getNumUnitDefinitions() != 0)
    {
      for (unsigned int counter = 0; counter < sbmlModel->getNumUnitDefinitions(); counter++)
        {
          UnitDefinition* uDef = sbmlModel->getUnitDefinition(counter);
          std::string unitId = uDef->getId();
          if (unitId == "substance")
            {
              if (uDef->getNumUnits() == 1)
                {
                  Unit* u = uDef->getUnit(0);
                  if ((u->getKind() == UNIT_KIND_MOLE))
                    {
                      if ((u->getExponent() == 1) && (u->getMultiplier() == 1) && ((u->getScale() % 3) == 0) && (u->getScale() < 1) && (u->getScale() > -16))
                        {
                          switch (u->getScale())
                            {
                            case 0:
                              copasiModel->setQuantityUnit(CModel::Mol);
                              break;
                            case - 3:
                              copasiModel->setQuantityUnit(CModel::mMol);
                              break;
                            case - 6:
                              copasiModel->setQuantityUnit(CModel::microMol);
                              break;
                            case - 9:
                              copasiModel->setQuantityUnit(CModel::nMol);
                              break;
                            case - 12:
                              copasiModel->setQuantityUnit(CModel::pMol);
                              break;
                            case - 15:
                              copasiModel->setQuantityUnit(CModel::fMol);
                              break;
                            default:
                              std::cerr << "Error. This value should never have been reached for the scale of the liter unit." << std::endl;
                              exit(1);
                              break;
                            }
                        }
                      else
                        {
                          throw StdException("Error. Invalid SBML substance unit definition.");
                        }
                    }
                  else if ((u->getKind() == UNIT_KIND_ITEM))
                    {
                      if ((u->getExponent() == 1) && (u->getMultiplier() == 1) && (u->getScale() == 0))
                        {
                          copasiModel->setQuantityUnit(CModel::number);
                        }
                      else
                        {
                          throw StdException("Error. Invalid SBML substance unit definition.");
                        }
                    }
                  else
                    {
                      throw StdException("Error. Invalid SBML volume unit definition.");
                    }
                }
              else
                {
                  throw StdException("Error. Invalid SBML substance unit definition.");
                }
            }
          else if (unitId == "time")
            {
              if (uDef->getNumUnits() == 1)
                {
                  Unit* u = uDef->getUnit(0);
                  if ((u->getKind() == UNIT_KIND_SECOND))
                    {
                      if ((u->getExponent() == 1) && ((u->getScale() % 3) == 0) && (u->getScale() < 1) && (u->getScale() > -16))
                        {
                          if (u->getMultiplier() == 1.0)
                            {
                              switch (u->getScale())
                                {
                                case 0:
                                  copasiModel->setTimeUnit(CModel::s);
                                  break;
                                case - 3:
                                  copasiModel->setTimeUnit(CModel::ms);
                                  break;
                                case - 6:
                                  copasiModel->setTimeUnit(CModel::micros);
                                  break;
                                case - 9:
                                  copasiModel->setTimeUnit(CModel::ns);
                                  break;
                                case - 12:
                                  copasiModel->setTimeUnit(CModel::ps);
                                  break;
                                case - 15:
                                  copasiModel->setTimeUnit(CModel::fs);
                                  break;
                                default:
                                  std::cerr << "Error. This value should never have been reached for the scale of the time unit." << std::endl;
                                  exit(1);
                                  break;
                                }
                            }
                          else if (u->getMultiplier() == 60.0)
                            {
                              copasiModel->setTimeUnit(CModel::m);
                            }
                          else if (u->getMultiplier() == 3600.0)
                            {
                              copasiModel->setTimeUnit(CModel::h);
                            }
                          else if (u->getMultiplier() == 86400.0)
                            {
                              copasiModel->setTimeUnit(CModel::d);
                            }
                          else
                            {
                              throw StdException("Error. Invalid SBML time unit definition.");
                            }
                        }
                      else
                        {
                          throw StdException("Error. Invalid SBML time unit definition.");
                        }
                    }
                  else
                    {
                      throw StdException("Error. Invalid SBML time unit definition.");
                    }
                }
              else
                {
                  throw StdException("Error. Invalid SBML time unit definition.");
                }
            }
          else if (unitId == "volume")
            {
              if (uDef->getNumUnits() == 1)
                {
                  Unit* u = uDef->getUnit(0);
                  if ((u->getKind() == UNIT_KIND_LITER) || (u->getKind() == UNIT_KIND_LITRE))
                    {
                      if ((u->getExponent() == 1) && (u->getMultiplier() == 1) && ((u->getScale() % 3) == 0) && (u->getScale() < 1) && (u->getScale() > -16))
                        {
                          switch (u->getScale())
                            {
                            case 0:
                              copasiModel->setVolumeUnit(CModel::l);
                              break;
                            case - 3:
                              copasiModel->setVolumeUnit(CModel::ml);
                              break;
                            case - 6:
                              copasiModel->setVolumeUnit(CModel::microl);
                              break;
                            case - 9:
                              copasiModel->setVolumeUnit(CModel::nl);
                              break;
                            case - 12:
                              copasiModel->setVolumeUnit(CModel::pl);
                              break;
                            case - 15:
                              copasiModel->setVolumeUnit(CModel::fl);
                              break;
                            default:
                              std::cerr << "Error. This value should never have been reached for the scale of the liter unit." << std::endl;
                              exit(1);
                              break;
                            }
                        }
                      else
                        {
                          throw StdException("Error. Invalid SBML volume unit definition.");
                        }
                    }
                  else if ((u->getKind() == UNIT_KIND_METER) || (u->getKind() == UNIT_KIND_METRE))
                    {
                      if ((u->getExponent() == 3) && (u->getMultiplier() == 1) && (u->getScale() == 0))
                        {
                          copasiModel->setVolumeUnit(CModel::m3);
                        }
                      else
                        {
                          throw StdException("Error. Invalid SBML volume unit definition.");
                        }
                    }
                  else
                    {
                      throw StdException("Error. Invalid SBML volume unit definition.");
                    }
                }
              else
                {
                  throw StdException("Error. Invalid SBML volume unit definition.");
                }
            }
          else if ((unitId == "area") || (unitId == "length"))
            {
              /* do nothing, but do not throw an exception either */
            }
          else
            {
              /* Dont' throw an exception any more because individual objects
              ** are tested for unit usage and warning will be created there.
              */ 
              //throw StdException("Error. SBML Units other than \"substance\", \"time\" and \"volume\" not implemented.");
            }
        }
    }

  std::string title = sbmlModel->getName();
  if (title == "")
    {
      title = "NoName";
    }
  copasiModel->setTitle(title.c_str());

  /* Set standard units to match the standard units of SBML files. */
  std::map<std::string, CCompartment*> compartmentMap;
  /* Create the compartments */
  //std::cerr << "Creating compartments." << std::endl;
  unsigned int num = sbmlModel->getNumCompartments();
  for (unsigned int counter = 0; counter < num; counter++)
    {
      Compartment* sbmlCompartment = sbmlModel->getCompartment(counter);
      CCompartment* copasiCompartment = this->createCCompartmentFromCompartment(sbmlCompartment, copasiModel);
      compartmentMap[sbmlCompartment->getId()] = copasiCompartment;
    }

  /* Create all species */
  //std::cerr << "Creating Metabolites." << std::endl;
  num = sbmlModel->getNumSpecies();
  for (unsigned int counter = num; counter > 0; counter--)
    {
      Species* sbmlSpecies = sbmlModel->getSpecies(counter - 1);
      CCompartment* copasiCompartment = compartmentMap[sbmlSpecies->getCompartment()];
      if (copasiCompartment != NULL)
        {
          CMetab* copasiMetabolite = this->createCMetabFromSpecies(sbmlSpecies, copasiModel, copasiCompartment);
          this->speciesMap[sbmlSpecies->getId()] = copasiMetabolite;
        }
    }

  /* Create all reactions */
  //std::cerr << "Creating reactions." << std::endl;
  num = sbmlModel->getNumReactions();
  for (unsigned int counter = 0; counter < num; counter++)
    {
      this->createCReactionFromReaction(sbmlModel->getReaction(counter), sbmlModel, copasiModel);
    }
  copasiModel->compile();
  return copasiModel;
}

/**
 * Creates and returns a Copasi CCompartment from the SBML Compartment
 * given as argument.
 */
CCompartment*
SBMLImporter::createCCompartmentFromCompartment(const Compartment* sbmlCompartment, CModel* copasiModel) throw(StdException)
{
  if (sbmlCompartment->isSetUnits())
    {
      std::string cU = sbmlCompartment->getUnits();
      if (cU != "volume" && cU != "area" && cU != "length")
        {
          throw StdException("Error. Compartment unit other than \"volume\", \"area\" or \"length\" are not supported.");
        }
      else if (cU == "area" || cU == "length")
        {
          /* !!!! create a warning that the units will be ignored. */
        }
    }
  std::string name = sbmlCompartment->getName();
  if (name == "")
    {
      name = sbmlCompartment->getId();
    }
  std::string appendix = "";
  unsigned int counter = 0;
  while (copasiModel->getCompartments().getIndex(name + appendix) != static_cast < unsigned C_INT32
         > (-1))
    {
      counter++;
      std::ostringstream numberStream;
      numberStream << "_" << counter;
      appendix = numberStream.str();
    }
  CCompartment* copasiCompartment = copasiModel->createCompartment(name + appendix, sbmlCompartment->getVolume());
  std::cerr << "Created Compartment: " << copasiCompartment->getObjectName() << std::endl;
  return copasiCompartment;
}

/**
 * Creates and returns a Copasi CMetab from the given SBML Species object.
 */
CMetab*
SBMLImporter::createCMetabFromSpecies(const Species* sbmlSpecies, CModel* copasiModel, CCompartment* copasiCompartment) throw(StdException)
{
  if (sbmlSpecies->isSetSubstanceUnits())
    {
      std::string cU = sbmlSpecies->getSubstanceUnits();
      if (cU != "substance")
        {
          throw StdException("Error. Compartment unit other than \"substance\" are not supported.");
        }
    }
  if (sbmlSpecies->isSetSpatialSizeUnits())
    {
      const std::string szU = sbmlSpecies->getSpatialSizeUnits();
      if (szU == "volume")
        {
          /* !!!! create a warning that the spatialSizeUnits will be ignored */
        }
    }
  std::string name = sbmlSpecies->getName();
  if (name == "")
    {
      name = sbmlSpecies->getId();
    }
  std::string appendix = "";
  unsigned int counter = 0;
  while (copasiCompartment->getMetabolites().getIndex(name + appendix) != static_cast<unsigned C_INT32>(-1))
    {
      counter++;
      std::ostringstream numberStream;
      numberStream << "_" << counter;
      appendix = numberStream.str();
    }

  CMetab* copasiMetabolite = copasiModel->createMetabolite(name + appendix, copasiCompartment->getObjectName());
  if (sbmlSpecies->getConstant() || sbmlSpecies->getBoundaryCondition())
    {
      copasiMetabolite->setStatus(CMetab::METAB_FIXED);
    }
  else
    {
      copasiMetabolite->setStatus(CMetab::METAB_VARIABLE);
    }
  if (sbmlSpecies->isSetInitialAmount())
    {
      copasiMetabolite->setInitialNumber(sbmlSpecies->getInitialAmount()*AVOGADRO); // CHECK UNITS !!!
    }
  else if (sbmlSpecies->isSetInitialConcentration())
    {
      copasiMetabolite->setInitialConcentration(sbmlSpecies->getInitialConcentration());      // CHECK UNITS !!!
    }
  //std::cerr << "Created metabolite: " << copasiMetabolite->getObjectName() << std::endl;
  return copasiMetabolite;
}

/**
 * Creates and returns a Copasi CReaction object from the given SBML
 * Reaction object.
 */
CReaction*
SBMLImporter::createCReactionFromReaction(const Reaction* sbmlReaction, const Model* sbmlModel, CModel* copasiModel) throw(StdException)
{
  /* Check if the name of the reaction is unique. */
  std::string name = sbmlReaction->getName();
  if (name == "")
    {
      name = sbmlReaction->getId();
    }
  std::string appendix = "";
  unsigned int counter = 0;
  while (copasiModel->getReactions().getIndex(name + appendix) != static_cast<unsigned C_INT32>(-1))
    {
      counter++;
      std::ostringstream numberStream;
      numberStream << "_" << counter;
      appendix = numberStream.str();
    }

  /* create a new reaction with the unique name */
  CReaction* copasiReaction = copasiModel->createReaction(name + appendix);
  /* Add all substrates to the reaction */
  unsigned int num = sbmlReaction->getNumReactants();
  bool singleCompartment = true;
  const CCompartment* compartment = NULL;
  for (unsigned int counter = 0; counter < num; counter++)
    {
      SpeciesReference* sr = sbmlReaction->getReactant(counter);
      float stoich = sr->getStoichiometry() / sr->getDenominator();
      std::map<std::string, CMetab*>::iterator pos;
      pos = this->speciesMap.find(sr->getSpecies());
      if (pos == this->speciesMap.end())
        {
          throw StdException("Error. Could not find CMetab for key " + sr->getSpecies() + ".");
          //std::cerr << "Error. Could not find CMetab for key " << sr->getSpecies() << "." << std::endl;

          //exit(1);
        }
      if (compartment == NULL)
        {
          compartment = pos->second->getCompartment();
        }
      else
        {
          if (singleCompartment && compartment != pos->second->getCompartment())
            {
              singleCompartment = false;
            }
        }
      copasiReaction->addSubstrate(pos->second->getKey(), stoich);
    }

  /* Add all products to the reaction */
  num = sbmlReaction->getNumProducts();
  for (unsigned int counter = 0; counter < num; counter++)
    {
      SpeciesReference* sr = sbmlReaction->getProduct(counter);
      float stoich = sr->getStoichiometry() / sr->getDenominator();
      std::map<std::string, CMetab*>::iterator pos;
      pos = this->speciesMap.find(sr->getSpecies());
      if (pos == this->speciesMap.end())
        {
          throw StdException("Error. Could not find CMetab for key " + sr->getSpecies() + ".");
          //std::cerr << "Error. Could not find CMetab for key " << sr->getSpecies() << "." << std::endl;
          //exit(1);
        }
      if (compartment == NULL)
        {
          compartment = pos->second->getCompartment();
        }
      else
        {
          if (singleCompartment && compartment != pos->second->getCompartment())
            {
              singleCompartment = false;
            }
        }
      copasiReaction->addProduct(pos->second->getKey(), stoich);
    }

  /* Add all modifiers to the reaction */
  num = sbmlReaction->getNumModifiers();
  for (unsigned int counter = 0; counter < num; counter++)
    {
      ModifierSpeciesReference* sr = sbmlReaction->getModifier(counter);
      std::map<std::string, CMetab*>::iterator pos;
      pos = this->speciesMap.find(sr->getSpecies());
      if (pos == this->speciesMap.end())
        {
          throw StdException("Error. Could not find CMetab for key " + sr->getSpecies() + ".");
          //std::cerr << "Error. Could not find CMetab for key " << sr->getSpecies() << "." << std::endl;
          //exit(1);
        }
      if (compartment == NULL)
        {
          compartment = pos->second->getCompartment();
        }
      else
        {
          if (singleCompartment && compartment != pos->second->getCompartment())
            {
              singleCompartment = false;
            }
        }
      copasiReaction->addModifier(pos->second->getKey());
    }

  /* replace substance names with something more meaningfull */
  /* in the newly created CFunction set the types for all parameters and
  ** either a mapping or a value */
  KineticLaw* kLaw = sbmlReaction->getKineticLaw();
  if (kLaw->isSetSubstanceUnits())
    {
      std::string cU = kLaw->getSubstanceUnits();
      if (cU != "substance")
        {
          throw StdException("Error. KineticLaw substance unit other than \"substance\" are not supported.");
        }
    }
  if (kLaw->isSetTimeUnits())
    {
      std::string cU = kLaw->getTimeUnits();
      if (cU != "time")
        {
          throw StdException("Error. KineticLaw time unit other than \"time\" are not supported.");
        }
    }

  if (!kLaw->isSetMath())
    {
      kLaw->setMathFromFormula();
    }
  const ASTNode* kLawMath = kLaw->getMath();
  ASTNode* node = new ConverterASTNode(*kLawMath);
  node = this->replaceUserDefinedFunctions(node, sbmlModel);
  this->replaceSubstanceNames((ConverterASTNode*)node, sbmlReaction);
  this->replacePowerFunctionNodes(node);
  /* if it is a single compartment reaction, we have to devide the whole kinetic
  ** equation by the volume of the compartment because copasi expects
  ** kinetic laws that specify concentration/time for single compartment
  ** reactions.
  */
  if (singleCompartment)
    {
      if (compartment != NULL)
        {
          /* only divide if the volume is not 1 */
          if (compartment->getVolume() != 1.0)
            {
              /* if the whole function has been multiplied by the same volume
              ** already, drop one level instead of adding one.
              */
              if ((ASTNode_getType(node) == AST_TIMES) && ((ASTNode_getType(ASTNode_getRightChild(node)) == AST_REAL) && (ASTNode_getReal(ASTNode_getRightChild(node)) == compartment->getVolume())))
                {
                  node = node->getLeftChild();
                }
              else
                {
                  ConverterASTNode* tmpNode1 = new ConverterASTNode();
                  tmpNode1->setType(AST_DIVIDE);
                  tmpNode1->addChild(node);
                  ConverterASTNode* tmpNode2 = new ConverterASTNode();
                  tmpNode2->setType(AST_REAL);
                  tmpNode2->setValue(compartment->getVolume());
                  tmpNode1->addChild(tmpNode2);
                  node = tmpNode1;
                }
            }
        }
      else
        {
          throw StdException("Error. Could not determine compartment for single compartment reaction.");
        }
    }

  /* Create a new user defined CKinFunction */
  std::string functionName = "function_4_" + copasiReaction->getObjectName();
  CFunction* cFun = this->functionDB->createFunction(functionName, CFunction::UserDefined);
  //ConverterASTNode::printASTNode(node);
  //std::cerr << "Kinetic Law: " << SBML_formulaToString(node) << std::endl;
  cFun->setDescription(SBML_formulaToString(node));
  cFun->setType(CFunction::UserDefined);
  cFun->setReversible(sbmlReaction->getReversible() ? TriTrue : TriFalse);
  //create parameters
  std::vector<CNodeK*>& v = dynamic_cast<CKinFunction*>(cFun)->getNodes();
  for (unsigned int counter = 0; counter < v.size(); counter++)
    {
      /* assign a type and a mapping */
      CNodeK* node = v[counter];
      if (node->isIdentifier())
        {
          std::string nodeName = node->getName();
          /* if the name start with "substrate_" it is a substrate
             if it starts with product_ it is a product
             if it start with modifier_ it is a modifier
             else it is a parameter
             */
          if (nodeName.find("substrate_") == 0)
            {
              cFun->addParameter(nodeName, CFunctionParameter::FLOAT64, "SUBSTRATE");
            }
          else if (nodeName.find("product_") == 0)
            {
              cFun->addParameter(nodeName, CFunctionParameter::FLOAT64, "PRODUCT");
            }
          else if (nodeName.find("modifier_") == 0)
            {
              cFun->addParameter(nodeName, CFunctionParameter::FLOAT64, "MODIFIER");
            }
          else
            {
              bool found = false;
              /* first check if the parameter is defined in the reaction */
              for (unsigned int x = 0; x < sbmlReaction->getKineticLaw()->getNumParameters(); x++)
                {
                  Parameter* p = sbmlReaction->getKineticLaw()->getParameter(x);
                  if (p->isSetUnits())
                    {
                      /* !!! */
                      /* create a warning that the units will be ignored */
                    }
                  if (p->getId() == nodeName)
                    {
                      found = true;
                      cFun->addParameter(nodeName, CFunctionParameter::FLOAT64, "PARAMETER");
                      break;
                    }
                }
              /* if the paramter was not defined in the reaction, check if it
               * is a global parameter */
              if (!found)
                {
                  for (unsigned int x = 0; x < sbmlModel->getNumParameters(); x++)
                    {
                      Parameter* parameter = sbmlModel->getParameter(x);
                      if (parameter->getId() == nodeName)
                        {
                          found = true;
                          cFun->addParameter(nodeName, CFunctionParameter::FLOAT64, "PARAMETER");
                          break;
                        }
                    }
                }
              /* If we have not found the parameter yet, we have a problem.
               * */
              if (!found)
                {
                  throw StdException("Error. Unknown SBML parameter " + nodeName + ".");
                }
            }
        }
    }

  //for internal structures in the function that can only be set after defining the parameters
  static_cast<CKinFunction*>(cFun)->compile();

  /* create a unique function name by adding the unique reaction name to some
   * prefix */
  copasiReaction->setFunction(cFun);
  /* do the mapping from reaction metabolites to the parameters of the
   * kinetic function */
  for (unsigned int counter = 0; counter < v.size(); counter++)
    {
      /* assign a type and a mapping */
      CNodeK* node = v[counter];
      if (node->isIdentifier())
        {
          std::string nodeName = node->getName();
          /* if the name start with "substrate_" it is a substrate
             if it starts with product_ it is a product
             if it start with modifier_ it is a modifier
             else it is a parameter
             */
          if (nodeName.find("substrate_") == 0)
            {
              std::string speciesKey = nodeName.substr(10);
              CMetab* cM = this->speciesMap[speciesKey];
              if (cM != NULL)
                {
                  copasiReaction->setParameterMapping(nodeName, cM->getKey());
                }
              else
                {
                  throw StdException("Error. Could not find CMetab for key " + speciesKey + ".");
                }
            }
          else if (nodeName.find("product_") == 0)
            {
              std::string speciesKey = nodeName.substr(8);
              CMetab* cM = this->speciesMap[speciesKey];
              if (cM != NULL)
                {
                  copasiReaction->setParameterMapping(nodeName, cM->getKey());
                }
              else
                {
                  throw StdException("Error. Could not find CMetab for key " + speciesKey + ".");
                }
            }
          else if (nodeName.find("modifier_") == 0)
            {
              std::string speciesKey = nodeName.substr(9);
              CMetab* cM = this->speciesMap[speciesKey];
              if (cM != NULL)
                {
                  copasiReaction->setParameterMapping(nodeName, cM->getKey());
                }
              else
                {
                  throw StdException("Error. Could not find CMetab for key " + speciesKey + ".");
                }
            }
          else
            {
              bool found = false;
              for (unsigned int x = 0; x < sbmlReaction->getKineticLaw()->getNumParameters(); x++)
                {
                  Parameter* p = sbmlReaction->getKineticLaw()->getParameter(x);
                  if (p->getId() == nodeName)
                    {
                      found = true;
                      copasiReaction->setParameterValue(nodeName, p->getValue());
                      break;
                    }
                }
              if (!found)
                {
                  for (unsigned int x = 0; x < sbmlModel->getNumParameters(); x++)
                    {
                      Parameter* parameter = sbmlModel->getParameter(x);
                      if (parameter->getId() == nodeName)
                        {
                          found = true;
                          copasiReaction->setParameterValue(nodeName, parameter->getValue());
                          break;
                        }
                    }
                }
              if (!found)
                {
                  //std::cerr << "Could not find parameter: " << nodeName << std::endl;
                  throw StdException("Error. Unknown SBML parameter " + nodeName + ".");
                }
            }
        }
    }
  copasiReaction->setReversible(sbmlReaction->getReversible());
  //std::cerr << "Created reaction: " << copasiReaction->getObjectName() << std::endl;
  return copasiReaction;
}

/**
 * Traverses the brach of the given AST node recursively and prefixes all substrate
 * parameters with "substrate_", all product parameters with "product_" and all modifier parameters with "modifier_".
 */
void
SBMLImporter::replaceSubstanceNames(ConverterASTNode* node, const Reaction* reaction)
{
  std::map< std::string, std::map<std::string, std::string> > substances;
  substances["substrates"] = std::map< std::string, std::string >();
  substances["products"] = std::map< std::string, std::string >();
  substances["modifiers"] = std::map< std::string, std::string >();
  for (unsigned int counter = 0; counter < reaction->getNumReactants(); counter++)
    {
      std::string name = reaction->getReactant(counter)->getSpecies();
      if (name.find("substrate_") == 0)
        {
          substances["substrates"][name] = name;
        }
      else
        {
          substances["substrates"][name] = "substrate_" + name;
        }
    }
  for (unsigned int counter = 0; counter < reaction->getNumProducts(); counter++)
    {
      std::string name = reaction->getProduct(counter)->getSpecies();
      if (name.find("product_") == 0)
        {
          substances["products"][name] = name;
        }
      else
        {
          substances["products"][name] = "product_" + name;
        }
    }
  for (unsigned int counter = 0; counter < reaction->getNumModifiers(); counter++)
    {
      std::string name = reaction->getModifier(counter)->getSpecies();
      if (name.find("modifier_") == 0)
        {
          substances["modifiers"][name] = name;
        }
      else
        {
          substances["modifiers"][name] = "modifier_" + name;
        }
    }
  this->replaceSubstanceNames(node, substances, reaction->getReversible());
}

/**
 * Recursively replaces all parameter names in the branch of the given AST node
 * with the ones give in the map.
 */
void
SBMLImporter::replaceSubstanceNames(ConverterASTNode* node, std::map< std::string, std::map<std::string, std::string > > substMap, bool reversible)
{
  if (node->isName())
    {
      std::map<std::string, std::string> substances = substMap["products"];
      if (reversible)
        {
          std::map<std::string, std::string>::iterator it = substances.find(node->getName());
          if (it != substances.end())
            {
              node->setName(it->second.c_str());
            }
        }
      substances = substMap["substrates"];
      std::map<std::string, std::string>::iterator it = substances.find(node->getName());
      if (it != substances.end())
        {
          node->setName(it->second.c_str());
        }
      substances = substMap["modifiers"];
      it = substances.find(node->getName());
      if (it != substances.end())
        {
          node->setName(it->second.c_str());
        }
    }
  else
    {
      for (unsigned int counter = 0; counter < node->getNumChildren(); counter++)
        {
          this->replaceSubstanceNames((ConverterASTNode*)node->getChild(counter), substMap, reversible);
        }
    }
}

/**
 * Replaces SBML user defined functions with the actual function definition.
 */
ConverterASTNode*
SBMLImporter::replaceUserDefinedFunctions(ASTNode* node, const Model* sbmlModel)
{
  /* copy the original node and make the list of children an empty list. */
  ConverterASTNode* newNode = new ConverterASTNode(*node);
  newNode->setChildren(new List());
  for (unsigned int counter = 0; counter < node->getNumChildren(); counter++)
    {
      ASTNode* child = node->getChild(counter);
      /* check if the child is a user defined function */
      if (child->getType() == AST_FUNCTION)
        {
          const std::string functionName = child->getName();
          /* try to find the actual function definition for that name */
          FunctionDefinition* uDefFun = this->getFunctionDefinitionForName(functionName, sbmlModel);
          if (uDefFun != NULL)
            {
              const ASTNode* uDefFunMath = uDefFun->getMath();
              std::map<std::string, ASTNode*> bvarMap = this->createBVarMap(uDefFunMath, child);
              /* create a new child node where the call to the function
               * definition and its children have been replaced by the actual
               * function definition */
              ConverterASTNode* temp = this->replaceBvars(uDefFunMath, bvarMap);
              newNode->addChild(temp);
            }
          else
            {
              throw StdException("Error. Unknown user defined function " + functionName + ".");
            }
        }
      else
        {
          newNode->addChild(this->replaceUserDefinedFunctions(child, sbmlModel));
        }
    }
  return newNode;
}

/**
 * Creates a map of each parameter of the function definition and its
 * corresponding parameter in the function call.
 */
std::map<std::string, ASTNode*>
SBMLImporter::createBVarMap(const ASTNode* uDefFunction, const ASTNode* function)
{
  /* the first n-1 children, where n is the number of children, of a function definition ASTnode are the
   * arguments to the function. These correspond to the m=n-1 children of the
   * function call.
   */
  std::map<std::string, ASTNode*> varMap;
  for (unsigned int counter = 0; counter < uDefFunction->getNumChildren() - 1; counter++)
    {
      varMap[uDefFunction->getChild(counter)->getName()] = function->getChild(counter);
    }
  return varMap;
}

/**
 * Returns the user defined SBML function definition that belongs to the given
 * name, or NULL if none can be found.
 */
FunctionDefinition*
SBMLImporter::getFunctionDefinitionForName(const std::string name, const Model* sbmlModel)
{
  FunctionDefinition* fDef = NULL;
  for (unsigned int counter = 0; counter < sbmlModel->getNumFunctionDefinitions(); counter++)
    {
      if (sbmlModel->getFunctionDefinition(counter)->getId() == name)
        {
          fDef = sbmlModel->getFunctionDefinition(counter);
          break;
        }
    }
  return fDef;
}

/**
 * Replaces the variables in a function definition with the actual function
 * parameters that were used when the function was called. The function returns
 * a pointer to the ConverterAST node with the replaced variables.
 */
ConverterASTNode*
SBMLImporter::replaceBvars(const ASTNode* node, std::map<std::string, ASTNode*> bvarMap)
{
  ConverterASTNode* newNode = new ConverterASTNode(*node);
  if (node->isName())
    {
      /* check if name matches any in bvarMap */
      /* if yes, replace node with node in bvarMap */
      /* node needs to be set to be a deep copy of the replacement */
      if (bvarMap.find(node->getName()) != bvarMap.end())
        {
          newNode->setName(bvarMap[newNode->getName()]->getName());
        }
    }
  else
    {
      newNode->setChildren(new List());
      for (unsigned int counter = 0; counter < node->getNumChildren(); counter++)
        {
          newNode->addChild(this->replaceBvars(node->getChild(counter), bvarMap));
        }
    }
  return newNode;
}

/**
 * Constructor that initializes speciesMap and the FunctionDB object
 */
SBMLImporter::SBMLImporter()
{
  this->speciesMap = std::map<std::string, CMetab*>();
  this->functionDB = NULL;
}

/**
 * Destructor that does nothing. 
 */
SBMLImporter::~SBMLImporter()
{}

/**
 * This function replaces the AST_FUNCTION_POWER ASTNodes in a ASTNode tree
 * with the AST_POWER node.
 */
void SBMLImporter::replacePowerFunctionNodes(ASTNode* node)
{
  if (node->getType() == AST_FUNCTION_POWER)
    {
      //node->setType(AST_POWER);
      node->setCharacter('^');
    }
  for (unsigned int counter = 0; counter < node->getNumChildren(); counter++)
    {
      this->replacePowerFunctionNodes(node->getChild(counter));
    }
}

/**
 * Function reads an SBML file with libsbml and converts it to a Copasi CModel
 * object which is returned. Deletion of the returned pointer is up to the
 * caller.
 */
CModel*
SBMLImporter::readSBML(std::string filename, CFunctionDB* funDB) throw(StdException)
{
  if (funDB != NULL)
    {
      this->functionDB = funDB;
      SBMLReader* reader = new SBMLReader(XML_SCHEMA_VALIDATION_NONE);
      SBMLDocument* sbmlDoc = reader->readSBML(filename);
      delete reader;
      //std::cerr << "Number of Compartments: " << sbmlDoc->getModel()->getNumCompartments() << std::endl;
      //std::cerr << "Number of Metabolites: "  << sbmlDoc->getModel()->getNumSpecies() << std::endl;
      //std::cerr << "Number of Reactions: "    << sbmlDoc->getModel()->getNumReactions()  << std::endl;

      CModel* model = this->createCModelFromSBMLDocument(sbmlDoc);
      delete sbmlDoc;
      return model;
    }
  else
    {
      throw StdException("Error. readSBML needs a valid CFunctionDB object.");
    }
}

/**
 * Constructor that sets the error message object to "Error."
 */
StdException::StdException() throw()
{
  this->message = "Error.";
}

/**
 * Constructor that sets the error message object to the string given.
 */
StdException::StdException(const std::string what) throw()
{
  this->message = what;
};

/**
 * Destructor that does nothing.
 */
StdException::~StdException() throw()
{}

/**
 * Returns the error message object as a character array.
 */
const char* StdException::what() const throw()
{
  return this->message.c_str();
}
