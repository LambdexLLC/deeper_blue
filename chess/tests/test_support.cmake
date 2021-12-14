
#
#	Defines a new test executable target for use with CTest
#
#	@param testName Target name for the new test
#   @param testSource C++ source file path, must be absolute
#
function(LBX_CHESS_ADD_TEST_FN testName testSource)

	# Test name
	set(tname lambdex_chess-test-${testName}-exe)

	# Define the target
	add_executable(${tname} ${testSource})
	
	# Link the chess library
	target_link_libraries(${tname} PRIVATE lbx::chess-lib)
	
	# Link the jclib test support header
	target_link_libraries(${tname} PRIVATE jclib::test)
	
	# Set C++ standard
	target_compile_features(${tname} PUBLIC cxx_std_20)

	# Tell CTest that we made a present for it
	add_test("${tname}" ${tname})
endfunction()

#
#	Defines a new test executable target for use with CTest
#
#	@param testName Target name for the new test
#   @param testSource C++ source file path, must be absolute
#
macro(LBX_CHESS_ADD_TEST testName testSource)
	LBX_CHESS_ADD_TEST_FN(${testName} ${testSource})
endmacro()