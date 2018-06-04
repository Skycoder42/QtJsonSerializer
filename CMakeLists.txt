cmake_minimum_required (VERSION 3.1)

project (QtJsonSerializer)

include_directories (src)
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (src/jsonserializer)
include_directories (src/jsonserializer/typeconverters)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})


set (HEADERS
src/jsonserializer/qjsonexceptioncontext_p.h
src/jsonserializer/qjsonserializer.h
src/jsonserializer/qjsonserializerexception.h
src/jsonserializer/qjsonserializerexception_p.h
src/jsonserializer/qjsonserializer_p.h
src/jsonserializer/qjsontypeconverter.h
src/jsonserializer/qtjsonserializer_global.h
src/jsonserializer/typeconverters/qjsonbytearrayconverter_p.h
src/jsonserializer/typeconverters/qjsongadgetconverter_p.h
src/jsonserializer/typeconverters/qjsongeomconverter_p.h
src/jsonserializer/typeconverters/qjsonjsonconverter_p.h
src/jsonserializer/typeconverters/qjsonlistconverter_p.h
src/jsonserializer/typeconverters/qjsonlocaleconverter_p.h
src/jsonserializer/typeconverters/qjsonmapconverter_p.h
src/jsonserializer/typeconverters/qjsonobjectconverter_p.h
src/jsonserializer/typeconverters/qjsonpairconverter_p.h
src/jsonserializer/typeconverters/qjsonregularexpressionconverter_p.h
src/jsonserializer/typeconverters/qjsonversionnumberconverter_p.h
)

if ((NOT MSVC) OR (MSVC_VERSION GREATER 1800))
set (HEADERS ${HEADERS}
src/jsonserializer/qjsonserializer_helpertypes.h
)
endif()

set (SOURCES
src/jsonserializer/qjsonexceptioncontext.cpp
src/jsonserializer/qjsonserializer.cpp
src/jsonserializer/qjsonserializerexception.cpp
src/jsonserializer/qjsontypeconverter.cpp
src/jsonserializer/qtjsonserializer_global.cpp
src/jsonserializer/typeconverters/qjsonbytearrayconverter.cpp
src/jsonserializer/typeconverters/qjsongadgetconverter.cpp
src/jsonserializer/typeconverters/qjsongeomconverter.cpp
src/jsonserializer/typeconverters/qjsonjsonconverter.cpp
src/jsonserializer/typeconverters/qjsonlistconverter.cpp
src/jsonserializer/typeconverters/qjsonlocaleconverter.cpp
src/jsonserializer/typeconverters/qjsonmapconverter.cpp
src/jsonserializer/typeconverters/qjsonobjectconverter.cpp
src/jsonserializer/typeconverters/qjsonpairconverter.cpp
src/jsonserializer/typeconverters/qjsonregularexpressionconverter.cpp
src/jsonserializer/typeconverters/qjsonversionnumberconverter.cpp
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic automatically when needed.
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Core)


option(STATIC_LIB "Build static lib instead of dynamic" OFF)


if (MSVC)
	add_definitions (/W3)
elseif (CMAKE_COMPILER_IS_GNUCXX)
        add_definitions (-Wall -pedantic -shared -fPIC)
else ()
	message ("Unknown compiler")
endif ()

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})


set(HEADERS_SOURCES ${HEADERS} ${SOURCES})

add_definitions(-DNO_QT_MODULE)

if (STATIC_LIB)
    add_definitions(-DSTATIC_LIB)
    add_library (${PROJECT_NAME} STATIC ${HEADERS_SOURCES})
else()
    add_definitions(-DQT_BUILD_JSONSERIALIZER_LIB)
    add_library (${PROJECT_NAME} SHARED ${HEADERS_SOURCES})
endif()


target_link_libraries (${PROJECT_NAME}  Qt5::Core)


if(NOT MSVC)
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++11 ")
        message ("Switched on c++11 support")
elseif(COMPILER_SUPPORTS_CXX0X)
        set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++0x")
        message ("Switched on c++0x support")
else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
endif(NOT MSVC)

install(TARGETS ${PROJECT_NAME} 
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
