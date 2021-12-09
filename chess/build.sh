#!

#
#   "build.sh"
#       (same as build.sh --build)
#
#   "build.sh --clean" | "build.sh -c"
#   "build.sh --build" | "build.sh -b"
#

COMMAND=$1

#
#   Handling for command line arguement(s)
#

# Set default command if none provided
if [[ -z "${COMMAND}" ]]; then
    COMMAND="build"
elif [[ "${COMMAND}" == "-b" ]]; then
    COMMAND="build"
elif [[ "${COMMAND}" == "--build" ]]; then
    COMMAND="build"
elif [[ "${COMMAND}" == "-c" ]]; then
    COMMAND="clean"
elif [[ "${COMMAND}" == "--clean" ]]; then
    COMMAND="clean"
else
    echo "unrecognized command ${COMMAND}"
    exit
fi



#
#   Builds a cargo project
#
build_cargo_project() {

    local BUILD_OUTPUT_ROOT=$OUTPUT_ROOT/build

    cargo build
    cargo build --release

    local GENERATOR_BUILD_OUTPUT_ROOT=$BUILD_OUTPUT_ROOT/release

    echo $GENERATOR_BUILD_OUTPUT_ROOT
}

#
#   Copies the build outputs into a common location
#
copy_build_outputs() {
    local INCLUDE_ROOT=include

    local GENERATOR_BUILD_OUTPUT_ROOT=$1
    local OUTPUT_ROOT=$2
    echo "Looking for outputs from ${GENERATOR_BUILD_OUTPUT_ROOT}"

    local LIBRARY_OUTPUT_ROOT=$OUTPUT_ROOT/lib
    local EXECUTABLE_OUTPUT_ROOT=$OUTPUT_ROOT/bin
    local INCLUDE_OUTPUT_ROOT=$OUTPUT_ROOT/include

    mkdir $EXECUTABLE_OUTPUT_ROOT
    mkdir $LIBRARY_OUTPUT_ROOT
    mkdir $INCLUDE_OUTPUT_ROOT

    # Copy build outputs into output directories
    ls $GENERATOR_BUILD_OUTPUT_ROOT | while read OUT_FILENAME; do
        if [[ "$OUT_FILENAME" =~ -exe$ ]]; then
            cp $GENERATOR_BUILD_OUTPUT_ROOT/$OUT_FILENAME $EXECUTABLE_OUTPUT_ROOT
        elif [[ "$OUT_FILENAME" =~ -exe\.exe$ ]]; then
            cp $GENERATOR_BUILD_OUTPUT_ROOT/$OUT_FILENAME $EXECUTABLE_OUTPUT_ROOT
        elif [[ "$OUT_FILENAME" =~ -lib$ ]]; then
            cp $GENERATOR_BUILD_OUTPUT_ROOT/$OUT_FILENAME $LIBRARY_OUTPUT_ROOT
        elif [[ "$OUT_FILENAME" =~ -lib\.lib$ ]]; then
            cp $GENERATOR_BUILD_OUTPUT_ROOT/$OUT_FILENAME $LIBRARY_OUTPUT_ROOT
        fi
    done

    # Check for include folder
    if [[ -e $INCLUDE_ROOT ]]; then
        ls -R $INCLUDE_ROOT | while read INCLUDE_FILE; do
            if [[ "${INCLUDE_FILE}" =~ .h$ ]]; then
                # C header file
                echo "${INCLUDE_FILE}"
            elif [[ "${INCLUDE_FILE}" =~ .hpp$ ]]; then
                # C++ header file      
                echo "${INCLUDE_FILE}"     
            fi
        done
    fi

}


CMAKE_GENERATOR=""
OUTPUT_ROOT=out

BUILD_OUTPUT_ROOT=$OUTPUT_ROOT/build


if [[ "${COMMAND}" == "clean" ]]; then
    
    # Remove output directory/directories
    rm -r out

elif [[ "${COMMAND}" == "build" ]]; then

    # Determine if this is a CMake or Rust project

    if [[ -e "Cargo.toml" ]]; then
        PROJECT_TYPE=cargo
    fi
    if [[ -e "CMakeLists.txt" ]]; then
        if [[ -z "${PROJECT_TYPE}" ]]; then
            PROJECT_TYPE=cmake
        else
            echo "Found a Cargo.toml AND CMakeLists.txt file, this is confusing me."
            exit
        fi
    fi

    # Build the project
    if [[ "${PROJECT_TYPE}" == "cargo" ]]; then
        echo "Building Cargo project"
        GENERATOR_BUILD_OUTPUT_ROOT=$(build_cargo_project)
        copy_build_outputs $GENERATOR_BUILD_OUTPUT_ROOT $OUTPUT_ROOT
    elif [[ "${PROJECT_TYPE}" == "cmake" ]]; then
        
        echo "Building CMake project"

        #
        #   Builds a cmake project
        #

        if [[ -z "${CMAKE_GENERATOR}" ]]; then

            # Determine cmake generator since none was provided
            CMAKE_GENERATOR_LIST=$(cmake -G 2>&1 | grep "=")

            if [[ "${CMAKE_GENERATOR_LIST}" =~ "Visual Studio 16 2019" ]]; then
                # Use visual studio
                CMAKE_GENERATOR="Visual Studio 16 2019"
            elif [[ "${CMAKE_GENERATOR_LIST}" =~ "Ninja" ]]; then
                # Use ninja
                CMAKE_GENERATOR="Ninja"
            else
                echo "Failed to find a supported cmake generator. Please install ninja, visual studio, or other supported cmake generator."
                echo "error"
                exit
            fi
        fi

        # Check that a cmake generator was provided
        echo "Using cmake generator \"${CMAKE_GENERATOR}\""

        # Generate the project files
        cmake -G "${CMAKE_GENERATOR}" -S . -B $BUILD_OUTPUT_ROOT

        #
        #   Handle outputs based on cmake generator
        #

        if [[ "${CMAKE_GENERATOR}" == "Ninja" ]]; then

            # Handle ninja outputs
            GENERATOR_CONFIG=""
            GENERATOR_BUILD_OUTPUT_ROOT=$BUILD_OUTPUT_ROOT

        elif [[ "${CMAKE_GENERATOR}" =~ "Visual Studio" ]]; then

            # Handle visual studio outputs
            GENERATOR_CONFIG="Release"
            GENERATOR_BUILD_OUTPUT_ROOT=$BUILD_OUTPUT_ROOT/Release

        else
            echo "unhandled output from generator ${CMAKE_GENERATOR} (REPORT TO MAINTAINER)"
            echo "error"
            exit
        fi

        # Build generated project files
        if [[ -z "${GENERATOR_CONFIG}" ]]; then
            cmake --build $BUILD_OUTPUT_ROOT
        else
            cmake --build $BUILD_OUTPUT_ROOT --config $GENERATOR_CONFIG
        fi
        
        copy_build_outputs $GENERATOR_BUILD_OUTPUT_ROOT $OUTPUT_ROOT

    else
        # Weird situation afoot
        echo "Unhandled but recognized project type \"${PROJECT_TYPE}\" (REPORT TO MAINTAINER)"
        exit
    fi
    
else
    echo "unhandled but recognized command ${COMMAND} (REPORT TO MAINTAINER)"
    exit
fi
