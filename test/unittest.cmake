cmake_minimum_required(VERSION 2.8)

include(CMakeParseArguments)

enable_testing()

set(CLANG_VERSION "6.0")

# creates test with coverage and valgrind targets
# adds the test to the global lists: UNITTEST_TARGETS, 
# UNITTEST_VALGRIND_TARGETS, UNITTEST_COVERAGE_TARGETS
function(make_test)
    cmake_parse_arguments(PARSED_ARGS "VALGRIND;COVERAGE" "NAME" "SRCS;DEPS;LIBS" ${ARGN})

    if(NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR "You must provide a name for the test!")
    endif(NOT PARSED_ARGS_NAME)

    add_executable(${PARSED_ARGS_NAME} ${PARSED_ARGS_SRCS})
    target_link_libraries (${PARSED_ARGS_NAME} LINK_PUBLIC ${PARSED_ARGS_LIBS})

    # add run_<test_name> target
    add_custom_target(
            "run_${PARSED_ARGS_NAME}" 
            COMMAND ./${PARSED_ARGS_NAME}
            DEPENDS ${PARSED_ARGS_NAME}    
    )

    # add to global list of unittest targets
    set(UNITTEST_TARGETS "${UNITTEST_TARGETS};run_${PARSED_ARGS_NAME}" CACHE INTERNAL "Unittest targets")

    # if valgrind is required, add run_valgrind_<test_name> target
    if(${PARSED_ARGS_VALGRIND})

        message("Creating valgrind target for '${PARSED_ARGS_NAME}'")

        # add to global list of valgrind targets
        set(UNITTEST_VALGRIND_TARGETS "${UNITTEST_VALGRIND_TARGETS};run_valgrind_${PARSED_ARGS_NAME}" CACHE INTERNAL "Unittest valgrind targets")

        # run valgrind on test and generate xml output
        add_custom_target(
            "run_valgrind_${PARSED_ARGS_NAME}" 
            COMMAND valgrind --xml=yes --xml-file=${PARSED_ARGS_NAME}.memcheck ./${PARSED_ARGS_NAME}
            DEPENDS ${PARSED_ARGS_NAME}    
        )

    endif(${PARSED_ARGS_VALGRIND})

    # if coverage is required
    if(${PARSED_ARGS_COVERAGE})

        message("Creating coverage targets for '${PARSED_ARGS_NAME}'")

        # add to global list of coverage targets
        set(UNITTEST_COVERAGE_TARGETS "${UNITTEST_COVERAGE_TARGETS};run_ccov_${PARSED_ARGS_NAME}" CACHE INTERNAL "Unittest coverage targets")

        add_custom_target("ccov_preprocessing_${PARSED_ARGS_NAME}"
            COMMAND LLVM_PROFILE_FILE=${PARSED_ARGS_NAME}.profraw $<TARGET_FILE:${PARSED_ARGS_NAME}>
            COMMAND llvm-profdata-${CLANG_VERSION} merge -sparse ${PARSED_ARGS_NAME}.profraw -o ${PARSED_ARGS_NAME}.profdata
            DEPENDS ${PARSED_ARGS_NAME})

        # add_custom_target(${PARSED_ARGS_NAME}-ccov-show
        #     COMMAND llvm-cov-${CLANG_VERSION} show $<TARGET_FILE:${PARSED_ARGS_NAME}> -instr-profile=${PARSED_ARGS_NAME}.profdata -show-line-counts-or-regions
        #     DEPENDS ${PARSED_ARGS_NAME}-ccov-preprocessing)

        # add_custom_target(${PARSED_ARGS_NAME}-ccov-report
        #     COMMAND llvm-cov-${CLANG_VERSION} report $<TARGET_FILE:${PARSED_ARGS_NAME}> -instr-profile=${PARSED_ARGS_NAME}.profdata
        #     DEPENDS ${PARSED_ARGS_NAME}-ccov-preprocessing)

        add_custom_target("run_ccov_${PARSED_ARGS_NAME}"
            COMMAND llvm-cov-${CLANG_VERSION} show $<TARGET_FILE:${PARSED_ARGS_NAME}> -instr-profile=${PARSED_ARGS_NAME}.profdata -show-line-counts-or-regions -output-dir=${PARSED_ARGS_NAME}-llvm-cov -format="html"
            DEPENDS "ccov_preprocessing_${PARSED_ARGS_NAME}")

        add_custom_command(TARGET "run_ccov_${PARSED_ARGS_NAME}" POST_BUILD
            COMMAND ;
            COMMENT "Open ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_ARGS_NAME}-llvm-cov/index.html in your browser to view the coverage report.")

    endif(${PARSED_ARGS_COVERAGE})

endfunction()

