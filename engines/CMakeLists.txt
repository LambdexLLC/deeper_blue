###############################################################################
###																			###
###		This cmake lists will add any engine projects in this				###
###		folder to the build.												###
###																			###
###		Of course it excludes the template project folder if there is one.	###
###																			###
###############################################################################

# Grab all of the subdirectories here
set(__subdirList )
SUBDIRLIST(__subdirList "${CMAKE_CURRENT_LIST_DIR}")

# Remove the template project folder
list(REMOVE_ITEM __subdirList "template")

# Add the subdirectories with a cmake lists
foreach(subd IN LISTS __subdirList)
	if (EXISTS "${subd}/CMakeLists.txt")
		add_subdirectory("${subd}")
	endif()
endforeach()