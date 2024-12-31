cmake_minimum_required(VERSION 3.10)
project(Fruity-Frenzy-Basket-Bonanza VERSION 1.0.0)  # 修改這行，加入版本訊息

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add version information for GitHub releases
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# Configure version header
configure_file(
    "${PROJECT_SOURCE_DIR}/version.h.in"
    "${PROJECT_BINARY_DIR}/version.h"
)

# Include binary directory for version header
include_directories(${PROJECT_BINARY_DIR})

# Add executable
add_executable(fruity-game main.cpp)

# Install target
install(TARGETS fruity-game DESTINATION bin)
install(FILES highscores.txt DESTINATION share/fruity-game)

# Package configuration
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "dundd2 <dundd2@github.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fruity Frenzy Basket Bonanza Game")
set(CPACK_PACKAGE_VENDOR "dundd2")

# 新增 Debian 特定設定
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "dundd2 <dundd2@github.com>")  # 必要
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "A fun fruit catching game with multiple levels and challenges\n This game features:\n * Multiple fruit types\n * Power-ups\n * Achievement system\n * High score tracking")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.19), libstdc++6 (>= 4.8)")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_DEBIAN_FILE_NAME "fruity-game_${CPACK_PACKAGE_VERSION}_amd64.deb")

include(CPack)
