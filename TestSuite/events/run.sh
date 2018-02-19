#!/bin/bash
# Copyright (C) 2017 - 2018 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and University of 
# of Connecticut School of Medicine. 
# All rights reserved. 

# Copyright (C) 2012 - 2016 by Pedro Mendes, Virginia Tech Intellectual 
# Properties, Inc., University of Heidelberg, and The University 
# of Manchester. 
# All rights reserved. 

CopasiSE=${1:-../../build/copasi/CopasiSE/CopasiSE}

function runTest () {
  echo Test: ${1}
  rm ${1}.1.txt > /dev/null 3>&1 2>&1 
  ${CopasiSE} --nologo ${1}.cps > /dev/null
  head -n 1 ${1}.1.txt.tgt > ${1}.1.csv
  ndiff -sbml -abserr 1e-5 ${1}.1.txt ${1}.1.txt.tgt >> ${1}.1.csv || echo ${1} failed
}

runTest EventTest1
runTest EventTest2
runTest EventTest3
runTest EventTest4
runTest EventTest5
runTest EventTest6
runTest EventTest7
runTest EventTest8  
runTest EventTest9
runTest EventTest10
runTest EventTest11
runTest EventTest12
runTest EventTest13
runTest EventTest14
runTest EventTest15
runTest EventTest16
runTest EventTest17
runTest EventTest18
runTest EventTest19
runTest EventTest20
runTest EventTest21
runTest EventTest22
runTest EventTest23
runTest EventTest24
runTest EventTest25
runTest EventTest26
runTest EventTest27
runTest EventTest28
runTest EventTest29
runTest EventTest30
