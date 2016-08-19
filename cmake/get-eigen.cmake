function(get_eigen GET_EIGEN_SOURCE_PATH)
    include(ExternalProject)
    ExternalProject_Add(
        External_Eigen
        SOURCE_DIR ${GET_EIGEN_SOURCE_PATH}
        URL https://bitbucket.org/eigen/eigen/get/3.2.9.tar.gz
        URL_HASH SHA512=bf669ec777ce4756ad6033e21bcf5d490cd3488159913fc24a08019fc59aed3e2b55a5732cfd66d88d72eedf939b26f71366d5ade1e9c6ed6bf765da5639c3fa
        CONFIGURE_COMMAND ""
        CMAKE_COMMAND     ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
    )
endfunction()
