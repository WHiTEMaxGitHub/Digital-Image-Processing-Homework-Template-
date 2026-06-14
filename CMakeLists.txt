# ==================== 需要修改的区域 ====================
cmake_minimum_required(VERSION 3.26)
project(digitalImageProcessing)

# ==================== 可选修改的区域 ====================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置构建类型（默认Debug）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 只允许Debug和Release两种模式
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "不支持的构建类型: ${CMAKE_BUILD_TYPE}，只支持 Debug 或 Release")
endif()

# Debug和Release模式的不同编译标志
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    add_compile_definitions(DEBUG _DEBUG ENABLE_LOGGING=1)
    message(STATUS "Debug 模式: 已定义 DEBUG, _DEBUG, ENABLE_LOGGING=1")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    add_compile_definitions(RELEASE NDEBUG ENABLE_LOGGING=0)
    message(STATUS "Release 模式: 已定义 RELEASE, NDEBUG, ENABLE_LOGGING=0")
endif()

# ==================================================
#           依赖管理系统の使用须知
# 需要注意，如果需求的库命名方式并不符合cmake的旧式规范，则需
# 要向下面那样根据库作者的 Find<Lib_Name>.cmake 或
# <Lib_Name>Config.cmake 进行特殊处理。
# 这时只需要在子项目中添加：
# set(DEPENDENCIES <依赖名...>)即可，例如：
# set(DEPENDENCIES OpenCV)
# ==================================================

function(handle_dependencies TARGET_NAME)
    foreach(DEP ${ARGN})
        message(STATUS "   📌 依赖: ${DEP}")
        
        find_package(${DEP} REQUIRED)
        
        # OpenCV 特殊处理
        if(${DEP} STREQUAL "OpenCV")
            target_include_directories(${TARGET_NAME} 
                PRIVATE ${OpenCV_INCLUDE_DIRS}
            )
            target_link_libraries(${TARGET_NAME} 
                PRIVATE ${OpenCV_LIBS}
            )
            message(STATUS "      ✓ 头文件: ${OpenCV_INCLUDE_DIRS}")
            message(STATUS "      ✓ 库文件: opencv_*")
        
        # 通用处理（其他库走标准命名约定）
        else()
            if(${DEP}_INCLUDE_DIRS)
                target_include_directories(${TARGET_NAME} 
                    PRIVATE ${${DEP}_INCLUDE_DIRS}
                )
                message(STATUS "      ✓ 头文件: ${${DEP}_INCLUDE_DIRS}")
            endif()
            
            if(${DEP}_LIBRARIES)
                target_link_libraries(${TARGET_NAME} 
                    PRIVATE ${${DEP}_LIBRARIES}
                )
                message(STATUS "      ✓ 库文件: ${${DEP}_LIBRARIES}")
            elseif(TARGET ${DEP}::${DEP})
                target_link_libraries(${TARGET_NAME} 
                    PRIVATE ${DEP}::${DEP}
                )
                message(STATUS "      ✓ 导入目标: ${DEP}::${DEP}")
            endif()
        endif()
    endforeach()
endfunction()

# ==================== 以下内容不需要修改 ====================
# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)

# 生成 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 自动扫描子项目
file(GLOB SUB_PROJECTS LIST_DIRECTORIES true RELATIVE ${CMAKE_SOURCE_DIR}/Source ${CMAKE_SOURCE_DIR}/Source/*)

foreach(SUB_PROJECT ${SUB_PROJECTS})
    if(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Source/${SUB_PROJECT})
        continue()
    endif()
    
    set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Source/${SUB_PROJECT}/include)
    set(PROJECT_SRC_DIR ${CMAKE_SOURCE_DIR}/Source/${SUB_PROJECT}/src)
    set(CONFIG_FILE ${CMAKE_SOURCE_DIR}/Source/${SUB_PROJECT}/.project_config.cmake)
    
    if(NOT EXISTS ${PROJECT_SRC_DIR})
        message(WARNING "跳过 ${SUB_PROJECT}: src目录不存在")
        continue()
    endif()
    
    file(GLOB_RECURSE PROJECT_SOURCES 
        ${PROJECT_SRC_DIR}/*.cpp 
        ${PROJECT_SRC_DIR}/*.cc 
        ${PROJECT_SRC_DIR}/*.cxx
        ${PROJECT_SRC_DIR}/*.c
    )
    
    if(NOT PROJECT_SOURCES)
        message(WARNING "跳过 ${SUB_PROJECT}: 没有找到源文件")
        continue()
    endif()
    
    # ============================================
    # 读取配置文件（必须存在！）
    # ============================================
    if(NOT EXISTS ${CONFIG_FILE})
        message(FATAL_ERROR "❌ ${SUB_PROJECT}: 缺少配置文件 .project_config.cmake")
    endif()
    
    include(${CONFIG_FILE})
    message(STATUS "📄 ${SUB_PROJECT}: 使用配置文件 (TYPE=${TYPE})")
    
    # ============================================
    # 根据配置创建目标
    # ============================================
    if(TYPE STREQUAL "executable")
        add_executable(${SUB_PROJECT} ${PROJECT_SOURCES})
        message(STATUS "🚀 ${SUB_PROJECT} -> 可执行文件")
        
    elseif(TYPE STREQUAL "shared")
        add_library(${SUB_PROJECT} SHARED ${PROJECT_SOURCES})
        message(STATUS "🔗 ${SUB_PROJECT} -> 动态库")
        
    elseif(TYPE STREQUAL "static")
        add_library(${SUB_PROJECT} STATIC ${PROJECT_SOURCES})
        message(STATUS "📦 ${SUB_PROJECT} -> 静态库")
        
    elseif(TYPE STREQUAL "object")
        add_library(${SUB_PROJECT} OBJECT ${PROJECT_SOURCES})
        message(STATUS "📦 ${SUB_PROJECT} -> 对象库")
        
    else()
        message(FATAL_ERROR "❌ ${SUB_PROJECT}: 未知类型 '${TYPE}'")
    endif()
    
    # ============================================
    # 自动依赖处理
    # ============================================
    if(DEPENDENCIES)
        message(STATUS "📦 处理依赖:")
        handle_dependencies(${SUB_PROJECT} ${DEPENDENCIES})
    endif()
    
    # ============================================
    # 应用配置
    # ============================================
    
    if(OUTPUT_NAME)
        set_target_properties(${SUB_PROJECT} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
        message(STATUS "   输出名称: ${OUTPUT_NAME}")
    endif()
    
    set_target_properties(${SUB_PROJECT} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    )
    
    if(EXISTS ${PROJECT_INCLUDE_DIR})
        target_include_directories(${SUB_PROJECT} 
            PUBLIC 
                $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
                $<INSTALL_INTERFACE:include>
        )
    endif()
    
    if(COMPILE_DEFINITIONS)
        target_compile_definitions(${SUB_PROJECT} PRIVATE ${COMPILE_DEFINITIONS})
        message(STATUS "   编译定义: ${COMPILE_DEFINITIONS}")
    endif()
    
    if(COMPILE_OPTIONS)
        target_compile_options(${SUB_PROJECT} PRIVATE ${COMPILE_OPTIONS})
        message(STATUS "   编译选项: ${COMPILE_OPTIONS}")
    endif()
    
    if(LINK_LIBRARIES)
        target_link_libraries(${SUB_PROJECT} PRIVATE ${LINK_LIBRARIES})
        message(STATUS "   链接库: ${LINK_LIBRARIES}")
    endif()
    
    if(LINK_DIRECTORIES)
        target_link_directories(${SUB_PROJECT} PRIVATE ${LINK_DIRECTORIES})
        message(STATUS "   链接目录: ${LINK_DIRECTORIES}")
    endif()
    
    if(TYPE STREQUAL "executable")
        set_target_properties(${SUB_PROJECT} PROPERTIES
            BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
            INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
        )
    endif()
    
    message(STATUS "")
endforeach()

# 打印构建总结
message(STATUS "========================================")
message(STATUS "构建配置完成 (${CMAKE_BUILD_TYPE} 模式)")
message(STATUS "输出目录:")
message(STATUS "  可执行文件: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "  动态库文件: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "  静态库文件: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "========================================")