cmake_minimum_required(VERSION 3.6)

project(AhoCorasick)

include_directories(${PROJECT_SOURCE_DIR}/include)

if (MSVC)  
    add_compile_options(/Zi   # pdb
                        /W4   # warning level 4
                        /EHsc  # exceptions: sync
                        /J    # use unsigned char
                        /Gd   # use cdecl
                        /we4715 # not all control paths return a value
                        /we4828 # disallow invalid characters
                        /we4473 # <function> : not enough arguments passed for format string 
                        /we4474 # <function> : too many arguments passed for format string 
                        /we4475 # <function> : length modifier <length> cannot be used with type field character <conversion-specifier> in format specifier 
                        /we4476 # <function> : unknown type field character <conversion-specifier> in format specifier 
                        /we4477 # <function> : format string <format-string> requires an argument of type <type>, but variadic argument <position> has type <type>
                        /we4478 # <function> : positional and non-positional placeholders cannot be mixed in the same format string 
                        /we4551 # function call missing argument list
                        /we4775 # nonstandard extension used in format string <format-string> of function <function> 
                        /we4776 # %<conversion-specifier> is not allowed in the format string of function <function>
                        /we4777 # <function> : format string <format-string> requires an argument of type <type>, but variadic argument <position> has type <type> 
                        /we4778 # <function> : unterminated format string <format-string>
                        /we4002 # too many actual parameters for macro 'identifier'
                        /we4003 # not enough actual parameters for macro 'identifier'
                        /we4840 # prinf-like function arg type mismatch
                        /we4309 # truncation of constant value
                        /we4113 # function pointers type mismatch
                        /we4804 # unsafe use of type 'bool' in operation
                        /utf-8  # utf-8 source
                        /wd4201 # disable 'nonstandard extension used: nameless struct/union'
                        /wd4200 # disable 'nonstandard extension used: zero-sized array in struct/union'
                        /MP     # multiprocessor compilation
                        /GF     # enable string pooling
                        /Zc:__cplusplus # https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
    )
    
    set(RELEASE_LINKER_FLAGS "/LTCG /OPT:REF,ICF /DEBUG")
    set(RELEASE_COMPILER_FLAGS "-DNDEBUG /Ox /Ob2 /Oi /Ot /Oy /GS- /Gy /GL /Gw /MT")
    set(DEBUG_COMPILER_FLAGS "/GS /MTd -D_DEBUG")
    
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_COMPILER_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_COMPILER_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_COMPILER_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RELEASE_COMPILER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${RELEASE_LINKER_FLAGS}")
endif()

add_executable(basicCharTestExec tests/basicCharTest.cpp)
add_executable(basicWCharTestExec tests/basicWCharTest.cpp)
add_executable(basicBinaryTestExec tests/basicBinaryTest.cpp)
add_executable(basicBinary64TestExec tests/basicBinary64Test.cpp)
add_executable(realWorldExamplesExec tests/realWorldExamples.cpp)
add_executable(basicNegativeTestExec tests/basicNegativeTest.cpp)
add_executable(continueTestExec tests/continueTest.cpp)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT basicCharTestExec)

enable_testing()
add_test(NAME basicCharTest         COMMAND basicCharTestExec)
add_test(NAME basicWCharTest        COMMAND basicWCharTestExec)
add_test(NAME basicBinaryTest       COMMAND basicBinaryTestExec)
add_test(NAME basicBinary64Test     COMMAND basicBinary64TestExec)
add_test(NAME realWorldExamplesTest COMMAND realWorldExamplesExec)
add_test(NAME basicNegativeTest     COMMAND basicNegativeTestExec)
add_test(NAME continueTest          COMMAND continueTestExec)
