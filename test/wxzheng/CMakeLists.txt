include(FetchContent)

# 3.1 自动下载 gtest（使用Gitee镜像）
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://gitee.com/mirrors/googletest.git  # 使用Gitee镜像
        GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# 3.2 编译测试可执行文件
add_executable(wxzheng_test test.cpp
        ../../tokenizer.h
        ../../AST_node.h
        ../../semantic_check.h
        ../../semantic_check.cpp
        ../../parser.h
        ../../AST_node.cpp)

# 3.3 链接被测库 + gtest
target_link_libraries(wxzheng_test
        PRIVATE
        gtest_main          # 自带 main 函数，不用自己写
)

# 3.4 把测试注册到 CTest
add_test(NAME wxzheng_test COMMAND wxzheng_test)
