# 纯手工编写的 Makefile（不依赖 CMake）
# 适配编译器项目构建 + Docker 测试的 make build/run 命令
# 修复：Shell 重定向兼容性问题、优化构建逻辑、添加注释说明
# 补充：确保最终可执行文件具备执行权限（.o文件本身无需执行权限）

# ------------------------------
# 配置项（根据你的项目修改）
# ------------------------------
# 1. 编译器（使用 g++，对应 gcc15 镜像）
CXX = g++
# 2. 编译选项（C++20 标准、全警告、调试信息，可根据需求调整）
# 可选：添加 -O2 开启优化（发布版本），调试时保留 -g
CXXFLAGS = -std=c++20 -Wall -Wextra -g
# 3. 可执行文件名称
TARGET = Compiler
# 4. 源码文件（列出所有 .cpp 文件，按项目实际情况补充）
SRCS = main.cpp AST_node.cpp ir.cpp semantic_check.cpp
# 5. 目标文件（自动将 .cpp 转为 .o，无需手动修改）
OBJS = $(SRCS:.cpp=.o)
# 6. builtin.c 路径（测试要求输出到 STDERR 的文件）
BUILTIN_FILE = builtin.c
# 7. 临时文件（用于重定向的中间文件，避免冲突）
TEMP_STDERR = .temp_stderr.txt

# ------------------------------
# 核心目标：make build（编译编译器）
# ------------------------------
build: $(TARGET)
	# 补充：确保可执行文件具备执行权限（默认g++链接后已有执行权限，此处做兜底）
	chmod +x $(TARGET)

# 生成可执行文件：链接所有目标文件
# 注：.o文件是中间目标文件，仅用于链接，无需执行权限；最终可执行文件才需要执行权限
$(TARGET): $(OBJS)
	@echo "Linking object files to create $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build completed successfully!"

# 编译源码文件：将每个 .cpp 转为 .o（自动推导规则）
# 注：g++ -c 编译生成的.o文件默认权限为644（rw-r--r--），无执行权限是正常的
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# ------------------------------
# 核心目标：make run（运行编译器，满足测试要求）
# 功能：
# 1. 编译器 STDIN 读入源码
# 2. STDOUT 输出 IR 代码
# 3. STDERR 输出内容写入 builtin.c，同时 STDERR 也显示在终端
# 修复：兼容 Dash/ Bash 所有 Shell，避免进程替换语法错误
# ------------------------------
run: build
	@echo "Running $(TARGET)..."
	# 步骤1：执行编译器，将 STDERR 重定向到临时文件，STDOUT 正常输出（IR 代码）
	./$(TARGET) 2> $(TEMP_STDERR)
	# 步骤2：将临时文件内容写入 builtin.c（覆盖模式，若需追加则用 >> 替换 >）
	cat $(TEMP_STDERR) > $(BUILTIN_FILE)
	# 步骤3：将临时文件内容输出到终端的 STDERR（还原原逻辑的终端显示）
	cat $(TEMP_STDERR) >&2
	# 步骤4：删除临时文件（清理痕迹）
	rm -f $(TEMP_STDERR)
	@echo "Run completed! STDERR content saved to $(BUILTIN_FILE)."

# ------------------------------
# 辅助目标：make clean（清理构建产物）
# ------------------------------
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(TARGET) $(OBJS) $(TEMP_STDERR) $(BUILTIN_FILE)
	@echo "Clean completed!"

# ------------------------------
# 伪目标声明（避免和同名文件冲突）
# ------------------------------
.PHONY: build run clean