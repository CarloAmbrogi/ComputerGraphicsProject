cl /MD /I C:\VulkanSDK\glfw-3.3.8.bin.WIN64\include /I .\headers\ /I C:\VulkanSDK\1.3.239.0\Include /std:c++17 /EHsc .\LabyrinthSurvival.cpp gdi32.lib opengl32.lib kernel32.lib user32.lib shell32.lib glfw3.lib vulkan-1.lib /link /LIBPATH:C:\VulkanSDK\glfw-3.3.8.bin.WIN64\lib-vc2022 /LIBPATH:C:\VulkanSDK\1.3.239.0\Lib