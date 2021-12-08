# Add the required libraries

#find_package(OpenSSL REQUIRED)

ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/cpp-http" httplib "https://github.com/yhirose/cpp-httplib")
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/jclib" jclib "https://github.com/JonathanCline/JCLib" dev)
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/fmt" fmt "https://github.com/fmtlib/fmt")

# Trickle down!

add_subdirectory("nlohmann")
