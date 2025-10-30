cmake_minimum_required(VERSION 3.31)
project(program_class LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

add_executable(program_class
        main.cpp
        Algorithm/src/FileHandle.cpp
        Algorithm/src/DocProcess.cpp
        Algorithm/src/DocCount.cpp
        Algorithm/src/DocConvert.cpp

)

# 测试标点转化功能
add_executable(only_convert_punctuation
        test/only_convert_punctuation.cpp
        Algorithm/src/DocProcess.cpp
)

# 测试删除句末空格功能
add_executable(only_remove_space
        test/only_remove_space.cpp
        Algorithm/src/FileHandle.cpp
        Algorithm/src/DocProcess.cpp
)

# 测试字符串的相关特性
add_executable(test_string
        test/test_string.cpp
        Algorithm/src/FileHandle.cpp
)

# 测试统计功能
add_executable(only_count
        test/only_count.cpp
        Algorithm/src/DocCount.cpp
        Algorithm/src/FileHandle.cpp)

# 测试转化功能
add_executable(only_convert_h2v
        test/only_convert_hor2ver.cpp
        Algorithm/src/DocConvert.cpp
        Algorithm/src/FileHandle.cpp
        Algorithm/src/DocProcess.cpp)