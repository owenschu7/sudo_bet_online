
cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# instead of downloading SFML, CMake reaches out to github and
# downloads the source code for SFML and perpares it to be built
# alongside your project
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# tell Cmake the name of your final program (run) and tells it
# which source files to compile to make it (src/main.cpp)
# when you add new .cpp files to your game you will list 
# them here -------------------V
add_executable(run 
  src/main.cpp
  src/Screen.h
  src/MainMenuScreen.h
  src/GameScreen.h

  # src/Card.cpp <- example
)
target_compile_features(run PRIVATE cxx_std_17)

# this connects the SFML libraries to your executable.
# if your games noods networking, you would simlpy add sfml-network to this
target_link_libraries(run PRIVATE SFML::Graphics)



# how to build and run your project
# CMake is a two-step process: configure (generating the bulid files for your specific system)
# and build (actually compiling the C++ code)

# STEP 1: CONFIGURE
# open your terminal or command prompt, navigate to the folder containing your CMakeLists.txt (this file)
# run these commands
# cmake -B build      // -B build flags tells CMake to put all the messy generated files into a folder named build
# 
# STEP 2: BULID
# cmake --build build     // looks inside the build folder and compiles your executable.
#                         // you will run this command every time you make a change to your C++ code
# 
# STEP 3: RUN
# once the build is complete you can run your app
# Windows: .\build\Debug\run.exe (or inside build\bin\)
# macOS / Linux: ./build/run (or inside ./build/bin/)




