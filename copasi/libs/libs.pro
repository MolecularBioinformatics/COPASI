######################################################################
# Automatically generated by qmake (1.07a) Fri Oct 27 13:31:24 2006
######################################################################

TEMPLATE = subdirs

include(../common.pri)

# Directories
SUBDIRS += COPASISE
!contains(BUILD_GUI, no) {
  SUBDIRS += COPASIUI
}

DISTDIRS = $${SUBDIRS}

DISTFILES += \
        libs.pro

src_distribution.commands = \
  rm -rf ../../copasi_src/copasi/libs; \
  $(CHK_DIR_EXISTS) ../../copasi_src || $(MKDIR) ../../copasi_src; \
  $(CHK_DIR_EXISTS) ../../copasi_src/copasi || $(MKDIR) ../../copasi_src/copasi; \
  $(CHK_DIR_EXISTS) ../../copasi_src/copasi/libs || $(MKDIR) ../../copasi_src/copasi/libs; \
  cp $$DISTFILES ../../copasi_src/copasi/libs; \
  $$join(DISTDIRS, "; $(MAKE) -f $(MAKEFILE) $@; cd ..; cd ", "cd ", "; $(MAKE) -f $(MAKEFILE) $@; cd ..;")

QMAKE_EXTRA_UNIX_TARGETS += src_distribution
