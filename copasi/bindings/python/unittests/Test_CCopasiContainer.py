import COPASI
import unittest
from types import *

class Test_CCopasiContainer(unittest.TestCase):

  def  test_ObjectFromName(self):
    self.assert_(false)


def suite():
  tests=[
          'test_ObjectFromName'
        ]
  return unittest.TestSuite(map(Test_CCopasiContainer,tests))

