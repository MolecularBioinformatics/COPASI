######################################################################
# $Revision: 1.1 $ $Author: shoops $ $Date: 2003/06/03 20:55:00 $  
######################################################################

include(../common.pri)

TEMPLATE = app
CONFIG -= qt 

DEPENDPATH += .. 
INCLUDEPATH += ..

contains(BUILD_OS, WIN32) {
  LIBS += ../lib/commandline.lib \
          ../lib/copasiXML.lib \
          ../lib/elementaryFluxModes.lib \
          ../lib/function.lib \
          ../lib/mathmodel.lib \
          ../lib/model.lib \
          ../lib/optimization.lib \
          ../lib/output.lib \
          ../lib/randomGenerator.lib \
          ../lib/report.lib \
          ../lib/scan.lib \
          ../lib/steadystate.lib \
          ../lib/trajectory.lib \
          ../lib/utilities.lib
} else {
  # The order of libs is important 
  tmpLIBS = $${LIBS}
  LIBS = -L../lib \
         -Wl,-lcommandline \
         -Wl,-lelementaryFluxModes \
         -Wl,-lcopasiXML \
         -Wl,-lmathmodel \
         -Wl,-lmodel \
         -Wl,-lfunction \
         -Wl,-loutput \
         -Wl,-lreport \
         -Wl,-loptimization \
         -Wl,-lscan \
         -Wl,-lsteadystate \
         -Wl,-ltrajectory \
         -Wl,-lrandomGenerator \
         -Wl,-lutilities \
         -Wl,-loutput \
         -Wl,-lfunction \
         -Wl,-lmodel
  LIBS += $${tmpLIBS}

  TARGETDEPS += ../lib/libcommandline.a \
                ../lib/libcopasiXML.a \
                ../lib/libelementaryFluxModes.a \
                ../lib/libfunction.a \
                ../lib/libmathmodel.a \
                ../lib/libmodel.a \
                ../lib/liboptimization.a \
                ../lib/liboutput.a \
                ../lib/librandomGenerator.a \
                ../lib/libreport.a \
                ../lib/libscan.a \
                ../lib/libsteadystate.a \
                ../lib/libtrajectory.a \
                ../lib/libutilities.a

}

# Input
HEADERS += 

SOURCES += CopasiSE.cpp
