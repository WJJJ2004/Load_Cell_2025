cmake_minimum_required(VERSION 3.5)
project(load_cell_2025)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find dependencie
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Boost 1.71.0 REQUIRED COMPONENTS thread)
find_package(Qt5 COMPONENTS Widgets Core Gui Network REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core SerialPort)
find_package(std_msgs REQUIRED)
find_package(humanoid_interfaces REQUIRED)

file(GLOB_RECURSE THIRD_PARTY_CPP third_party/qcustomplot/*.cpp)
file(GLOB_RECURSE THIRD_PARTY_HPP third_party/qcustomplot/*.h)
file(GLOB PRO_SOURCE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  src/*.cpp)
file(GLOB PRO_UI_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  ui/*.ui)
file(GLOB PRO_INCLUDE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  include/${PROJECT_NAME}/*.h include/${PROJECT_NAME}/*.hpp)
file(GLOB PRO_RESOURCE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  resources/*.qrc)

qt5_wrap_ui(QT_UI_HPP ${PRO_UI_DIR})
qt5_wrap_cpp(QT_MOC_HPP ${PRO_INCLUDE_DIR})
qt5_add_resources(QT_RESOUCE_HPP ${PRO_RESOURCE_DIR})

include_directories(
  include/load_cell_2025
  third_party/qcustomplot
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Qt5SerialPort_INCLUDE_DIRS}
  ${humanoid_interfaces_INCLUDE_DIRS}
)
add_executable(${PROJECT_NAME}
    ${PRO_SOURCE_DIR}
    ${PRO_INCLUDE_DIR}
    ${PRO_UI_DIR}
    ${QT_MOC_HPP}
    ${QT_RESOUCE_HPP}
    ${QT_UI_HPP}
    ${THIRD_PARTY_CPP}
    ${THIRD_PARTY_HPP}
)
target_sources(${PROJECT_NAME} PRIVATE ${THIRD_PARTY_HPP})
ament_target_dependencies(${PROJECT_NAME} rclcpp  Boost std_msgs humanoid_interfaces)
target_link_libraries(
  ${PROJECT_NAME}
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
  Qt5::Network
  Qt5::SerialPort
)

install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
