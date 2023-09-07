
cmake_minimum_required(VERSION 3.16)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(NOT DEFINED ACRT_SDK_ROOT)
    set(ACRT_SDK_ROOT "/opt/autocore")
endif()
set(ACRT_DEP_ROOT "${ACRT_SDK_ROOT}/../autocore-deps")

execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE HOST_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(ACRT_SDK_LIBDIR ${ACRT_SDK_ROOT}/lib)
set(ACRT_DEP_LIBDIR ${ACRT_DEP_ROOT}/lib)
if (DEFINED BUILD_ARCH AND DEFINED HOST_ARCH AND (NOT ${BUILD_ARCH} STREQUAL ${HOST_ARCH}))
    if (${BUILD_ARCH} STREQUAL "aarch64")
        set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc-9)
        set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++-9)
        set(CMAKE_LD aarch64-linux-gnu-ld)
        set(CMAKE_AR aarch64-linux-gnu-ar)
        set(CMAKE_AS aarch64-linux-gnu-as)
        set(CMAKE_NM aarch64-linux-gnu-nm)
        set(CMAKE_STRIP aarch64-linux-gnu-strip)

        set(ACRT_SDK_LIBDIR ${ACRT_SDK_ROOT}/libaarch64)
        set(ACRT_DEP_LIBDIR ${ACRT_DEP_ROOT}/libaarch64)
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${ACRT_SDK_LIBDIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${ACRT_SDK_LIBDIR}/cmake)

set(SAMPLE_NAME "SampleEvent")
project(${SAMPLE_NAME})

find_package(AutoCoreDDS REQUIRED)
if (AGENT_DDS)
    set(_libdds)
else()
    set(_libdds "AutoCoreDDS::ddsc")
endif()

include("cmcg/cmcg")
cmcg_generate(
    TARGET ${SAMPLE_NAME}_cmcg
    GEN_IDL
    FILES ${SAMPLE_NAME}.json
)

set(CONSUMER_EXEC ${SAMPLE_NAME}Consumer)
add_executable(${CONSUMER_EXEC})
target_sources(${CONSUMER_EXEC}
    PRIVATE SampleEventConsumer.cc
    PRIVATE joystick.cc
    PRIVATE Pangolin_controller.cpp 
)

target_include_directories(${CONSUMER_EXEC}
    PRIVATE ${ACRT_SDK_ROOT}/include
    PRIVATE include
    
)

target_link_directories(${CONSUMER_EXEC}
    PRIVATE ${ACRT_SDK_LIBDIR}
    PRIVATE ${ACRT_DEP_LIBDIR}
)

target_link_libraries(${CONSUMER_EXEC}
    PUBLIC wiringPi
    PRIVATE ${SAMPLE_NAME}_cmcg
    PRIVATE acfwcm
    PRIVATE ${_libdds}
)


# target_link_libraries(SampleEventProvider pthread)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(PROVIDER_EXEC ${SAMPLE_NAME}Provider)
add_executable(${PROVIDER_EXEC})
target_sources(${PROVIDER_EXEC}
    PRIVATE SampleEventProvider.cc
    PRIVATE joystick.cc
    PRIVATE Pangolin_controller.cpp 
)

target_include_directories(${PROVIDER_EXEC}
    PRIVATE ${ACRT_SDK_ROOT}/include
    PRIVATE include
)

target_link_directories(${PROVIDER_EXEC}
    PRIVATE ${ACRT_SDK_LIBDIR}
    PRIVATE ${ACRT_DEP_LIBDIR}
)

target_link_libraries(${PROVIDER_EXEC}
    PUBLIC wiringPi
    PRIVATE ${SAMPLE_NAME}_cmcg
    PRIVATE acfwcm
    PRIVATE ${_libdds}
)
