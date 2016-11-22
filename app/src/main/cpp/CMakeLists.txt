# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -Wall")

add_definitions(-DANDROID)
add_subdirectory("tinyxml2/")

if (${ANDROID_PLATFORM_LEVEL} LESS 11)
  message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 (currently using ${ANDROID_PLATFORM_LEVEL}).")
  return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
 # add_definitions("-DDYNAMIC_ES3")
  set(OPENGL_LIB GLESv2)
else ()
 #  add_definitions("-DDYNAMIC_ES3")
  set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 11)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             stitch-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             stitcher/ColorCorrector.cpp
             stitcher/MapPointsBuilder.cpp
             stitcher/MaskMaker.cpp
             stitcher/TexMapBuilder.cpp
             stitcher/VertexBuilder.cpp
             stitcher/ImageStitcher.cpp
             stitcher/GLStitcher.cpp
             stitcher/ParamReader.cpp
             stitcher/MemTransfer.cpp
             stitcher/MemTransferAndroid.cpp
             stitcher/MemTransferFactory.cpp
             stitcher/GraphicBuffer.cpp
             stitcher/DynamicLibrary.cpp
             stitcher/FormatConverter.cpp
             stitcher/PackedCopyConverter.cpp
             stitcher/FormatConverterFactory.cpp
             platform/android/egl.cpp
             esutil/Source/esShader.c
             esutil/Source/esUtil.c
             esutil/Source/Android/esUtil_Android.c
             stitcher/StitchJNIWrapper.cpp

            )
target_include_directories(stitch-lib PUBLIC "esutil/Include"
                "tinyxml2/"
            )


# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       stitch-lib

                       # Links the target library to the log library
                       # included in the NDK.
                         tinyxml2
                         ${OPENGL_LIB}
                         android
                         EGL
                         m
                       ${log-lib} )
