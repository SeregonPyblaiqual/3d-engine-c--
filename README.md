## Warnings

I have tried to have a clean structure, but I have little experience so I don't know what are the conventional ways to structure a vs project, it may be very different from what senior developers expect.

## Requirements

- Having visual studio 2022 with vs c++ compiler installed

## Getting started

- Copy the one line command located at Build/Instructions/build_one_line.bat

## Tooling

### Vs code extensions

- Dark Modern theme

- code
  - aaron-bond.better-comments
  - VisualStudioExptTeam.vscodeintellicode
  - (VisualStudioExptTeam.intellicode-api-usage-examples) ?
  - Remisa.mathover
- c++
  - ms-vscode.cpptools
  - ms-vscode.cpptools-extension-pack
    - ms-vscode.cpptools
    - ms-vscode.cpptools-themes ?
    - (twxs.cmake)
    - (ms-vscode.cmake-tools)
- glsl
  - dtoplak.vscode-glsllint
  - slevesque.shader
- git
  - eamodio.gitlens

## Convention used

camelCase for functions
snake*case for variables
snake_case with "m*" prefix for member variables
UPPER_SNAKE_CASE for constant variables

## Sources

### Dependencies

- 3d engine c++/Dependencies/Includes
  - FastNoiseLite
    - https://github.com/Auburn/FastNoiseLite
  - glad
    - https://glad.dav1d.de/
  - ...
- 3d engine c++/Dependencies/Libs
  - glfw3_64.lib
    - https://www.glfw.org/download.html
      - precompiled binaries windows 64 bit version > lib-vc2022
  - glfw3_32.lib - https://www.glfw.org/download.html - precompiled binaries windows 32 bit version > lib-vc2022

### Assets

- 3d engine c++/Assets/Textures/container.jpg
  - https://learnopengl.com/img/textures/container.jpg
    - https://learnopengl.com/Getting-started/Textures
- 3d engine c++/Assets/Textures/grass_block_top_x64.png
  - https://faithfulpack.net/faithful64x/R4
- 3d engine c++/Assets/Textures/grass_block_top_x16.png
  - https://texture-packs.com/resourcepack/default-pack/
- 3d engine c++/Assets/Textures/grassx1000.jpg
  - ?
- 3d engine c++/Assets/Textures/grass.jpg
  - https://seamless-pixels.blogspot.com/2014/07/grass-5-seamless-turf-lawn-green-ground.html
