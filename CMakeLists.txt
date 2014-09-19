CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(DTIAtlasRegionAnalyzer) # name of the project

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE}) # creates ITK_DIR
else(ITK_FOUND)
  message(FATAL_ERROR "ITK not found. Please set ITK_DIR")
endif(ITK_FOUND)

find_package(GenerateCLP REQUIRED)
if(GenerateCLP_FOUND)
  include(${GenerateCLP_USE_FILE}) # creates GenerateCLP_DIR ModuleDescriptionParser_DIR TCLAP_DIR
else(GenerateCLP_FOUND)
  message(FATAL_ERROR "GenerateCLP not found. Please set GenerateCLP_DIR")
endif(GenerateCLP_FOUND)

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_USE_FILE)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${QT_INCLUDE_DIR})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT)
ELSE(QT_USE_FILE)
MESSAGE(FATAL_ERROR, "QT not found. Pease set QT_DIR.")
ENDIF(QT_USE_FILE)

QT4_WRAP_CPP(QtProject_HEADERS_MOC DTIAtlasRegionAnalyzer.h)
QT4_WRAP_UI(UI_FILES GUI.ui)

SET(sources main.cxx DTIAtlasRegionAnalyzer.cxx DTIAtlasRegionAnalyzer.h Scriptwriter.cxx Scriptwriter.h ${QtProject_HEADERS_MOC} ${UI_FILES})

ADD_EXECUTABLE(DTIAtlasRegionAnalyzer ${sources}) # add the files contained by "sources" to the project

TARGET_LINK_LIBRARIES(DTIAtlasRegionAnalyzer ${QT_LIBRARIES} ${ITK_LIBRARIES})

