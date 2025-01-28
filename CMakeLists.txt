cmake_minimum_required(VERSION 3.5)

project(
  calculateFLow
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

file(GLOB PROJECT_SOURCES *.h *.cpp *.ui *.json)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(calculateFLow MANUAL_FINALIZATION ${PROJECT_SOURCES})
  # Define target properties for Android with Qt 6 as: set_property(TARGET
  # calculateFLow APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
  # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  if(ANDROID)
    add_library(calculateFLow SHARED ${PROJECT_SOURCES})
    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(calculateFLow ${PROJECT_SOURCES})
  endif()
endif()

target_link_libraries(calculateFLow PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(
  calculateFLow
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

install(
  TARGETS calculateFLow
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(calculateFLow)
endif()

if(WIN32_)
  target_link_libraries(${PROJECT_NAME} PRIVATE setupapi)
  # windeployqt
  set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
  get_target_property(_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake
                      IMPORTED_LOCATION)
  get_filename_component(_QT_BIN_DIR "${_QMAKE_EXECUTABLE}" DIRECTORY)
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_QT_BIN_DIR}")
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND
      "${CMAKE_COMMAND}" -E env PATH="${_QT_BIN_DIR}"
      "${WINDEPLOYQT_EXECUTABLE}" --verbose 1 --no-compiler-runtime
      \"$<TARGET_FILE:${PROJECT_NAME}>\"
    COMMENT "Deploying Qt...")
elseif(UNIX) # LINUX

endif()
