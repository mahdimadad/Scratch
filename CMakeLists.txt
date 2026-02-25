cmake_minimum_required(VERSION 3.26)
project(Scratch)
set(CMAKE_CXX_STANDARD 17)

add_executable(Scratch
        src/main.cpp

        src/core/Engine.cpp
        src/core/Logger.cpp
        src/core/Events.cpp
        src/core/Runner.cpp
        src/core/Utils.cpp
        src/core/MultiRunner.cpp
        src/core/Eval.cpp

        src/ui/window.cpp
        src/ui/Renderer.cpp
        src/ui/UIManager.cpp
        src/ui/Text.cpp
)

target_include_directories(Scratch PRIVATE include)
target_link_libraries(Scratch -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)