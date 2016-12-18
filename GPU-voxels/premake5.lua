
workspace "gpu-voxels"
    if _ACTION then
        location("build/" .._ACTION)
    end

    configurations { "Debug", "Release" }

    project "app"
        kind "ConsoleApp"
        language "C++"
        targetdir "bin"
        files {
            "src/**.cpp", "src/**.h", "shaders/**.glsl",
            "extern/imgui/**.cpp", "extern/json11/**.cpp",
            "extern/tinyobj/**.cc", "src/config.json"
        }
        includedirs { "src/", "extern/", "extern/SDL/include", "extern/glew-1.13.0/include" }
        
        -- global workspace configurations
        filter "configurations:Debug"
            defines { "DEBUG" }
            flags { "Symbols" }
            debugdir "bin"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"    
        
        project "app"
            postbuildcommands {
                "{COPY} ../../src/config.json ../../bin/"
            }

        --[[
  _   ___               __  ______          ___    
 | | / (_)__ __ _____ _/ / / __/ /___ _____/ (_)__ 
 | |/ / (_-</ // / _ `/ / _\ \/ __/ // / _  / / _ \
 |___/_/___/\_,_/\_,_/_/ /___/\__/\_,_/\_,_/_/\___/
                                                   
--]]
        configuration "vs*"
            defines { "_CRT_SECURE_NO_WARNINGS" }
            prebuildcommands {
                "if not exist \"..\\..\\bin\\shaders\" mkdir ..\\..\\bin\\shaders",
                ""
            }
            links { "SDL2", "glew32", "opengl32" }
            libdirs {
                "extern/SDL/lib",
                "extern/glew-1.13.0/lib/"
            }
            filter "files:**.glsl"
                buildcommands { "copy ..\\..\\shaders\\%{file.name} ..\\..\\bin\\shaders" }
                buildoutputs { "..\\..\\bin\\shaders\\%{file.name}" }

        --[[
   __  ___     __       ____ __   
  /  |/  /__ _/ /_____ / _(_) /__ 
 / /|_/ / _ `/  '_/ -_) _/ / / -_)
/_/  /_/\_,_/_/\_\\__/_//_/_/\__/ 
                                  
--]]
        configuration "gmake"
            -- Mac and Linux support go here
            prebuildcommands {
                "mkdir -p ../../bin/shaders"
            }
            filter "files:**.wren"
                buildcommands { "cp ../../shaders/%{file.name} ../../bin/shaders" }
                buildoutputs { "../../bin/shaders/%{file.name}" }
