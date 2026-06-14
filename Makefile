# ==================== 项目配置 ====================
PROJECT_NAME := digitalImageProcessing
BUILD_DIR := build
EXECUTABLE := out/bin/$(PROJECT_NAME)
LINK_EXECUTABLE := $(PROJECT_NAME)
LINK_COMPILE_COMMANDS := compile_commands.json

# ==================== 编译器设置 ====================
CMAKE := cmake
CMAKE_BUILD := cmake --build
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# ==================== 颜色输出 ====================
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
NC := \033[0m

# ==================== 目标 ====================
.PHONY: all configure build clean distclean run debug release rebuild help debug-build release-build

# 默认目标：配置并构建
all: configure build
	@echo "$(GREEN)✓ Build completed successfully$(NC)"
	@echo "$(BLUE)Run with: ./$(LINK_EXECUTABLE) or make run$(NC)"

# 配置 CMake（默认模式）
configure:
	@echo "$(YELLOW)Configuring project...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@ln -sf $(BUILD_DIR)/compile_commands.json $(LINK_COMPILE_COMMANDS)
	@echo "$(GREEN)✓ Configuration completed$(NC)"

# 构建项目（基于已有配置）
build:
	@echo "$(YELLOW)Building project...$(NC)"
	@$(CMAKE_BUILD) $(BUILD_DIR) -j$(NPROC)
	@ln -sf $(EXECUTABLE) $(LINK_EXECUTABLE)
	@echo "$(GREEN)✓ Build completed$(NC)"

# Debug 配置（只配置，不编译）
debug:
	@echo "$(YELLOW)Configuring for Debug...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@ln -sf $(BUILD_DIR)/compile_commands.json $(LINK_COMPILE_COMMANDS)
	@echo "$(GREEN)✓ Debug configuration completed$(NC)"
	@echo "$(BLUE)Run 'make build' to compile, or 'make debug-build' for both$(NC)"

# Release 配置（只配置，不编译）
release:
	@echo "$(YELLOW)Configuring for Release...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@ln -sf $(BUILD_DIR)/compile_commands.json $(LINK_COMPILE_COMMANDS)
	@echo "$(GREEN)✓ Release configuration completed$(NC)"
	@echo "$(BLUE)Run 'make build' to compile, or 'make release-build' for both$(NC)"

# Debug 配置 + 编译
debug-build: debug
	@echo "$(YELLOW)Building Debug project...$(NC)"
	@$(CMAKE_BUILD) $(BUILD_DIR) -j$(NPROC)
	@ln -sf $(EXECUTABLE) $(LINK_EXECUTABLE)
	@echo "$(GREEN)✓ Debug build completed$(NC)"
	@echo "$(BLUE)Run with: ./$(LINK_EXECUTABLE) or make run$(NC)"

# Release 配置 + 编译
release-build: release
	@echo "$(YELLOW)Building Release project...$(NC)"
	@$(CMAKE_BUILD) $(BUILD_DIR) -j$(NPROC)
	@ln -sf $(EXECUTABLE) $(LINK_EXECUTABLE)
	@echo "$(GREEN)✓ Release build completed$(NC)"
	@echo "$(BLUE)Run with: ./$(LINK_EXECUTABLE) or make run$(NC)"

# 仅重新构建（不重新配置）
rebuild:
	@echo "$(YELLOW)Rebuilding project...$(NC)"
	@$(CMAKE_BUILD) $(BUILD_DIR) -j$(NPROC)
	@ln -sf $(EXECUTABLE) $(LINK_EXECUTABLE)
	@echo "$(GREEN)✓ Rebuild completed$(NC)"

# 运行程序
run: all
	@echo "$(YELLOW)Running $(PROJECT_NAME)...$(NC)"
	@echo "$(BLUE)========================================$(NC)"
	@./$(LINK_EXECUTABLE)
	@echo "$(BLUE)========================================$(NC)"
	@echo "$(GREEN)✓ Program finished$(NC)"

# 清理编译产物（保留配置，删除软链接）
clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@if [ -d $(BUILD_DIR) ]; then \
		$(CMAKE_BUILD) $(BUILD_DIR) --target clean; \
	fi
	@rm -f $(LINK_EXECUTABLE) $(LINK_COMPILE_COMMANDS)
	@echo "$(GREEN)✓ Clean completed$(NC)"

# 完全清理（删除整个build目录、out目录和软链接）
distclean:
	@echo "$(YELLOW)Removing all build files...$(NC)"
	@rm -rf $(BUILD_DIR) out
	@rm -f $(LINK_EXECUTABLE) $(LINK_COMPILE_COMMANDS)
	@echo "$(GREEN)✓ Distclean completed$(NC)"

# 显示项目信息
info:
	@echo "$(BLUE)Project Information:$(NC)"
	@echo "  Project Name:  $(PROJECT_NAME)"
	@echo "  Build Dir:     $(BUILD_DIR)"
	@echo "  Executable:    $(EXECUTABLE)"
	@echo "  Symlink:       $(LINK_EXECUTABLE)"
	@echo "  CPU Cores:     $(NPROC)"
	@echo "  Build Type:    $(shell [ -f $(BUILD_DIR)/CMakeCache.txt ] && grep CMAKE_BUILD_TYPE $(BUILD_DIR)/CMakeCache.txt | cut -d= -f2 || echo 'not configured')"

# 帮助信息
help:
	@echo "$(BLUE)Available targets:$(NC)"
	@echo ""
	@echo "  $(GREEN)Configuration (no build):$(NC)"
	@echo "    make configure - Configure project (default mode)"
	@echo "    make debug     - Configure Debug mode only"
	@echo "    make release   - Configure Release mode only"
	@echo ""
	@echo "  $(GREEN)Build (use existing config):$(NC)"
	@echo "    make build     - Build project with existing configuration"
	@echo "    make rebuild   - Rebuild without reconfiguring"
	@echo ""
	@echo "  $(GREEN)Configure + Build:$(NC)"
	@echo "    make all           - Configure (default) and build"
	@echo "    make debug-build   - Configure Debug + Build"
	@echo "    make release-build - Configure Release + Build"
	@echo "    make run           - Configure (default), build, and run"
	@echo ""
	@echo "  $(GREEN)Cleanup:$(NC)"
	@echo "    make clean     - Clean build artifacts (keep config, remove symlinks)"
	@echo "    make distclean - Remove everything (build dir + out dir + symlinks)"
	@echo ""
	@echo "  $(GREEN)Information:$(NC)"
	@echo "    make info      - Show project information"
	@echo "    make help      - Show this help"
	@echo ""
	@echo "$(BLUE)Typical workflow:$(NC)"
	@echo "  First time:           make debug-build"
	@echo "  After code change:    make build"
	@echo "  Switch to Release:    make release-build"
	@echo "  Quick run:            make run"