// Main
//
// (C) Stefan Hoops 2002
//

#ifdef XXXX
#ifdef WIN32
# include <windows.h>
# include <winbase.h>
# include <direct.h>
# define getcwd _getcwd
# ifdef ERROR
#  undef ERROR
# endif
#else
# include <unistd.h>
#endif
#endif

#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>

#define COPASI_MAIN
#define COPASI_TRACE_CONSTRUCTION

#include "copasi.h"
#include "utilities/CGlobals.h"
#include "utilities/CCopasiMessage.h"
#include "utilities/CCopasiException.h"
#include "commandline/COptionParser.h"
#include "commandline/COptions.h"
#include "function/CFunctionDB.h"
#include "model/CModel.h"
#include "output/COutputList.h"
#include "trajectory/CTrajectoryTask.h"
#include "trajectory/CTrajectoryProblem.h"
#include "steadystate/CSteadyStateTask.h"
#include "steadystate/CSteadyStateProblem.h"

void processTrajectory(CModel & model, CReadConfig & copasiFile);
void processSteadyState(CModel & model, CReadConfig & copasiFile);

int main(int argc, char *argv[])
{
  try
    {
      Copasi = new CGlobals;

      // Parse the commandline options
      COptions::init(argc, argv);

      // Get the name of the SystemFunctionDB
      std::string tmp;
      COptions::getValue("SystemFunctionDB", tmp);
      Copasi->pFunctionDB->setFilename(tmp);

      // Get the name of the CopasiFile
      COptions::getValue("CopasiFile", tmp);
      CReadConfig CopasiFile(tmp);

      // Load the model
      CModel Model;
      Model.load(CopasiFile);

      // Load the output specifications
      Copasi->pOutputList->load(CopasiFile);

      bool runTrajectory;
      CopasiFile.getVariable("Dynamics", "bool", &runTrajectory,
                             CReadConfig::LOOP);
      bool runSteadyState;
      CopasiFile.getVariable("SteadyState", "bool", & runSteadyState);

      if (runTrajectory) processTrajectory(Model, CopasiFile);
      if (runSteadyState) processSteadyState(Model, CopasiFile);
    }

  catch (copasi::autoexcept &e)
    {
      switch (e.get_autothrow_id())
        {
        case copasi::autothrow_help:
          std::cout << "Usage: " << argv[0] << " [options]\n";
          std::cout << e.what();
        }
    }

  catch (copasi::option_error &e)
    {
      std::cerr << argv[0] << ": " << e.what() << "\n";
      std::cerr << e.get_help_comment() << std::endl;
    }

  catch (CCopasiException Exception)
    {
      std::cout << Exception.getMessage().getText() << std::endl;
    }

  delete Copasi;
  std::cout << "Leaving main program." << std::endl;
  return 0;
}

void processTrajectory(CModel & model, CReadConfig & copasiFile)
{
  CTrajectoryTask task;

  task.load(copasiFile);

  task.getProblem()->setModel(&model);

  ofstream TrajectoryFile(Copasi->pOutputList->getTrajectoryFile().c_str());
  task.initializeReporting(TrajectoryFile);

  task.process();
}

void processSteadyState(CModel & model, CReadConfig & copasiFile)
{
  CSteadyStateTask task;

  task.load(copasiFile);

  task.getProblem()->setModel(&model);

  ofstream SteadyStateFile(Copasi->pOutputList->getSteadyStateFile().c_str());
  task.initializeReporting(SteadyStateFile);

  task.process();
}
