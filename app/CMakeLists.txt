# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/native-lib.cpp
             src/main/cpp/test.cpp
             src/main/cpp/transcode_aac.cpp
             src/main/cpp/transcode_aac.h
             src/main/cpp/BuffQueue.h
             src/main/cpp/BuffQueue.cpp
             src/main/cpp/Encoder.h
             src/main/cpp/Encoder.cpp
             src/main/cpp/ErrorCheck.h
             src/main/cpp/FFAudioBufferEncoder.cpp
             src/main/cpp/FFAudioBufferEncoder.hpp
             src/main/cpp/FFAudioHelper.cpp
             src/main/cpp/FFAudioHelper.hpp
             src/main/cpp/FFAudioMixing.cpp
             src/main/cpp/FFAudioMixing.hpp
             src/main/cpp/FFAutoReleasePool.cpp
             src/main/cpp/FFAutoReleasePool.hpp
             src/main/cpp/JNI_FFAudioMixing.cpp
             src/main/cpp/JNI_AAC_Encoder.cpp
           )
include_directories (
    src/main/cpp/libffmpeg/include
)
link_directories(
    src/main/jniLibs/armeabi
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
              log
              z
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavcodec.so
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavformat.so
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavutil.so
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libswresample.so
                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/armeabi/libavfilter.so
                        )
