/**
 * CFunctionParameter
 * 
 * Created for Copasi by Stefan Hoops
 * (C) Stefan Hoops 2001
 */

#ifndef COPASI_CFunctionParameter
#define COPASI_CFunctionParameter

#include <string>
#include <iostream>

#include "utilities/utilities.h"
#include "report/CCopasiContainer.h"

/** @dia:pos 15.1888,24.6765 */
class CFunctionParameter : CCopasiContainer
  {
  public:
    static const std::string DataTypeName[];

    //Attributes

  public:
    /**
     *  Valid data type for a function parameter
     */
    enum DataType {INT32 = 1, FLOAT64 = 5, VINT32 = 7, VFLOAT64 = 11};

  private:
    /**
     *  The name of the parameter. 
     */
    std::string & mName;

    /**
     *  The data type of the parameter this may be one of INT16, INT32, 
     *  UINT16, UINT32, FLOAT32, FLOAT64, VINT16, VINT32, VUINT16, VUINT32,
     *  VFLOAT32, VFLOAT64
     */
    CFunctionParameter::DataType mType;

    /**
     *  The usage or meaning of the parameter. This has no functional 
     *  implication for CFunction but it might be used in derived classes.
     *  Possible usage is SUBSTRATE, PRODUCT, MODIFIER, or PARAMETER
     */
    std::string mUsage;

  public:
    /**
     * Default constructor 
     * @param const std::string & name (default: "NoName")
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CFunctionParameter(const std::string & name = "NoName",
                       const CCopasiContainer * pParent = NULL);

    /**
     * Copy constructor
     * @param "const CFunctionParameter" & src
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CFunctionParameter(const CFunctionParameter & src,
                       const CCopasiContainer * pParent = NULL);

    /**
     * Specified constuctor, sets all member values at once
     * @param "const string &" name
     * @param "const CFunctionParameter::DataType &" type
     * @param "const string &" usage
     * @param const CCopasiContainer * pParent (default: NULL)
     */
    CFunctionParameter(const std::string & name,
                       const DataType & type,
                       const std::string & usage,
                       const CCopasiContainer * pParent = NULL);

    /**
     *  Destructor
     */
    virtual ~CFunctionParameter();

    /**
     *  Cleanup
     */
    void cleanup();

    /**
     *  Loads an object with data coming from a CReadConfig object. 
     *  (CReadConfig object reads an input stream)
     *  @param pconfigbuffer reference to a CReadConfig object.
     *  @return Fail
     */
    virtual void load(CReadConfig & configbuffer,
                      CReadConfig::Mode mode = CReadConfig::NEXT);

    /**
     *  Saves the contents of the object to a CWriteConfig object.
     * (Which usually has a file attached but may also have socket)
     *  @param pconfigbuffer reference to a CWriteConfig object.
     *  @return Fail
     */
    virtual void save(CWriteConfig & configbuffer);

    /**
     *  Retrieves the name of the parameter
     *  @return  "const string" & name
     */
    const std::string & getName() const;

    /**
     *  Sets the name of the parameter
     *  @param "const string" & name
     */
    void setName(const std::string & name);

    /**
     *  Retrieves the data type of the parameter
     *  @return "const CFunctionParameter::DataType" & type
     */
    const CFunctionParameter::DataType & getType() const;

    /**
     *  Sets the data type of the parameter
     *  @param "const CFunctionParameter::DataType" & type
     */
    void setType(const CFunctionParameter::DataType & type);

    /**
     *  Retrieves the usage of the parameter
     *  @return "const string" & usage
     */
    const std::string & getUsage() const;

    /**
     *  Sets the usage of the parameter
     *  @param "const string" & usage
     */
    void setUsage(const std::string & usage);

    /**
     * insert operator
     */
    friend std::ostream& operator<<(std::ostream &os, const CFunctionParameter & d)
    {
      os << "CFunctionParameter: "
      << d.mName
      << " mType " << d.mType
      << " mUsage " << d.mUsage << std::endl;

      return os;
    }
  };

#endif // COPASI_CFunctionParameter
