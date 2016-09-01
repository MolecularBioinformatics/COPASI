// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2005 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include "copasi.h"
#include "CEvaluationNode.h"

#include "sbml/math/ASTNode.h"
#include "sbml/ConverterASTNode.h"
#include "sbml/util/List.h"

#include "utilities/CNodeIterator.h"

CEvaluationNode::CPrecedence::CPrecedence(const size_t & left,
    const size_t & right):
  left(left),
  right(right)
{}

CEvaluationNode::CPrecedence::CPrecedence(const CPrecedence & src):
  left(src.left),
  right(src.right)
{}

CEvaluationNode::CPrecedence::~CPrecedence() {}

CEvaluationNode * CEvaluationNode::create(const CEvaluationNode::MainType & mainType,
    const CEvaluationNode::SubType & subType,
    const std::string & data)
{
  CEvaluationNode * pNode = NULL;

  switch (mainType)
    {
      case T_CALL:
        pNode = new CEvaluationNodeCall(subType, data);
        break;

      case T_CHOICE:
        pNode = new CEvaluationNodeChoice(subType, data);
        break;

      case T_CONSTANT:
        pNode = new CEvaluationNodeConstant(subType, data);
        break;

      case T_DELAY:
        pNode = new CEvaluationNodeDelay(subType, data);
        break;

      case T_FUNCTION:
        pNode = new CEvaluationNodeFunction(subType, data);
        break;

      case T_LOGICAL:
        pNode = new CEvaluationNodeLogical(subType, data);
        break;

      case T_NUMBER:
        pNode = new CEvaluationNodeNumber(subType, data);
        break;

      case T_OBJECT:
        pNode = new CEvaluationNodeObject(subType, data);
        break;

      case T_OPERATOR:
        pNode = new CEvaluationNodeOperator(subType, data);
        break;

      case T_STRUCTURE:
        pNode = new CEvaluationNodeStructure(subType, data);
        break;

      case T_VARIABLE:
        pNode = new CEvaluationNodeVariable(subType, data);
        break;

      case T_VECTOR:
        pNode = new CEvaluationNodeVector(subType, data);
        break;

      case T_WHITESPACE:
        pNode = new CEvaluationNodeWhiteSpace(subType, data);
        break;

      case T_INVALID:
        pNode = new CEvaluationNode();
        break;

      case T_MV_FUNCTION:
        break;
    }

  return pNode;
}

const CEvaluationNode::SubType & CEvaluationNode::subType() const
{
  return mSubType;
}

const CEvaluationNode::MainType & CEvaluationNode::mainType() const
{
  return mMainType;
}

// static
const char * CEvaluationNode::Keywords[] =
{
  "log", "LOG",
  "log10", "LOG10",
  "exp", "EXP",
  "sin", "SIN",
  "cos", "COS",
  "tan", "TAN",
  "sec", "SEC",
  "csc", "CSC",
  "cot", "COT",
  "sinh", "SINH",
  "cosh", "COSH",
  "tanh", "TANH",
  "sech", "SECH",
  "csch", "CSCH",
  "coth", "COTH",
  "asin", "ASIN",
  "acos", "ACOS",
  "atan", "ATAN",
  "arcsec", "ARCSEC",
  "arccsc", "ARCCSC",
  "arccot", "ARCCOT",
  "arcsinh", "ARCSINH",
  "arccosh", "ARCCOSH",
  "arctanh", "ARCTANH",
  "arcsech", "ARCSECH",
  "arccsch", "ARCCSCH",
  "arccoth", "ARCCOTH",
  "sqrt", "SQRT",
  "abs", "ABS",
  "floor", "FLOOR",
  "ceil", "CEIL",
  "factorial", "FACTORIAL",
  "uniform", "UNIFORM",
  "normal", "NORMAL",
  "gamma", "GAMMA",
  "poisson", "POISSON",
  "max", "MAX",
  "min", "MIN",
  "delay", "DELAY",
  "if", "IF",
  NULL
};

// static
bool CEvaluationNode::isKeyword(const std::string & str)
{
  const char ** pKeyword = Keywords;

  for (; *pKeyword != NULL; ++pKeyword)
    if (!strcmp(str.c_str(), *pKeyword)) return true;

  return false;
}

CEvaluationNode::CEvaluationNode():
  CCopasiNode<Data>(""),
  mMainType(T_INVALID),
  mSubType(S_INVALID),
  mValue(std::numeric_limits<C_FLOAT64>::quiet_NaN()),
  mpValue(NULL),
  mPrecedence(PRECEDENCE_DEFAULT)
{
  mpValue = & mValue;
}

CEvaluationNode::CEvaluationNode(const MainType & mainType,
                                 const SubType & subType,
                                 const Data & data):
  CCopasiNode<Data>(data),
  mMainType(mainType),
  mSubType(subType),
  mValue(std::numeric_limits<C_FLOAT64>::quiet_NaN()),
  mpValue(NULL),
  mPrecedence(PRECEDENCE_DEFAULT)
{
  mpValue = & mValue;
}

CEvaluationNode::CEvaluationNode(const CEvaluationNode & src):
  CCopasiNode<Data>(src),
  mMainType(src.mMainType),
  mSubType(src.mSubType),
  mValue(src.mValue),
  mpValue(NULL),
  mPrecedence(src.mPrecedence)
{
  mpValue = & mValue;
}

CEvaluationNode::~CEvaluationNode() {}

bool CEvaluationNode::compile(const CEvaluationTree * /* pTree */)
{return true;}

// virtual
std::string CEvaluationNode::getInfix(const std::vector< std::string > & /* children */) const
{return mData;}

std::string CEvaluationNode::buildInfix() const
{
  std::string Infix = "";
  CNodeContextIterator< const CEvaluationNode, std::vector< std::string > > it(this);

  while (it.next() != it.end())
    {
      if (*it != NULL)
        {
          if (it.parentContextPtr() != NULL)
            {
              it.parentContextPtr()->push_back(it->getInfix(it.context()));
            }
          else
            {
              Infix = it->getInfix(it.context());
            }
        }
    }

  return Infix;
}

// virtual
std::string CEvaluationNode::getDisplayString(const std::vector< std::string > & /* children */) const
{return mData;}

std::string CEvaluationNode::buildDisplayString() const
{
  std::string DisplayString = "";
  CNodeContextIterator< const CEvaluationNode, std::vector< std::string > > it(this);

  while (it.next() != it.end())
    {
      if (*it != NULL)
        {
          if (it.parentContextPtr() != NULL)
            {
              it.parentContextPtr()->push_back(it->getDisplayString(it.context()));
            }
          else
            {
              DisplayString = it->getDisplayString(it.context());
            }
        }
    }

  return DisplayString;
}

// virtual
std::string CEvaluationNode::getCCodeString(const std::vector< std::string > &  /* children */) const
{return mData;}

/**
 * Build the C-code string.
 */
std::string CEvaluationNode::buildCCodeString() const
{
  std::string CCodeString = "";
  CNodeContextIterator< const CEvaluationNode, std::vector< std::string > > it(this);

  while (it.next() != it.end())
    {
      if (*it != NULL)
        {
          if (it.parentContextPtr() != NULL)
            {
              it.parentContextPtr()->push_back(it->getCCodeString(it.context()));
            }
          else
            {
              CCodeString = it->getCCodeString(it.context());
            }
        }
    }

  return CCodeString;
}

// virtual
std::string CEvaluationNode::getBerkeleyMadonnaString(const std::vector< std::string > & /* children */) const
{return mData;}

std::string CEvaluationNode::buildBerkeleyMadonnaString() const
{
  std::string BerkeleyMadonnaString = "";
  CNodeContextIterator< const CEvaluationNode, std::vector< std::string > > it(this);

  while (it.next() != it.end())
    {
      if (*it != NULL)
        {
          if (it.parentContextPtr() != NULL)
            {
              it.parentContextPtr()->push_back(it->getBerkeleyMadonnaString(it.context()));
            }
          else
            {
              BerkeleyMadonnaString = it->getBerkeleyMadonnaString(it.context());
            }
        }
    }

  return BerkeleyMadonnaString;
}

// virtual
std::string CEvaluationNode::getXPPString(const std::vector< std::string > & /* children */) const
{return mData;}

std::string CEvaluationNode::buildXPPString() const
{
  std::string BerkeleyMadonnaString = "";
  CNodeContextIterator< const CEvaluationNode, std::vector< std::string > > it(this);

  while (it.next() != it.end())
    {
      if (*it != NULL)
        {
          if (it.parentContextPtr() != NULL)
            {
              it.parentContextPtr()->push_back(it->getXPPString(it.context()));
            }
          else
            {
              BerkeleyMadonnaString = it->getXPPString(it.context());
            }
        }
    }

  return BerkeleyMadonnaString;
}

// virtual
bool CEvaluationNode::isBoolean() const
{return false;}

void CEvaluationNode::addChildren(const std::vector< CEvaluationNode * > & children)
{
  std::vector< CEvaluationNode * >::const_iterator it = children.begin();
  std::vector< CEvaluationNode * >::const_iterator end = children.end();

  for (; it != end; ++it)
    {
      addChild(*it);
    }
}

bool CEvaluationNode::operator < (const CEvaluationNode & rhs)
{return (mPrecedence.right < rhs.mPrecedence.left);}

CEvaluationNode* CEvaluationNode::copyNode(CEvaluationNode* child1, CEvaluationNode* child2) const
{
  std::vector<CEvaluationNode*> children;

  if (child1 != NULL) children.push_back(child1);

  if (child2 != NULL) children.push_back(child2);

  return copyNode(children);
}

CEvaluationNode* CEvaluationNode::copyNode(const std::vector<CEvaluationNode*>& children) const
{
  CEvaluationNode * pNode = create(mMainType, mSubType, getData());
  std::vector<CEvaluationNode*>::const_iterator it = children.begin();
  std::vector<CEvaluationNode*>::const_iterator endit = children.end();

  while (it != endit)
    {
      pNode->addChild(*it);
      ++it;
    }

  return pNode;
}

CEvaluationNode * CEvaluationNode::copyBranch() const
{
  CEvaluationNode * pNode = NULL;
  CNodeContextIterator< const CEvaluationNode, std::vector< CEvaluationNode * > > itNode(this);

  while (itNode.next() != itNode.end())
    {
      if (*itNode == NULL)
        {
          continue;
        }

      if (itNode.parentContextPtr() != NULL)
        {
          itNode.parentContextPtr()->push_back(itNode->copyNode(itNode.context()));
        }
      else
        {
          assert(*itNode == this);
          pNode = itNode->copyNode(itNode.context());
        }
    }

  return pNode;
}

CEvaluationNode* CEvaluationNode::simplifyNode(const std::vector<CEvaluationNode*>& children) const
{
  CEvaluationNode *newnode = copyNode(children);
  return newnode;
}

ASTNode* CEvaluationNode::toAST(const CCopasiDataModel* /*pDataModel*/) const
{
  return new ASTNode();
}

const C_FLOAT64 * CEvaluationNode::getValuePointer() const
{return mpValue;}

// virtual
std::string CEvaluationNode::getMMLString(const std::vector< std::string > & /* children */,
    bool /* expand */,
    const std::vector< std::vector< std::string > > & /* variables */) const
{
  return "";
}

std::string CEvaluationNode::buildMMLString(bool expand,
    const std::vector< std::vector< std::string > > & variables) const
{
  std::string MMLString = "";
  CNodeContextIterator< const CEvaluationNode, std::vector< std::string > > it(this);

  while (it.next() != it.end())
    {
      if (*it != NULL)
        {
          if (it.parentContextPtr() != NULL)
            {
              it.parentContextPtr()->push_back(it->getMMLString(it.context(), expand, variables));
            }
          else
            {
              MMLString = it->getMMLString(it.context(), expand, variables);
            }
        }
    }

  return MMLString;
}

// TODO Replace the recursive call (not critical since only used for debug)
void CEvaluationNode::printRecursively(std::ostream & os, int indent) const
{
  int i;

  os << std::endl;

  for (i = 0; i < indent; ++i) os << " ";

  os << "mData: " << mData << std::endl;

  for (i = 0; i < indent; ++i) os << " ";

  os << "mType: " << mMainType << "  subType: " << mSubType << std::endl;

  for (i = 0; i < indent; ++i) os << " ";

  os << "mValue: " << mValue << std::endl;

  CEvaluationNode* tmp;

  tmp = (CEvaluationNode*)getChild();

  while (tmp)
    {
      tmp -> printRecursively(os, indent + 2);
      tmp = (CEvaluationNode*)tmp->getSibling();
    }
}

void CEvaluationNode::printRecursively() const
{
  this->printRecursively(std::cout, 0);
}

CEvaluationNode* CEvaluationNode::splitBranch(const CEvaluationNode* splitnode, bool left) const
{
  if (splitnode == this)
    {
      const CEvaluationNode *child = dynamic_cast<const CEvaluationNode*>(this->getChild());

      if (!child) return NULL;

      if (left)
        {
          return child->copyBranch();
        }
      else
        {
          child = dynamic_cast<const CEvaluationNode*>(child->getSibling());

          if (!child) return NULL;

          return child->copyBranch();
        }
    }
  else
    {
      /*        const CEvaluationNode *child1 = dynamic_cast<const CEvaluationNode*>(getChild());
              CEvaluationNode *newchild1 = NULL;
              CEvaluationNode *newchild2 = NULL;
              if (child1 != NULL)
                {
                  newchild1 = child1->splitBranch(splitnode, left);
                  const CEvaluationNode *child2 = dynamic_cast<const CEvaluationNode*>(child1->getSibling());
                  if (child2 != NULL)
                    {
                      newchild2 = child2->splitBranch(splitnode, left);
                    }
                }
              CEvaluationNode *newnode = copyNode(newchild1, newchild2);
              return newnode;*/

      std::vector<CEvaluationNode*> children;
      const CEvaluationNode* child = dynamic_cast<const CEvaluationNode*>(getChild());

      while (child != NULL)
        {
          CEvaluationNode *newchild = NULL;
          newchild = child->splitBranch(splitnode, left);
          children.push_back(newchild);
          child = dynamic_cast<const CEvaluationNode*>(child->getSibling());
        }

      children.push_back(NULL);
      CEvaluationNode *newnode = copyNode(children);
      return newnode;
    }
}

const CEvaluationNode * CEvaluationNode::findTopMinus(const std::vector<CFunctionAnalyzer::CValue> & callParameters) const
{
  CNodeContextIterator< const CEvaluationNode, std::vector< const CEvaluationNode * > > itNode(this);
  itNode.setProcessingModes(CNodeIteratorMode::Before | CNodeIteratorMode::After);
  const CEvaluationNode * pMinus = NULL;

  while (itNode.next() != itNode.end())
    {
      if (*itNode == NULL)
        {
          continue;
        }

#ifdef COPASI_DEBUG
      std::cout << itNode->getData() << std::endl;
#endif

      switch (itNode.processingMode())
        {
          case CNodeIteratorMode::Before:

            if (itNode->mainType() == T_OPERATOR && itNode->subType() == CEvaluationNode::S_MINUS)
              {
                // We found a minus no need to go down the tree.
                itNode.skipChildren();
                pMinus = *itNode;

                if (itNode.parentContextPtr() != NULL)
                  {
                    itNode.parentContextPtr()->push_back(pMinus);
                  }
              }

            break;

          case CNodeIteratorMode::After:

            if (itNode->mainType() == T_OPERATOR && itNode->subType() == CEvaluationNode::S_MULTIPLY)
              {
                // Left child
                if (itNode.context()[0] != NULL)
                  {
                    // Both children contain a minus, this is not a valid split point.

                    if (itNode.context()[1] != NULL)
                      {
                        pMinus = NULL;
                      }
                    // Check whether the right is positive
                    else if (CFunctionAnalyzer::evaluateNode(static_cast< const CEvaluationNode *>(itNode->getChild(1)),
                             callParameters, CFunctionAnalyzer::NOOBJECT).isPositive())
                      {
                        pMinus = itNode.context()[0];
                      }
                    else
                      {
                        pMinus = NULL;
                      }
                  }
                // Right child
                else if (itNode.context()[1] != NULL)
                  {
                    // Check whether the left is positive
                    if (CFunctionAnalyzer::evaluateNode(static_cast< const CEvaluationNode *>(itNode->getChild(0)),
                                                        callParameters, CFunctionAnalyzer::NOOBJECT).isPositive())
                      pMinus = itNode.context()[1];
                    else
                      pMinus = NULL;
                  }
                else
                  {
                    pMinus = NULL;
                  }
              }
            else if (itNode->mainType() == T_OPERATOR && itNode->subType() == CEvaluationNode::S_DIVIDE)
              {
                // Left child
                pMinus = itNode.context()[0];
              }
            else
              {
                pMinus = NULL;
              }

            if (itNode.parentContextPtr() != NULL)
              {
                itNode.parentContextPtr()->push_back(pMinus);
              }

            break;

          default:
            // This will never happen
            break;
        }
    }

  return pMinus;
}

bool CEvaluationNode::operator!=(const CEvaluationNode& right) const
{
  return !(*this == right);
}

bool CEvaluationNode::operator==(const CEvaluationNode& right) const
{
  CNodeIterator< const CEvaluationNode > itLeft(this);
  CNodeIterator< const CEvaluationNode > itRight(&right);

  while (itLeft.next() != itLeft.end() &&
         itRight.next() != itRight.end())
    {
      if (*itLeft == NULL && *itRight == NULL)
        {
          continue;
        }

      if (*itLeft == NULL || *itRight == NULL)
        {
          return false;
        }

      if (itLeft->mainType() != itRight->mainType() ||
          itLeft->subType() != itRight->subType() ||
          itLeft->getData() != itRight->getData())
        {
          return false;
        }
    }

  return true;
}

bool CEvaluationNode::operator<(const CEvaluationNode& right) const
{
  if (mainType() != right.mainType())
    {
      return mainType() < right.mainType();
    }

  if (subType() != right.subType())
    {
      return subType() < right.subType();
    }

  switch (mainType())
    {
      case T_CONSTANT:
      case T_NUMBER:
      case T_OBJECT:
      case T_CALL:
      case T_STRUCTURE:
      case T_VARIABLE:
      case T_WHITESPACE:
        return getData() < right.getData();
        break;

      case T_OPERATOR:
      case T_FUNCTION:
      case T_CHOICE:
      case T_LOGICAL:
      case T_MV_FUNCTION:
      case T_VECTOR:
      case T_DELAY:
      case T_INVALID:
        break;
    }

  const CEvaluationNode* pChild1 = dynamic_cast<const CEvaluationNode*>(this->getChild());
  const CEvaluationNode* pChild2 = dynamic_cast<const CEvaluationNode*>(right.getChild());

  while (true)
    {
      if (pChild1 == NULL || pChild2 == NULL)
        {
          return pChild1 < pChild2;
        }

      if (*pChild1 < *pChild2) return true;

      pChild1 = dynamic_cast<const CEvaluationNode*>(pChild1->getSibling());
      pChild2 = dynamic_cast<const CEvaluationNode*>(pChild2->getSibling());
    }

  return false;
}
