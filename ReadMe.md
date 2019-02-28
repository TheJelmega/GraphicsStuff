# Required libraries

To be able to compile, a Lib folder needs to contain the following libraries:

glm:

    Make sure to copy the glm header files to: Libs/glm/include/

SFML 2.5.1

GLFW 3.2.1: Make sure when building, to build glfw as x64, this can be done using the following lines:

    Windows, VS17: cmake -G "Visual Studio 15 2017 Win64"

    Make sure to copy the generated static library to: Libs/glfw-3.2.1/lib
    
    When using a debug build of glfw, remember to add the .pdb in the lib folder

    The header should be in: Libs/glfw-3.2.1/include


Currently, vertsion 1.1.97.0 of the vulkan library is used

Before running, make sure to navigate to GraphicsStuff/Data/Shader and to compile the included shaders
