if(NOT DEFINED CLI_EXECUTABLE
   OR NOT DEFINED WORKING_DIRECTORY
   OR NOT DEFINED EXPECTED_OUTPUT
   OR NOT DEFINED EXPECTED_EXIT_CODE)
    message(FATAL_ERROR "Missing test parameters")
endif()

execute_process(
    COMMAND "${CLI_EXECUTABLE}" --list
    WORKING_DIRECTORY "${WORKING_DIRECTORY}"
    RESULT_VARIABLE cli_result
    OUTPUT_VARIABLE cli_stdout
    ERROR_VARIABLE cli_stderr
)

set(cli_output "${cli_stdout}\n${cli_stderr}")

if(NOT cli_output MATCHES "${EXPECTED_OUTPUT}")
    message(FATAL_ERROR
        "Expected output was not found.\n"
        "Expected: ${EXPECTED_OUTPUT}\n"
        "Actual output:\n${cli_output}"
    )
endif()

if(NOT "${cli_result}" STREQUAL "${EXPECTED_EXIT_CODE}")
    message(FATAL_ERROR
        "Unexpected exit code.\n"
        "Expected: ${EXPECTED_EXIT_CODE}\n"
        "Actual: ${cli_result}"
    )
endif()