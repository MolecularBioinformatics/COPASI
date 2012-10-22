# Copyright (C) 2010 - 2012 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

# Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
# and The University of Manchester. 
# All rights reserved. 

# Copyright (C) 2006 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc. and EML Research, gGmbH. 
# All rights reserved. 

TEMPLATE = lib
CONFIG -= qt

include(../../common.pri)
include(../../app.pri)

TARGET = CopasiJava

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

  !isEmpty(JAVA_HOME){
   isEmpty(JAVA_INCLUDE_PATH){
     INCLUDEPATH += $${JAVA_HOME}/include/
     INCLUDEPATH += $${JAVA_HOME}/include/linux
   }
  }

  !isEmpty(JAVA_INCLUDE_PATH){
    INCLUDEPATH += $${JAVA_INCLUDE_PATH} 
    INCLUDEPATH += $${JAVA_INCLUDE_PATH}/linux
  }
}


contains(BUILD_OS, Darwin) {
  QMAKE_LFLAGS += -Wl,-search_paths_first
  
  LIBS = $$join(COPASI_LIBS, ".a  ../../lib/lib", ../../lib/lib, .a) \
         $${LIBS}
  
  POST_TARGETDEPS += $$join(COPASI_LIBS, ".a  ../../lib/lib", ../../lib/lib, .a)

    LIBS += -framework JavaVM
    LIBS += -framework QuickTime
    LIBS += -framework Carbon
    LIBS += -framework Accelerate

    QMAKE_LFLAGS_SHLIB += -unexported_symbols_list unexported_symbols.list
    
    # make a hard link from the generated dylib file to a file with the ending
    # jnilib
    QMAKE_PRE_LINK = nm -g $${SBML_PATH}/lib/libsbml.a | grep "^[0-9]" | cut -d\" \" -f3  > unexported_symbols.list ; nm -g $$EXPAT_PATH/lib/libexpat.a | grep "^[0-9]" | cut -d\" \" -f3  >> unexported_symbols.list
    QMAKE_POST_LINK = ln -f libCopasiJava.1.0.0.dylib libCopasiJava.jnilib 

  !isEmpty(JAVA_HOME){
   isEmpty(JAVA_INCLUDE_PATH){
     INCLUDEPATH += $${JAVA_HOME}/include/
   }  
  }

  !isEmpty(JAVA_INCLUDE_PATH){
    INCLUDEPATH += $${JAVA_INCLUDE_PATH}
  }
  

}

contains(BUILD_OS, WIN32) { 
  CONFIG += debug_and_release

  debug{
    LIBS += $$join(COPASI_LIBS, ".lib  ../../lib/debug/", ../../lib/debug/, .lib)
  }  
  release{
    LIBS += $$join(COPASI_LIBS, ".lib  ../../lib/release/", ../../lib/release/, .lib)
  }  

  debug{
    PRE_TARGETDEPS += $$join(COPASI_LIBS, ".lib  ../../lib/debug/", ../../lib/debug/, .lib)
  }

  release{
    PRE_TARGETDEPS += $$join(COPASI_LIBS, ".lib  ../../lib/release/", ../../lib/release/, .lib)
  }

  CONFIG -= staticlib
  CONFIG += dll
  CONFIG += embed_manifest_dll
  LIBS += delayimp.lib



  !isEmpty(JAVA_HOME){
   isEmpty(JAVA_INCLUDE_PATH){
     INCLUDEPATH += $${JAVA_HOME}\\include
     INCLUDEPATH += $${JAVA_HOME}\\include\\win32
   }  
  }

  !isEmpty(JAVA_INCLUDE_PATH){
    INCLUDEPATH += $${JAVA_INCLUDE_PATH}
    INCLUDEPATH += $${JAVA_INCLUDE_PATH}\\win32
  }
  
}

include(../common/swig_files.pri)



#DISTFILE   = $$SWIG_INTERFACE_FILES
#DISTFILES += local.cpp
#DISTFILES += java.i
#DISTFILES += gui/org/COPASI/gui/TaskWidget.java
#DISTFILES += gui/org/COPASI/gui/TrajectoryTaskWidget.java
#DISTFILES += gui/org/COPASI/gui/PositiveIntegerVerifier.java
#DISTFILES += gui/org/COPASI/gui/IntegerVerifier.java
#DISTFILES += gui/org/COPASI/gui/PositiveFloatVerifier.java
#DISTFILES += gui/org/COPASI/gui/FloatVerifier.java
#DISTFILES += unittests/Test_CreateSimpleModel.java 
#DISTFILES += unittests/Test_RunSimulations.java
#DISTFILES += unittests/Test_RunOptimization.java

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

    debug{
      JAVAC_DEBUG_OPTIONS="-g"
    }
    
    release{
      JAVAC_DEBUG_OPTIONS=""
    }

    contains(BUILD_OS, WIN32){
      # since the wrapper file is in a subdirectory, we need to add 
      # the project directory to the include path
      INCLUDEPATH += .

      JAVA_FILE_PATH = "."

      debug{
        JAVA_FILE_PATH = debug
        wrapper_source.target = "debug\\copasi_wrapper.cpp"
      }	
      release{
        JAVA_FILE_PATH = release
        wrapper_source.target = "release\\copasi_wrapper.cpp"
      }

      # we force the rebuild of the wrapper sources
      wrapper_source.depends = FORCE

      # using the -C switch with jar from JDK 1.6 seems to lead to errors, so I am switching paths explicitely
      wrapper_source.commands = $(DEL_FILE) $${wrapper_source.target} & $(MKDIR) $${JAVA_FILE_PATH}\\java_files\\org\\COPASI & $(DEL_FILE) $${JAVA_FILE_PATH}\\java_files\\org\\COPASI\\*.java & $(DEL_FILE) $${JAVA_FILE_PATH}\\java_files\\org\\COPASI\\*.class & $(DEL_FILE) gui\\org\\COPASI\\gui\\*.class & $${SWIG_PATH}\\swig.exe $${DEFINE_COMMANDLINE} -I..\\.. -c++ -java -o $${wrapper_source.target} -package org.COPASI -outdir $${JAVA_FILE_PATH}\\java_files\\org\\COPASI  java.i  & $${JAVA_HOME}\\bin\\javac.exe $${JAVAC_DEBUG_OPTIONS} -classpath .\\$${JAVA_FILE_PATH}\\java_files -d .\\$${JAVA_FILE_PATH}\\java_files .\\$${JAVA_FILE_PATH}\\java_files\\org\\COPASI\\*.java & cd .\\$${JAVA_FILE_PATH}\java_files & $${JAVA_HOME}\\bin\\jar.exe cvf ..\\copasi.jar org\\COPASI\\*.java org\\COPASI\\*.class & cd ..\..\gui & $${JAVA_HOME}\\bin\\javac.exe $${JAVAC_DEBUG_OPTIONS} -classpath .;..\\$${JAVA_FILE_PATH}\\copasi.jar -d . .\\org\\COPASI\\gui\\*.java & $${JAVA_HOME}\\bin\\jar.exe cvf ..\\$${JAVA_FILE_PATH}\\copasi_gui.jar org\\COPASI\\gui\\*.java org\\COPASI\\gui\\*.class & cd .. 
      
      QMAKE_EXTRA_TARGETS += wrapper_source

      debug {
      QMAKE_CLEAN += .\\debug\\copasi_wrapper.cpp
      QMAKE_CLEAN += .\\debug\\copasi.jar 
      QMAKE_CLEAN += .\\debug\\copasi_gui.jar
      QMAKE_CLEAN += .\\debug\\java_files\\org\\COPASI\\*.java
      QMAKE_CLEAN += .\\debug\\java_files\\org\\COPASI\\*.class
      }

      release {
      QMAKE_CLEAN += .\\release\\copasi_wrapper.cpp
      QMAKE_CLEAN += .\\release\\copasi.jar 
      QMAKE_CLEAN += .\\release\\copasi_gui.jar
      QMAKE_CLEAN += .\\release\\java_files\\org\\COPASI\\*.java
      QMAKE_CLEAN += .\\release\\java_files\\org\\COPASI\\*.class
      }
      QMAKE_CLEAN += .\\gui\\org\\COPASI\\gui\\*.class
    }
    !contains(BUILD_OS, WIN32){

      wrapper_source.target = copasi_wrapper.cpp
      wrapper_source.depends = $${SWIG_INTERFACE_FILES} java.i local.cpp
      wrapper_source.commands = $(DEL_FILE) $${wrapper_source.target}; $(DEL_FILE) java_files/org/COPASI/*; $(DEL_FILE) gui/org/COPASI/gui/*.class ; mkdir -p java_files/org/COPASI ; $${SWIG_PATH}/bin/swig $${DEFINE_COMMANDLINE} -I../.. -c++ -java -o $${wrapper_source.target} -package org.COPASI -outdir java_files/org/COPASI/  java.i; cd java_files; $${JAVA_HOME}/bin/javac $${JAVAC_DEBUG_OPTIONS} -classpath . -d . org/COPASI/*.java ;rm -f  copasi.jar;$${JAVA_HOME}/bin/jar cf ../copasi.jar org ; cd .. ; cd  gui; $${JAVA_HOME}/bin/javac $${JAVAC_DEBUG_OPTIONS} -classpath ../copasi.jar:. -d . org/COPASI/gui/*.java ; $${JAVA_HOME}/bin/jar cf ../copasi_gui.jar org/COPASI/gui/*.class org/COPASI/gui/*.java 
      QMAKE_EXTRA_TARGETS += wrapper_source
      
      QMAKE_CLEAN += copasi.jar 
      QMAKE_CLEAN += copasi_gui.jar
      QMAKE_CLEAN += ./java_files/org/COPASI/*.java
      QMAKE_CLEAN += ./java_files/org/COPASI/*.class
      QMAKE_CLEAN += ./gui/org/COPASI/gui/*.class
    }
    PRE_TARGETDEPS += $${wrapper_source.target}
}


SOURCES += $${wrapper_source.target}
# under windows qmake seems to ignore the last line of project files

