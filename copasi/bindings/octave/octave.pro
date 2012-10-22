# Copyright (C) 2010 - 2012 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

TEMPLATE = lib
CONFIG -= qt

include(../../common.pri)
include(../../app.pri)

contains(BUILD_OS,WIN32){
   TARGET = _COPASI
} else {
   TARGET = COPASI
}


# the code generated by swig has to be compiled with -O1
# since -O2 and higher do things that might break the binary
QMAKE_CFLAGS_RELEASE -= -O3
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O1
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O1

COPASI_LIBS += $${COPASI_LIBS_SE}


INCLUDEPATH += ../../..
contains(BUILD_OS,Linux){

  LIBS = -L../../lib \
         $$join(COPASI_LIBS, " -l", -l) \
         $${LIBS}

  POST_TARGETDEPS += $$join(COPASI_LIBS, ".a  ../../lib/lib", ../../lib/lib, .a)

  QMAKE_POST_LINK += $${QMAKE_MOVE} libCOPASI.so.1.0.0 COPASI.oct $$escape_expand(\n\t)
  QMAKE_POST_LINK += $${QMAKE_DEL_FILE} libCOPASI.so* $$escape_expand(\n\t)
}

contains(BUILD_OS, Darwin) {
  QMAKE_LFLAGS += -Wl,-search_paths_first

  LIBS = $$join(COPASI_LIBS, ".a  ../../lib/lib", ../../lib/lib, .a) \
         $${LIBS}
  
  POST_TARGETDEPS += $$join(COPASI_LIBS, ".a  ../../lib/lib", ../../lib/lib, .a)

  QMAKE_LFLAGS_SHLIB += -unexported_symbols_list unexported_symbols.list
  QMAKE_PRE_LINK = nm -g $${SBML_PATH}/lib/libsbml.a | grep "^[0-9]" | cut -d\" \" -f3  > unexported_symbols.list ; nm -g $${EXPAT_PATH}/lib/libexpat.a | grep "^[0-9]" | cut -d\" \" -f3  >> unexported_symbols.list

}

contains(BUILD_OS, WIN32) { 
  CONFIG += debug_and_release

  debug {
    LIBS += $$join(COPASI_LIBS, ".lib  ../../lib/debug/", ../../lib/debug/, .lib)
  }
  release {
    LIBS += $$join(COPASI_LIBS, ".lib  ../../lib/release/", ../../lib/release/, .lib)
  }

  debug {
    PRE_TARGETDEPS += $$join(COPASI_LIBS, ".lib  ../../lib/debug/", ../../lib/debug/, .lib)
  }

  release {
    PRE_TARGETDEPS += $$join(COPASI_LIBS, ".lib  ../../lib/release/", ../../lib/release/, .lib)
  }

  CONFIG -= staticlib
  CONFIG += dll
  CONFIG += embed_manifest_dll
  LIBS += delayimp.lib
  
}


include(../common/swig_files.pri)


#DISTFILE   = $$SWIG_INTERFACE_FILES
#DISTFILES += local.cpp
#DISTFILES += octave.i
#DISTFILES += $$UNITTEST_FILES

isEmpty(SWIG_PATH){
    # check if the wrapper file is there
    !exists(copasi_wrapper.cpp){
        error(Wrapper file copasi_wrapper.cpp missing. Please reconfigure with --with-swig=PATH_TO_SWIG.)
    }
}

!isEmpty(SWIG_PATH){
    # check if swig is there and create a target to run it to create
    # copasi_wrapper.cpp
    contains(BUILD_OS, WIN32){
        !exists($${SWIG_PATH}\\swig.exe){
        error(Unable to find swig excecutable in $${SWIG_PATH}. Please use --with-swig=PATH to specify the path where PATH/swig.exe is located.) 
         }
    }
    !contains(BUILD_OS, WIN32){
      !exists($${SWIG_PATH}/bin/swig){
        error(Unable to find swig excecutable in $${SWIG_PATH}/bin/. Please use --with-swig=PATH to specify the path where PATH/bin/swig is located.) 
      }
    }

    DEFINE_COMMANDLINE = $$join(DEFINES," -D",-D)
    contains(BUILD_OS, WIN32){
      # since the wrapper file is in a subdirectory, we need to add 
      # the project directory to the include path
      INCLUDEPATH += .

      WRAPPER_FILE_PATH = "."

      debug{
        WRAPPER_FILE_PATH = debug
        wrapper_source.target = "debug\\copasi_wrapper.cpp"
      }	
      release{
        WRAPPER_FILE_PATH = release
        wrapper_source.target = "release\\copasi_wrapper.cpp"
      }

      # we force the rebuild of the wrapper sources
      wrapper_source.depends = FORCE

      wrapper_source.commands = $(DEL_FILE) $${wrapper_source.target} & $${SWIG_PATH}\\swig.exe $${DEFINE_COMMANDLINE} -I..\\.. -c++ -octave -o $${wrapper_source.target} octave.i

      QMAKE_EXTRA_TARGETS += wrapper_source
      debug {
        QMAKE_CLEAN += debug\\copasi_wrapper.cpp 
        QMAKE_CLEAN += debug\\COPASI.oct
      }
      release {
        QMAKE_CLEAN += release\\copasi_wrapper.cpp 
        QMAKE_CLEAN += release\\COPASI.oct
      }
    }
    !contains(BUILD_OS, WIN32){
      wrapper_source.target = copasi_wrapper.cpp
      wrapper_source.depends = $${SWIG_INTERFACE_FILES} octave.i local.cpp
      wrapper_source.commands = $(DEL_FILE) $${wrapper_source.target} ; $${SWIG_PATH}/bin/swig $${DEFINE_COMMANDLINE} -I../.. -c++ -octave -o $${wrapper_source.target} octave.i; sed -e 's/octave_map/Octave_map/' $${wrapper_source.target} > tmp.cpp;mv tmp.cpp $${wrapper_source.target}
  
      QMAKE_EXTRA_TARGETS += wrapper_source
      QMAKE_CLEAN += copasi_wrapper.cpp 
      QMAKE_CLEAN += COPASI.oct
    }
    PRE_TARGETDEPS += $${wrapper_source.target}
}



isEmpty(MKOCTFILE_BIN){
  # we just assume it is in the path
  MKOCTFILE_BIN = $$system(which mkoctfile)
}

isEmpty(MKOCTFILE_BIN) | !exists($${MKOCTFILE_BIN}){
  error("Could not find mkoctfile binary at \"$${MKOCTFILE_BIN}\"."); 
}

OCTAVE_INCLUDES = $$system($${MKOCTFILE_BIN} -p INCFLAGS)
OCTAVE_LIBS = $$system($${MKOCTFILE_BIN} -p OCTAVE_LIBS)
OCTAVE_LIB_DIRS = $$system($${MKOCTFILE_BIN} -p LFLAGS)
QMAKE_CXXFLAGS += $${OCTAVE_INCLUDES}
QMAKE_CXXFLAGS -= -Wall

LIBS += $${OCTAVE_LIB_DIRS}
LIBS += $${OCTAVE_LIBS}


SOURCES += $${wrapper_source.target}
# under windows qmake seems to ignore the last line of project files

