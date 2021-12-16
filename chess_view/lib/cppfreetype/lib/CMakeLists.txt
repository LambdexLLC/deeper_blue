#
#   Dependency handler for cppfreetype
#


# For now this will just clone the dependencies if their targets are not already defined

ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/jclib" jclib "https://github.com/JonathanCline/JCLib" dev)
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/freetype" freetype "https://github.com/freetype/freetype")

