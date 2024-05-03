cmake_minimum_required(VERSION 3.27)

project(blob-sync VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (APPLE)
    list(APPEND CMAKE_PREFIX_PATH /opt/homebrew/opt/openssl)

    include_directories(/opt/homebrew/opt/libgit2/include)
    link_directories(/opt/homebrew/opt/libgit2/lib)
endif()

find_package(OpenSSL REQUIRED)

add_executable(blob-sync
        ${CMAKE_CURRENT_BINARY_DIR}/GitEnv.cpp
        main.cpp
        Base/Encoding.cpp
        Base/Crypto.cpp
        Base/StringUtil.cpp
        Base/BitUtil.cpp
        Base/IOUtil.cpp
        Base/SystemCommand.cpp
        Base/SystemCommandException.cpp
        App/Config.cpp
        App/ContentFileInfo.cpp
        App/ContentFileCache.cpp
        App/Command.cpp
        App/ScanCommand.cpp
        App/SyncCommand.cpp
        App/ContentStorage.cpp
        App/Repository.cpp
        App/Logger.cpp
        App/ConsoleLogger.cpp
        App/CommandArguments.cpp
        Git/Git.cpp
)

target_link_libraries(blob-sync
        OpenSSL::SSL
        git2
)

ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GitEnv.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/Dummy.cpp
        COMMAND ${CMAKE_COMMAND} -P
        ${CMAKE_CURRENT_SOURCE_DIR}/GitEnv.cmake)

configure_file(Version.h.in Version.h @ONLY)
