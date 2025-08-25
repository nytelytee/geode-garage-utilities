cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GarageUtilities VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED
  src/constants/other.cpp
  src/constants/authors.cpp
  src/constants/categories.cpp

  src/hooks/GameManager.cpp
  src/hooks/GJGarageLayer.cpp
  src/hooks/ItemInfoPopup.cpp
  src/hooks/PurchaseItemPopup.cpp

  src/popups/OptionsPopup.cpp
  src/popups/SortPopup.cpp
  src/popups/FilterPopup.cpp
  src/popups/AuthorFilterPopup.cpp
  src/popups/CategoryFilterPopup.cpp
  src/popups/FilterAndSortPopup.cpp

  src/utils/SortIconButtonSprite.cpp
  src/utils/BoundCCMenu.cpp
  src/utils/LinkedCCMenu.cpp
  src/utils/CCMenuItemTogglerSpoof.cpp

  src/iconkit.cpp
  src/logic.cpp
  src/main.cpp
  src/debug.cpp
)
include_directories(${PROJECT_NAME} PUBLIC include)


if (NOT DEFINED ENV{GEODE_SDK})
  message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
  message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

if (MSVC)
  target_compile_options(
    ${PROJECT_NAME} PUBLIC
    "/Wall"
    "/wd4820"  # padding inserted
    "/wd4710"  # function not inlined
    #"/wd4265"  # non-virtual destructor
    "/wd5045"  # disable spectre mitigation warning
    # implicitly marked as deleted
    "/wd4625"
    "/wd5026"
    "/wd4626"
    "/wd5027"
    "/wd4464"  # relative include path, in geode
  )
else()
  target_compile_options(${PROJECT_NAME} PUBLIC "-Wall" "-Wextra" "-Wpedantic" "-Wno-dollar-in-identifier-extension")
endif()

set_property(TARGET GarageUtilities PROPERTY COMPILE_WARNING_AS_ERROR ON)

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
