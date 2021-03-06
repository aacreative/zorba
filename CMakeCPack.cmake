# Copyright 2006-2008 The FLWOR Foundation.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Zorba XQuery Processor")
SET(CPACK_PACKAGE_VENDOR "The FLWOR Fundation")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")


SET(CPACK_NSIS_COMPONENT_INSTALL ON)
SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "FALSE")

SET(CPACK_COMPONENT_UNSPECIFIED_DISPLAY_NAME "Zorba")
SET(CPACK_COMPONENT_DOC_DISPLAY_NAME "Docs")
SET(CPACK_COMPONENT_PHP_EXAMPLES_DISPLAY_NAME "PHP Examples")
SET(CPACK_COMPONENT_CXX_EXAMPLES_DISPLAY_NAME "CXX Examples")
SET(CPACK_COMPONENT_PYTHON_EXAMPLES_DISPLAY_NAME "Python Examples")
SET(CPACK_COMPONENT_RUBY_EXAMPLES_DISPLAY_NAME "Ruby Examples")
SET(CPACK_COMPONENT_JAVA_SWIG_DISPLAY_NAME "Java API")
SET(CPACK_COMPONENT_PHP_SWIG_DISPLAY_NAME "PHP API")
SET(CPACK_COMPONENT_PYTHON_SWIG_DISPLAY_NAME "Python API")
SET(CPACK_COMPONENT_RUBY_SWIG_DISPLAY_NAME "Ruby API")
SET(CPACK_COMPONENT_XQJ_SWIG_DISPLAY_NAME "Java XQJ API")
SET(CPACK_COMPONENT_THESAURUS_DISPLAY_NAME "Thesaurus")
SET(CPACK_COMPONENT_CSHARP_SWIG_DISPLAY_NAME "C# API")

SET(CPACK_COMPONENT_UNSPECIFIED_DESCRIPTION "Zorba Required Files.")
SET(CPACK_COMPONENT_DOC_DESCRIPTION "Files with the Description of different features of zorba.")
SET(CPACK_COMPONENT_PHP_EXAMPLES_DESCRIPTION "Examples for the PHP extension.")
SET(CPACK_COMPONENT_PYTHON_EXAMPLES_DESCRIPTION "Examples for the Python extension.")
SET(CPACK_COMPONENT_RUBY_EXAMPLES_DESCRIPTION "Examples for the Ruby extension.")
SET(CPACK_COMPONENT_CXX_EXAMPLES_DESCRIPTION "Examples for CXX.")
SET(CPACK_COMPONENT_JAVA_SWIG_DESCRIPTION "API for Java")
SET(CPACK_COMPONENT_PHP_SWIG_DESCRIPTION "API for PHP")
SET(CPACK_COMPONENT_PYTHON_SWIG_DESCRIPTION "API for Python")
SET(CPACK_COMPONENT_RUBY_SWIG_DESCRIPTION "API for Ruby")
SET(CPACK_COMPONENT_XQJ_SWIG_DESCRIPTION "XQJ API for Java")
SET(CPACK_COMPONENT_THESAURUS_DESCRIPTION "Thesaurus for Zorba")
SET(CPACK_COMPONENT_CSHARP_SWIG_DESCRIPTION "API for C#")

SET(CPACK_COMPONENT_DOC_GROUP "Documents")
SET(CPACK_COMPONENT_PHP_EXAMPLES_GROUP "Documents")
SET(CPACK_COMPONENT_PYTHON_EXAMPLES_GROUP "Documents")
SET(CPACK_COMPONENT_RUBY_EXAMPLES_GROUP "Documents")
SET(CPACK_COMPONENT_CXX_EXAMPLES_GROUP "Documents")

SET(CPACK_COMPONENT_JAVA_SWIG_GROUP "APIs")
SET(CPACK_COMPONENT_PHP_SWIG_GROUP "APIs")
SET(CPACK_COMPONENT_PYTHON_SWIG_GROUP "APIs")
SET(CPACK_COMPONENT_RUBY_SWIG_GROUP "APIs")
SET(CPACK_COMPONENT_XQJ_SWIG_GROUP "APIs")
SET(CPACK_COMPONENT_CSHARP_SWIG_GROUP "APIs")

SET(CPACK_COMPONENT_GROUP_EXTERNAL_MODULES_DISPLAY_NAME "External Modules")
SET(CPACK_COMPONENT_GROUP_EXTERNAL_MODULES_DESCRIPTION "Complete list of external modules.")
SET(CPACK_COMPONENT_GROUP_DOCUMENTS_DESCRIPTION "Complete Documentation of Zorba and Modules.")
SET(CPACK_COMPONENT_GROUP_APIS_DESCRIPTION "APIs to work with zorba.")

SET(CPACK_ALL_INSTALL_TYPES Full Simple Lite)

SET(CPACK_COMPONENT_DOC_INSTALL_TYPES Full Simple)
SET(CPACK_COMPONENT_PHP_EXAMPLES_INSTALL_TYPES Full Simple)
SET(CPACK_COMPONENT_PYTHON_EXAMPLES_INSTALL_TYPES Full Simple)
SET(CPACK_COMPONENT_RUBY_EXAMPLES_INSTALL_TYPES Full Simple)
SET(CPACK_COMPONENT_CXX_EXAMPLES_INSTALL_TYPES Full Simple)
SET(CPACK_COMPONENT_JAVA_SWIG_INSTALL_TYPES Full)
SET(CPACK_COMPONENT_PYTHON_SWIG_INSTALL_TYPES Full)
SET(CPACK_COMPONENT_PHP_SWIG_INSTALL_TYPES Full)
SET(CPACK_COMPONENT_RUBY_SWIG_INSTALL_TYPES Full)
SET(CPACK_COMPONENT_XQJ_SWIG_INSTALL_TYPES Full)
SET(CPACK_COMPONENT_UNSPECIFIED_INSTALL_TYPES Full Simple Lite)
SET(CPACK_COMPONENT_THESAURUS_INSTALL_TYPES Full)
SET(CPACK_COMPONENT_CSHARP_SWIG_INSTALL_TYPES Full)

SET(CPACK_COMPONENT_ZORBA_SCHEMA_TOOLS_MODULE_GROUP "external_modules")
SET(CPACK_COMPONENT_ZORBA_SCHEMA_TOOLS_MODULE_DISPLAY_NAME "schema tools module")
SET(CPACK_COMPONENT_ZORBA_SCHEMA_TOOLS_MODULE_DESCRIPTION "Install the functionalities of the schema tools module.")
SET(CPACK_COMPONENT_ZORBA_SCHEMA_TOOLS_MODULE_INSTALL_TYPES Full)

INCLUDE(${CMAKE_BINARY_DIR}/CMakeCPackModules.cmake)


CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/CMakeCPackOptions.cmake.in"
               "${CMAKE_BINARY_DIR}/CMakeCPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CMakeCPackOptions.cmake")

SET(CPACK_PACKAGE_VERSION_MAJOR  ${ZORBA_MAJOR_NUMBER})
SET(CPACK_PACKAGE_VERSION_MINOR  ${ZORBA_MINOR_NUMBER})
SET(CPACK_PACKAGE_VERSION_PATCH  ${ZORBA_PATCH_NUMBER})

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Zorba XQuery Processor ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "zorba-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

IF(NOT DEFINED CPACK_SYSTEM_NAME)
  SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
ENDIF(NOT DEFINED CPACK_SYSTEM_NAME)

IF(${CPACK_SYSTEM_NAME} MATCHES Windows)
IF(CMAKE_CL_64)
  SET(CPACK_SYSTEM_NAME win64-x64)
ELSE(CMAKE_CL_64)
  SET(CPACK_SYSTEM_NAME win32-${CMAKE_SYSTEM_PROCESSOR})
ENDIF(CMAKE_CL_64)
ENDIF(${CPACK_SYSTEM_NAME} MATCHES Windows)

IF(NOT DEFINED CPACK_PACKAGE_FILE_NAME)
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_SOURCE_PACKAGE_FILE_NAME}-${CPACK_SYSTEM_NAME}")
ENDIF(NOT DEFINED CPACK_PACKAGE_FILE_NAME)
SET(CPACK_PACKAGE_CONTACT "info@flworfound.org")

IF(UNIX)
  SET(CPACK_STRIP_FILES "")
  SET(CPACK_SOURCE_STRIP_FILES "")
  SET(CPACK_PACKAGE_EXECUTABLES "zorba" "Zorba")
ENDIF(UNIX)
IF ( APPLE )
  SET(CPACK_POSTFLIGHT_SCRIPT "${CMAKE_BINARY_DIR}/osx_postflight.sh")
  CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/scripts/osx_postflight.sh.in"
               "${CMAKE_BINARY_DIR}/osx_postflight.sh")
  MESSAGE ( STATUS "script = ${CPACK_POSTFLIGHT_SCRIPT}" )
ENDIF ( APPLE )
INCLUDE(CPack)
INCLUDE(CPack.cmake)
