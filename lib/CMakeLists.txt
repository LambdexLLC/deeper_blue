# Add the required libraries

#find_package(OpenSSL REQUIRED)

ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/cpp-http" httplib "https://github.com/yhirose/cpp-httplib")



# Trickle down!

add_subdirectory("nlohmann")
