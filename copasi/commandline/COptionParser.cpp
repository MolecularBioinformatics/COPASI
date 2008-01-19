// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/commandline/COptionParser.cpp,v $
//   $Revision: 1.22.14.2 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2008/01/19 03:57:00 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

/*
 * This file was automaticly generated by clo++ version 0.6.4 (REL-0_6_4)
 * http://pmade.org/pjones/software/clo++/
 *
 * Template enhancements by Stefan Hoops mailto:shoops@vt.edu
 */

/**
 * This file contains the implementation of the COptionParser class.
 * You can control the name of this source file using the
 * cxx_source_file variable.
 */

#ifdef WIN32
#pragma warning (disable: 4786)
#define strcasecmp _stricmp
#endif

#include "COptionParser.h"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <sstream>
#include <errno.h>

namespace
  {
  const char const_usage[] =
    "  --SBMLSchema schema           The Schema of the SBML file to export.\n"
    "  --configdir dir               The configuration directory for copasi. The\n"
    "                                default is .copasi in the home directory.\n"
    "  --configfile file             The configuration file for copasi. The\n"
    "                                default is copasi in the ConfigDir.\n"
    "  --exportBerkeleyMadonna file  The Berkeley Madonna file to export.\n"
    "  --exportC file                The C code file to export.\n"
    "  --home dir                    Your home directory.\n"
    "  --license                     Display the license.\n"
    "  --newExportSBML string        The SBML file to export.\n"
    "  --nologo                      Surpresses the startup message.\n"
    "  --validate                    Validate the given input file (COPASI or\n"
    "                                SBML).\n"
    "  --verbose                     Enable output of messages during runtime to\n"
    "                                std::error.\n"
    "  -c, --copasidir dir           The COPASI installation directory.\n"
    "  -e, --exportSBML file         The SBML file to export.\n"
    "  -i, --importSBML file         A SBML file to import.\n"
    "  -s, --save file               The file the model is saved to after work.\n"
    "  -t, --tmp dir                 The temp directory used for autosave.\n";

  const char const_help_comment[] =
    "use the -h option for help";

  const char* expand_long_name (const std::string &name);
}

//#########################################################################
copasi::COptionParser::COptionParser (void)
    : state_(state_option)
{
  memset(&locations_, 0, sizeof(locations_));
}
//#########################################################################
void copasi::COptionParser::parse(int argc, char *argv[], bool call_finalize)
{
  int i = 1;

#ifdef Darwin
  if (argc > 1)
    {
      std::string tmp(argv[1]);
      if (!tmp.compare(0, 4, "-psn")) i = 2;
    }
#endif // Darwin

  for (; i < argc; ++i) parse_element(argv[i], i, source_cl);

  if (call_finalize) finalize();
}
//#########################################################################
void copasi::COptionParser::parse(const char * fileName)
{
  int LineCounter = 0;
  std::string Line;
  std::string Option;
  std::string Value;
  std::string::size_type pos;
  std::ifstream File(fileName);

  if (File.fail())
    {
      std::ostringstream error;
      error << "error opening file: '" << fileName << "'";
      errno = ENOENT;

      throw option_error(error.str());
    }

  while (true)
    {
      try
        {
          std::getline(File, Line);
          LineCounter++;

          if (File.eof()) break;
          if (File.fail())
            {
              std::ostringstream error;
              error << "unknown problem";

              throw option_error(error.str());
            }

#ifndef WIN32
          /* Take care of dos style line ending under UNIX */
          /* A case where MS has the better implementation */
          pos = Line.length();
          (pos < 1) ? 0 : pos--;
          if (Line[pos] == 0xd) Line.erase(pos);
#endif  // not WIN32

          // eat leading spaces
          Line.erase(0, Line.find_first_not_of(' '));

          // skip comments and empty lines
          if (Line.length() == 0 || Line[0] == '#') continue;

          // find first position of '=' or ':'
          pos = Line.find_first_of(":=");

          Option = "--" + Line.substr(0, pos);
          Option.erase(Option.find_last_not_of(' ') + 1);

          parse_element(Option.c_str(), 0, source_cf);

          if (pos != std::string::npos) // We have a '='
            {
              Value = Line.substr(pos + 1);
              Value.erase(0, Value.find_first_not_of(' '));
              Value.erase(Value.find_last_not_of(' ') + 1);

              if (Value.length()) parse_element(Value.c_str(), 0, source_cf);
            }

          finalize();
        }

      catch (copasi::option_error &e)
        {
          std::ostringstream error;
          error << "error reading file: '" << fileName << "(" << LineCounter
          << ")': " << e.what();

          throw option_error(error.str());
        }
    }

  File.clear();
  File.close();
}
//#########################################################################
void copasi::COptionParser::finalize (void)
{
  if (state_ == state_value)
    {
      switch (openum_)
        {
        case option_ConfigDir:
          throw option_error("missing value for 'configdir' option");
        case option_ConfigFile:
          throw option_error("missing value for 'configfile' option");
        case option_CopasiDir:
          throw option_error("missing value for 'copasidir' option");
        case option_ExportBerkeleyMadonna:
          throw option_error("missing value for 'exportBerkeleyMadonna' option");
        case option_ExportC:
          throw option_error("missing value for 'exportC' option");
        case option_ExportSBML:
          throw option_error("missing value for 'exportSBML' option");
        case option_Home:
          throw option_error("missing value for 'home' option");
        case option_ImportSBML:
          throw option_error("missing value for 'importSBML' option");
        case option_License:
          throw option_error("missing value for 'license' option");
        case option_NewExportSBML:
          throw option_error("missing value for 'newExportSBML' option");
        case option_NoLogo:
          throw option_error("missing value for 'nologo' option");
        case option_RegisteredEmail:
          throw option_error("missing value for 'rEmail' option");
        case option_RegisteredUser:
          throw option_error("missing value for 'rUser' option");
        case option_RegistrationCode:
          throw option_error("missing value for 'rCode' option");
        case option_SBMLSchema:
          throw option_error("missing value for 'SBMLSchema' option");
        case option_Save:
          throw option_error("missing value for 'save' option");
        case option_Tmp:
          throw option_error("missing value for 'tmp' option");
        case option_Validate:
          throw option_error("missing value for 'validate' option");
        case option_Verbose:
          throw option_error("missing value for 'verbose' option");
        }
    }
}
//#########################################################################
void copasi::COptionParser::parse_element (const char *element, int position, opsource source)
{
  size_t length = strlen(element);

  switch (state_)
    {
    case state_consume:
      non_options_.push_back(element);
      break;
    case state_option:
      if (length >= 2 && element[0] == '-' && element[1] == '-')
        {
          if (length == 2) {state_ = state_consume; return;}
          element += 2;
          const char *value = element;
          while (*value != 0 && *value != '=') ++value;
          if (*value == '=')
            {
              std::string selement(element, value - element), svalue(++value);

              parse_long_option(selement.c_str(), position, source);
              if (state_ != state_value)
                {
                  std::string error("the '"); error += element; error += "' option does not take a value";
                  throw option_error(error);
                }

              parse_value(svalue.c_str());
              state_ = state_option;
            }
          else
            {
              parse_long_option(element, position, source);
            }
        }
      else if (length >= 2 && element[0] == '-')
        {
          ++element;

          if (length > 2)
            {
              while (*element != 0)
                {
                  parse_short_option(*element, position, source);
                  ++element;

                  if (state_ == state_value && *element == '=')
                    {
                      parse_value(++element);
                      state_ = state_option;
                      break;
                    }
                  else if (state_ == state_value) finalize();
                }
            }
          else
            {
              parse_short_option(*element, position, source);
            }
        }
      else
        {
          non_options_.push_back(element);
        }
      break;
    case state_value:
      parse_value(element);
      state_ = state_option;
      break;
    }
}
//#########################################################################
void copasi::COptionParser::parse_short_option (char option, int position, opsource source)
{
  switch (option)
    {
    case 'c':
      if (source != source_cl) throw option_error("the 'copasidir' option can only be used on the command line");
      if (locations_.CopasiDir)
        {
          throw option_error("the 'copasidir' option is only allowed once");
        }
      openum_ = option_CopasiDir;
      state_ = state_value;
      locations_.CopasiDir = position;
      return;
    case 'e':
      if (source != source_cl) throw option_error("the 'exportSBML' option can only be used on the command line");
      if (locations_.ExportSBML)
        {
          throw option_error("the 'exportSBML' option is only allowed once");
        }
      openum_ = option_ExportSBML;
      state_ = state_value;
      locations_.ExportSBML = position;
      return;
    case 'i':
      if (source != source_cl) throw option_error("the 'importSBML' option can only be used on the command line");
      if (locations_.ImportSBML)
        {
          throw option_error("the 'importSBML' option is only allowed once");
        }
      openum_ = option_ImportSBML;
      state_ = state_value;
      locations_.ImportSBML = position;
      return;
    case 's':
      source = source; // kill compiler unused variable warning
      if (locations_.Save)
        {
          throw option_error("the 'save' option is only allowed once");
        }
      openum_ = option_Save;
      state_ = state_value;
      locations_.Save = position;
      return;
    case 't':
      source = source; // kill compiler unused variable warning
      if (locations_.Tmp)
        {
          throw option_error("the 'tmp' option is only allowed once");
        }
      openum_ = option_Tmp;
      state_ = state_value;
      locations_.Tmp = position;
      return;
    case 'h':
      if (source != source_cl) break;
      throw autoexcept(autothrow_help, const_usage);
    case '?':
      if (source != source_cl) break;
      throw autoexcept(autothrow_help, const_usage);
    }

  std::string error("unknown option: '"); error += option; error += "'";
  throw option_error(error);
}
//#########################################################################
void copasi::COptionParser::parse_long_option (const char *option, int position, opsource source)
{
  option = expand_long_name(option);

  if (strcmp(option, "SBMLSchema") == 0)
    {
      if (source != source_cl) throw option_error("the 'SBMLSchema' option is only allowed on the command line");
      if (locations_.SBMLSchema)
        {
          throw option_error("the 'SBMLSchema' option is only allowed once");
        }
      openum_ = option_SBMLSchema;
      locations_.SBMLSchema = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "configdir") == 0)
    {
      if (source != source_cl) throw option_error("the 'configdir' option is only allowed on the command line");
      if (locations_.ConfigDir)
        {
          throw option_error("the 'configdir' option is only allowed once");
        }
      openum_ = option_ConfigDir;
      locations_.ConfigDir = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "configfile") == 0)
    {
      if (source != source_cl) throw option_error("the 'configfile' option is only allowed on the command line");
      if (locations_.ConfigFile)
        {
          throw option_error("the 'configfile' option is only allowed once");
        }
      openum_ = option_ConfigFile;
      locations_.ConfigFile = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "copasidir") == 0)
    {
      if (source != source_cl) throw option_error("the 'copasidir' option is only allowed on the command line");
      if (locations_.CopasiDir)
        {
          throw option_error("the 'copasidir' option is only allowed once");
        }
      openum_ = option_CopasiDir;
      locations_.CopasiDir = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "exportBerkeleyMadonna") == 0)
    {
      if (source != source_cl) throw option_error("the 'exportBerkeleyMadonna' option is only allowed on the command line");
      if (locations_.ExportBerkeleyMadonna)
        {
          throw option_error("the 'exportBerkeleyMadonna' option is only allowed once");
        }
      openum_ = option_ExportBerkeleyMadonna;
      locations_.ExportBerkeleyMadonna = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "exportC") == 0)
    {
      if (source != source_cl) throw option_error("the 'exportC' option is only allowed on the command line");
      if (locations_.ExportC)
        {
          throw option_error("the 'exportC' option is only allowed once");
        }
      openum_ = option_ExportC;
      locations_.ExportC = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "exportSBML") == 0)
    {
      if (source != source_cl) throw option_error("the 'exportSBML' option is only allowed on the command line");
      if (locations_.ExportSBML)
        {
          throw option_error("the 'exportSBML' option is only allowed once");
        }
      openum_ = option_ExportSBML;
      locations_.ExportSBML = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "home") == 0)
    {
      if (source != source_cl) throw option_error("the 'home' option is only allowed on the command line");
      if (locations_.Home)
        {
          throw option_error("the 'home' option is only allowed once");
        }
      openum_ = option_Home;
      locations_.Home = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "importSBML") == 0)
    {
      if (source != source_cl) throw option_error("the 'importSBML' option is only allowed on the command line");
      if (locations_.ImportSBML)
        {
          throw option_error("the 'importSBML' option is only allowed once");
        }
      openum_ = option_ImportSBML;
      locations_.ImportSBML = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "license") == 0)
    {
      if (source != source_cl) throw option_error("the 'license' option is only allowed on the command line");
      if (locations_.License)
        {
          throw option_error("the 'license' option is only allowed once");
        }
      openum_ = option_License;
      locations_.License = position;
      options_.License = !options_.License;
      return;
    }
  else if (strcmp(option, "newExportSBML") == 0)
    {
      if (source != source_cl) throw option_error("the 'newExportSBML' option is only allowed on the command line");
      if (locations_.NewExportSBML)
        {
          throw option_error("the 'newExportSBML' option is only allowed once");
        }
      openum_ = option_NewExportSBML;
      locations_.NewExportSBML = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "nologo") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.NoLogo)
        {
          throw option_error("the 'nologo' option is only allowed once");
        }
      openum_ = option_NoLogo;
      locations_.NoLogo = position;
      options_.NoLogo = !options_.NoLogo;
      return;
    }
  else if (strcmp(option, "rCode") == 0)
    {
      if (source != source_cl) throw option_error("the 'rCode' option is only allowed on the command line");
      if (locations_.RegistrationCode)
        {
          throw option_error("the 'rCode' option is only allowed once");
        }
      openum_ = option_RegistrationCode;
      locations_.RegistrationCode = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "rEmail") == 0)
    {
      if (source != source_cl) throw option_error("the 'rEmail' option is only allowed on the command line");
      if (locations_.RegisteredEmail)
        {
          throw option_error("the 'rEmail' option is only allowed once");
        }
      openum_ = option_RegisteredEmail;
      locations_.RegisteredEmail = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "rUser") == 0)
    {
      if (source != source_cl) throw option_error("the 'rUser' option is only allowed on the command line");
      if (locations_.RegisteredUser)
        {
          throw option_error("the 'rUser' option is only allowed once");
        }
      openum_ = option_RegisteredUser;
      locations_.RegisteredUser = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "save") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.Save)
        {
          throw option_error("the 'save' option is only allowed once");
        }
      openum_ = option_Save;
      locations_.Save = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "tmp") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.Tmp)
        {
          throw option_error("the 'tmp' option is only allowed once");
        }
      openum_ = option_Tmp;
      locations_.Tmp = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "validate") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.Validate)
        {
          throw option_error("the 'validate' option is only allowed once");
        }
      openum_ = option_Validate;
      locations_.Validate = position;
      options_.Validate = !options_.Validate;
      return;
    }
  else if (strcmp(option, "verbose") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.Verbose)
        {
          throw option_error("the 'verbose' option is only allowed once");
        }
      openum_ = option_Verbose;
      locations_.Verbose = position;
      options_.Verbose = !options_.Verbose;
      return;
    }
  else if (source == source_cl && strcmp(option, "help") == 0)
    {
      throw autoexcept(autothrow_help, const_usage);
    }

  std::string error("unknown option '"); error += option; error += "'";
  throw option_error(error);
}
//#########################################################################
void copasi::COptionParser::parse_value (const char *value)
{
  switch (openum_)
    {
    case option_ConfigDir:
      {
        options_.ConfigDir = value;
      }
      break;
    case option_ConfigFile:
      {
        options_.ConfigFile = value;
      }
      break;
    case option_CopasiDir:
      {
        options_.CopasiDir = value;
      }
      break;
    case option_ExportBerkeleyMadonna:
      {
        options_.ExportBerkeleyMadonna = value;
      }
      break;
    case option_ExportC:
      {
        options_.ExportC = value;
      }
      break;
    case option_ExportSBML:
      {
        options_.ExportSBML = value;
      }
      break;
    case option_Home:
      {
        options_.Home = value;
      }
      break;
    case option_ImportSBML:
      {
        options_.ImportSBML = value;
      }
      break;
    case option_License:
      break;
    case option_NewExportSBML:
      {
        options_.NewExportSBML = value;
      }
      break;
    case option_NoLogo:
      break;
    case option_RegisteredEmail:
      {
        options_.RegisteredEmail = value;
      }
      break;
    case option_RegisteredUser:
      {
        options_.RegisteredUser = value;
      }
      break;
    case option_RegistrationCode:
      {
        options_.RegistrationCode = value;
      }
      break;
    case option_SBMLSchema:
      {
        SBMLSchema_enum evalue;

        if (strcmp(value, "L1V1") == 0)
          {
            evalue = SBMLSchema_L1V1;
          }
        else if (strcmp(value, "L1V2") == 0)
          {
            evalue = SBMLSchema_L1V2;
          }
        else if (strcmp(value, "L2V1") == 0)
          {
            evalue = SBMLSchema_L2V1;
          }
        else if (strcmp(value, "L2V2") == 0)
          {
            evalue = SBMLSchema_L2V2;
          }
        else if (strcmp(value, "L2V3") == 0)
          {
            evalue = SBMLSchema_L2V3;
          }
        else
          {
            std::string error("'"); error += value; error += "' is an invalid value for the 'SBMLSchema' option";
            throw option_error(error);
          }

        options_.SBMLSchema = evalue;
      }
      break;
    case option_Save:
      {
        options_.Save = value;
      }
      break;
    case option_Tmp:
      {
        options_.Tmp = value;
      }
      break;
    case option_Validate:
      break;
    case option_Verbose:
      break;
    }
}
//#########################################################################
const char* copasi::option_error::get_help_comment (void) const
  {
    return const_help_comment;
  }
//#########################################################################
namespace
  {
  const char* expand_long_name (const std::string &name)
  {
    std::string::size_type name_size = name.size();
    std::vector<const char*> matches;

    if (name_size <= 10 && name.compare("SBMLSchema") == 0)
      matches.push_back("SBMLSchema");

    if (name_size <= 9 && name.compare("configdir") == 0)
      matches.push_back("configdir");

    if (name_size <= 10 && name.compare("configfile") == 0)
      matches.push_back("configfile");

    if (name_size <= 9 && name.compare("copasidir") == 0)
      matches.push_back("copasidir");

    if (name_size <= 21 && name.compare("exportBerkeleyMadonna") == 0)
      matches.push_back("exportBerkeleyMadonna");

    if (name_size <= 7 && name.compare("exportC") == 0)
      matches.push_back("exportC");

    if (name_size <= 10 && name.compare("exportSBML") == 0)
      matches.push_back("exportSBML");

    if (name_size <= 4 && name.compare("home") == 0)
      matches.push_back("home");

    if (name_size <= 10 && name.compare("importSBML") == 0)
      matches.push_back("importSBML");

    if (name_size <= 7 && name.compare("license") == 0)
      matches.push_back("license");

    if (name_size <= 13 && name.compare("newExportSBML") == 0)
      matches.push_back("newExportSBML");

    if (name_size <= 6 && name.compare("nologo") == 0)
      matches.push_back("nologo");

    if (name_size <= 5 && name.compare("rCode") == 0)
      matches.push_back("rCode");

    if (name_size <= 6 && name.compare("rEmail") == 0)
      matches.push_back("rEmail");

    if (name_size <= 5 && name.compare("rUser") == 0)
      matches.push_back("rUser");

    if (name_size <= 4 && name.compare("save") == 0)
      matches.push_back("save");

    if (name_size <= 3 && name.compare("tmp") == 0)
      matches.push_back("tmp");

    if (name_size <= 8 && name.compare("validate") == 0)
      matches.push_back("validate");

    if (name_size <= 7 && name.compare("verbose") == 0)
      matches.push_back("verbose");

    if (name_size <= 4 && name.compare("help") == 0)
      matches.push_back("help");

    if (matches.empty())
      {
        std::string error("unknown option '"); error += name; error += "'";
        throw copasi::option_error(error);
      }

    if (matches.size() == 1)
      {
        return matches[0];
      }

    std::string error("the option name '"); error += name; error += "' is ambiguous";
    throw copasi::option_error(error);
  }
} // end anonymous namespace
