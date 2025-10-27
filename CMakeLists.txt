cmake_minimum_required(VERSION 3.31)
project(program_class LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

add_executable(program_class main.cpp)

# 测试标点转化功能
add_executable(only_convert_punctuation
        test/only_convert_punctuation.cpp
        algorithm/src/DocProcess.cpp
)

# 测试删除句末空格功能
add_executable(only_remove_space
        test/only_remove_space.cpp
        algorithm/src/FileHandle.cpp
        algorithm/src/DocProcess.cpp
)

# 测试字符串的相关特性
add_executable(test_string
        test/test_string.cpp
        algorithm/src/FileHandle.cpp
)