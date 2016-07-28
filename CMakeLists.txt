#----------------------------------------------------------------------------
# Setup the project
#
FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(rootAnalysis)

#----------------------------------------------------------------------------
find_package( ROOT REQUIRED ) 

FOREACH( pkg ROOT )
    IF( ${pkg}_FOUND )
        INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
    ENDIF()
ENDFOREACH()

#----------------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(houghDistance houghDistance.cxx ${sources} ${headers})
add_executable(houghEfficiency houghEfficiency.cxx ${sources} ${headers})
add_executable(simuEta simuEta.cxx ${sources} ${headers})
add_executable(electronShowerAnalysis electronShowerAnalysis.cxx ${sources} ${headers})
add_executable(energyProfile energyProfile.cxx ${sources} ${headers})
add_executable(fsrPlots fsrPlots.cxx ${sources} ${headers})
add_executable(mergeRootFiles mergeRootFiles.cxx ${sources} ${headers})
add_executable(muonPlusJetDistance muonPlusJetDistance.cxx ${sources} ${headers})
add_executable(muonPlusJetEfficiency muonPlusJetEfficiency.cxx ${sources} ${headers})
add_executable(muonPlusJetAngle muonPlusJetAngle.cxx ${sources} ${headers})
target_link_libraries(houghDistance)
target_link_libraries(houghEfficiency)
target_link_libraries(simuEta)
target_link_libraries(electronShowerAnalysis)
target_link_libraries(energyProfile)
target_link_libraries(fsrPlots)
target_link_libraries(mergeRootFiles)
target_link_libraries(muonPlusJetDistance)
target_link_libraries(muonPlusJetEfficiency)
target_link_libraries(muonPlusJetAngle)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS houghDistance houghEfficiency simuEta electronShowerAnalysis energyProfile fsrPlots mergeRootFiles muonPlusJetDistance muonPlusJetEfficiency muonPlusJetAngle DESTINATION ${PROJECT_SOURCE_DIR}/bin)
