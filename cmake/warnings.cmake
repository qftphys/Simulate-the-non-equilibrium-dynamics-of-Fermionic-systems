# C compiler warnings
if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
elseif(${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    if(CLANG_EXTRA_VERBOSE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything -Wno-c++98-compat -Wno-missing-prototypes -Wno-padded")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
    endif()
elseif(${CMAKE_C_COMPILER_ID} STREQUAL "Intel")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
elseif(${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
endif()

# C++ compiler warnings
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if(CLANG_EXTRA_VERBOSE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-missing-prototypes -Wno-padded")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    endif()
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
endif()
