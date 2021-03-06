if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
elseif(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
elseif(${CMAKE_C_COMPILER} MATCHES "icc.*$")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -xHost")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
elseif(${CMAKE_CXX_COMPILER} MATCHES "icpc.*$")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xHost")
endif()
