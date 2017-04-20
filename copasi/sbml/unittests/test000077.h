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

#ifndef TEST_000077_H__
#define TEST_000077_H__

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestResult.h>
#include <cppunit/extensions/HelperMacros.h>

// Bug 1129
// Test import and export of dimensionless units

class CDataModel;

class test000077 : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(test000077);
  // correct dimensionless model
  CPPUNIT_TEST(test_import_dimensionless_1);

  // correct dimensionless model, but with multipliers and scales
  CPPUNIT_TEST(test_import_dimensionless_2);

  // incorrect volume unit
  CPPUNIT_TEST(test_import_dimensionless_3);

  // incorrect time unit
  CPPUNIT_TEST(test_import_dimensionless_4);

  // incorrect quantity unit
  CPPUNIT_TEST(test_import_dimensionless_5);

  // test for export of dimensionless
  CPPUNIT_TEST(test_export_dimensionless);
  CPPUNIT_TEST_SUITE_END();

protected:
  static CDataModel* pCOPASIDATAMODEL;
  static const char* MODEL_STRING1;
  static const char* MODEL_STRING2;
  static const char* MODEL_STRING3;
  static const char* MODEL_STRING4;
  static const char* MODEL_STRING5;
  static const char* MODEL_STRING6;

public:
  void setUp();

  void tearDown();

  void test_import_dimensionless_1();
  void test_import_dimensionless_2();
  void test_import_dimensionless_3();
  void test_import_dimensionless_4();
  void test_import_dimensionless_5();
  void test_export_dimensionless();
};

#endif /* TEST000077_H__ */
