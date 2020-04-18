# CMake generated Testfile for 
# Source directory: C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests
# Build directory: C:/Development/op3d_active/osgearth-3.0.0/msvc/src/tests/osgEarth_tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(osgEarth_tests "osgEarth_tests")
  set_tests_properties(osgEarth_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;18;add_test;C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(osgEarth_tests "osgEarth_tests")
  set_tests_properties(osgEarth_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;18;add_test;C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(osgEarth_tests "osgEarth_tests")
  set_tests_properties(osgEarth_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;18;add_test;C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(osgEarth_tests "osgEarth_tests")
  set_tests_properties(osgEarth_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;18;add_test;C:/Development/op3d_active/osgearth-3.0.0/src/tests/osgEarth_tests/CMakeLists.txt;0;")
else()
  add_test(osgEarth_tests NOT_AVAILABLE)
endif()
