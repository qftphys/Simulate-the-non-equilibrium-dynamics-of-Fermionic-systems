set(CMAKE_C_COMPILER /net/sw/gcc/5.3.0/rtf/bin/gcc)
set(CMAKE_CXX_COMPILER /net/sw/gcc/5.3.0/rtf/bin/g++)
set(Boost_NO_BOOST_CMAKE ON)
set(DBOOST_ROOT /data/fkoehler/local/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIEOMPP_DISABLE_ARROWS")
set(GET_EIGEN_USE_HG ON)
