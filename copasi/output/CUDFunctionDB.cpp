/*****************************************************************************
* PROGRAM NAME: CUDFunctionDB.h
* PROGRAMMER: Wei Sun	wsun@vt.edu
* PURPOSE: Define the user defined function DB object to hold all the user
*		   defined function				
*****************************************************************************/

#include "copasi.h"
#include "output/output.h"
#include "utilities/utilities.h"
#include "CUDFunctionDB.h"


/**
 *
 */
CUDFunctionDB::CUDFunctionDB() {}

/**
 *
 */
CUDFunctionDB::~CUDFunctionDB() {}

/**
 *
 */
void CUDFunctionDB::cleanup() {mUDFunctions.cleanup();}

/**
 *  Loads an object with data coming from a CReadConfig object.
 *  (CReadConfig object reads an input stream)
 *  @param pconfigbuffer reference to a CReadConfig object.
 *  @return mFail
 */
C_INT32 CUDFunctionDB::load(CReadConfig & configbuffer)
{
  CUDFunction * udFunction = NULL;
  C_INT32	Fail = 0;
  
  if (configbuffer.getVersion() < "4")
    {
      if ((Fail = configbuffer.getVariable("User-defined functions",
                                           "string", &mNameStr,
                                           CReadConfig::LOOP)))
        return Fail;

      if ((Fail = configbuffer.getVariable("Items", "C_INT32", &mItems)))
        return Fail;
    }
  else
      if ((Fail = configbuffer.getVariable("TotalOutputFunctions", "C_INT32",
                                           &mItems)))
        return Fail;
    
  // Load each user defined function
  for (C_INT32 i = 0; i < mItems; i++)
    {
      udFunction = new CUDFunction;
      udFunction->load(configbuffer);
      mUDFunctions.add(udFunction);
    }
  
  return Fail;
}

/**
 *  Saves the contents of the object to a CWriteConfig object.
 *  (Which usually has a file attached but may also have socket)
 *  @param pconfigbuffer reference to a CWriteConfig object.
 *  @return mFail
 */
C_INT32 CUDFunctionDB::save(CWriteConfig & configbuffer)
{
  C_INT32 Size = mUDFunctions.size();
  C_INT32 Fail = 0;

  if ((Fail = configbuffer.setVariable("TotalOutputFunctions",
                                       "C_INT32", &Size)))
    return Fail;

  for (C_INT32 i = 0; i < Size; i++)
    mUDFunctions[i]->save(configbuffer);
  
  return Fail;
}

/**
 *  Add the function to the database
 *  @param CUDFunction &function
 *  @return C_INT32 Fail
 */
void CUDFunctionDB::add(CUDFunction & function)
{
}


/**
 * Get the number of user defined functions
 */
C_INT16 CUDFunctionDB::getItems() const
{
	return mItems;
}
  
/**
 * Set the number of user defined funcstion
 */
void CUDFunctionDB::setItems(const C_INT16 items)
{
	mItems = items;
}

/**
 * Get the UDFunction array
 */
CCopasiVectorN < CUDFunction > & CUDFunctionDB::getFunctions()
{
	return mUDFunctions;
}
