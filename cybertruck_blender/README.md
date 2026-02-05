<style>
    @media print {
        header {
            display: none !important;
        }
        .markdown-body {
            padding: 0 !important;
            border: none !important;
        }
        .page-break {
            page-break-after: always;
        }
        .todo {
            display: none;
        }
    }
    @media not print {
        .page-break {
            border-bottom: 1px dashed red;
            margin-bottom: 1em;
        }
    }
    .hljs {
        border-radius: 1em !important;
    }
    .todo {
        color: red;
        font-weight: bold;
    }
    .todo:before {
        content: "TODO: ";
    }
</style>

# Model progress

<img src="./doc/model-progress/1.png"  width="230">
<img src="./doc/model-progress/2.png"  width="230">
<img src="./doc/model-progress/3.png"  width="230">
<img src="./doc/model-progress/4.png"  width="230">
<img src="./doc/model-progress/5.png"  width="230">
<img src="./doc/model-progress/6.png"  width="230">
<img src="./doc/model-progress/7.png"  width="230">
<img src="./doc/model-progress/8.png"  width="230">
<img src="./doc/model-progress/9.png"  width="230">
<img src="./doc/model-progress/10.png" width="230">
<img src="./doc/model-progress/11.png" width="230">
<img src="./doc/model-progress/12.png" width="230">
<img src="./doc/model-progress/13.png" width="230">
<img src="./doc/model-progress/14.png" width="230">
<img src="./doc/model-progress/15.png" width="230">
<img src="./doc/model-progress/16.png" width="230">
<img src="./doc/model-progress/17.png" width="230">
<img src="./doc/model-progress/18.png" width="230">
<img src="./doc/model-progress/19.png" width="230">
<img src="./doc/model-progress/20.png" width="230">
<img src="./doc/model-progress/21.png" width="230">
<img src="./doc/model-progress/22.png" width="230">
<img src="./doc/model-progress/23.png" width="230">
<img src="./doc/model-progress/24.png" width="230">

<div class="page-break"></div>

# OpenGL run

<img src="./doc/opengl-run/1.png"  width="230">
<img src="./doc/opengl-run/2.png"  width="230">
<img src="./doc/opengl-run/3.png"  width="230">
<img src="./doc/opengl-run/4.png"  width="230">
<img src="./doc/opengl-run/5.png"  width="230">
<img src="./doc/opengl-run/6.png"  width="230">
<img src="./doc/opengl-run/7.png"  width="230">
<img src="./doc/opengl-run/8.png"  width="230">
<img src="./doc/opengl-run/9.png"  width="230">
<img src="./doc/opengl-run/10.png" width="230">
<img src="./doc/opengl-run/11.png" width="230">

<div class="page-break"></div>

# Team members and work division

- **40281057** - Mykyta Onipchenko
    - Implemented model loading
- **40283137** - Bogdan Ivan
    - Textured the car
- **40283954** - Kelsey Pereira Costa
    - Implemented diffuse lighting
- **40264524** - Maxime Smith
    - Modelled the car

<div class="page-break"></div>

# File structure

- `src/`, `vendor/`, `CMakeLists.txt`, `CMakePresets.json`, `run.sh`
    - Files necessary for building and running the project
- `res/model/`
    - Resources including the `.blend` file of the model and exported `.obj` file

# Running

## Pre-requisites

- Linux
    1. Install `cmake`, `make`, and `g++` through your package manager
- Windows
    1. Install [CMake](https://cmake.org/download/)
    2. Install [MinGW](https://code.visualstudio.com/docs/cpp/config-mingw#_installing-the-mingww64-toolchain) or MSVC (honestly no clue how to do it without installing Visual Studio)

## Cloning

- Make sure to clone with `--recursive` flag
    - This will clone all library build dependencies
    - If you didn't, run
        ```sh
        git submodule update --init
        ```

## Running

- VSCode
    1. Open the project
    2. If prompted, install recommended extensions
    3. If prompted, select `Debug` or `Release` CMake configuration
    4. Run with `Ctrl+F5`
- Other
    1. Build and run the project with CMake
        ```sh
        # Where $PRESET is either `Debug` or `Release` (without ``)
        cmake --preset $PRESET
        cmake --build --preset $PRESET
        ./build/$PRESET/assignment-3
        ```
    2. For clarifications, take a look at `run.sh` script
