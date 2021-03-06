// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#ifndef TEST_000083_H__
#define TEST_000083_H__

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestResult.h>
#include <cppunit/extensions/HelperMacros.h>

/**
 * Test for 3D compartment unit import and export.
 */

class CDataModel;

class test000083 : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(test000083);
  CPPUNIT_TEST(test_import_compartment_units_1);
  CPPUNIT_TEST(test_import_compartment_units_2);
  CPPUNIT_TEST(test_import_compartment_units_3);
  CPPUNIT_TEST(test_import_compartment_units_4);
  CPPUNIT_TEST(test_import_compartment_units_5);
  CPPUNIT_TEST(test_import_compartment_units_6);
  CPPUNIT_TEST(test_import_compartment_units_7);
  CPPUNIT_TEST(test_import_compartment_units_8);
  CPPUNIT_TEST(test_import_compartment_units_9);
  CPPUNIT_TEST(test_import_compartment_units_10);
  CPPUNIT_TEST(test_import_compartment_units_11);
  CPPUNIT_TEST(test_import_compartment_units_12);
  CPPUNIT_TEST(test_import_compartment_units_13);
  CPPUNIT_TEST(test_import_compartment_units_14);
  CPPUNIT_TEST(test_import_compartment_units_15);
  CPPUNIT_TEST(test_import_compartment_units_16);
  CPPUNIT_TEST(test_import_compartment_units_17);
  CPPUNIT_TEST(test_import_compartment_units_18);
  CPPUNIT_TEST(test_import_compartment_units_19);
  CPPUNIT_TEST(test_import_compartment_units_20);
  CPPUNIT_TEST(test_import_compartment_units_21);
  CPPUNIT_TEST_SUITE_END();

protected:
  // models for import test

  // L2V1 model with global volume unit "nl"
  static const char* MODEL_STRING1;
  // L2V1 model with global volume unit "10e-4*l"
  static const char* MODEL_STRING2;
  // L2V1 model with global volume unit "mole"
  static const char* MODEL_STRING3;
  // L2V1 model with global volume unit "nl"
  // and local unit ml
  static const char* MODEL_STRING4;
  // L2V1 model with global volume unit "nl"
  // and local unit mole
  static const char* MODEL_STRING5;
  // L2V1 model with global volume unit "mole"
  // and local unit nl
  static const char* MODEL_STRING6;
  // L2V1 model with no global volume unit
  // and local unit nl
  static const char* MODEL_STRING7;
  // L2V1 model with no global volume unit
  // and local unit volume
  static const char* MODEL_STRING8;
  // L2V1 model with no global volume unit
  // and local unit litre
  static const char* MODEL_STRING9;
  // L2V1 model with no global volume unit
  // and no local volume unit
  static const char* MODEL_STRING10;
  // L2V1 model with no global volume unit
  // and a local volume unit of "area"
  static const char* MODEL_STRING11;
  // L2V1 model with no global volume unit
  // and a local volume unit of "length"
  static const char* MODEL_STRING12;
  // L2V1 model with no global volume unit
  // and a local volume unit of "dimensionless"
  static const char* MODEL_STRING13;
  // L2V1 model with no global volume unit
  // and a local volume unit of "m^2"
  static const char* MODEL_STRING14;
  // L2V1 model with no global volume unit
  // and a local volume unit of "m"
  static const char* MODEL_STRING15;
  // L2V1 model with no global volume unit
  // and a local volume unit of "mole"
  static const char* MODEL_STRING16;
  // L2V1 model with no global volume unit
  // and a local volume unit of "substance"
  static const char* MODEL_STRING17;
  // L2V1 model with two compartments
  // one has no unit set, the other uses volume
  static const char* MODEL_STRING18;
  // L2V1 model with two compartments
  // one has no unit set, the other uses litre
  static const char* MODEL_STRING19;
  // L2V1 model with two compartments
  // one has no unit set, the other uses nl
  static const char* MODEL_STRING20;
  // L2V1 model with two compartments
  // one uses nl, the other has no unit set
  static const char* MODEL_STRING21;

  static CDataModel* pCOPASIDATAMODEL;

public:
  void setUp();

  void tearDown();

  void test_import_compartment_units_1();
  void test_import_compartment_units_2();
  void test_import_compartment_units_3();
  void test_import_compartment_units_4();
  void test_import_compartment_units_5();
  void test_import_compartment_units_6();
  void test_import_compartment_units_7();
  void test_import_compartment_units_8();
  void test_import_compartment_units_9();
  void test_import_compartment_units_10();
  void test_import_compartment_units_11();
  void test_import_compartment_units_12();
  void test_import_compartment_units_13();
  void test_import_compartment_units_14();
  void test_import_compartment_units_15();
  void test_import_compartment_units_16();
  void test_import_compartment_units_17();
  void test_import_compartment_units_18();
  void test_import_compartment_units_19();
  void test_import_compartment_units_20();
  void test_import_compartment_units_21();
};

#endif /* TEST000083_H__ */
