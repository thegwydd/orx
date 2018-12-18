-- This premake script should be used with orx-customized version of premake4.
-- Its Hg repository can be found at https://bitbucket.org/orx/premake-stable.
-- A copy, including binaries, can also be found in the extern/premake folder.

--
-- Globals
--

function initconfigurations ()
    return
    {
        "Debug",
        "Profile",
        "Release"
    }
end

function initplatforms ()
    if os.is ("windows") then
        if string.lower(_ACTION) == "vs2013"
        or string.lower(_ACTION) == "vs2015"
        or string.lower(_ACTION) == "vs2017" then
            return
            {
                "x64",
                "x32"
            }
        else
            return
            {
                "Native"
            }
        end
    elseif os.is ("linux") then
        if os.is64bit () then
            return
            {
                "x64",
                "x32"
            }
        else
            return
            {
                "x32",
                "x64"
            }
        end
    elseif os.is ("macosx") then
        if string.find(string.lower(_ACTION), "xcode") then
            return
            {
                "Universal"
            }
        else
            return
            {
                "x32", "x64"
            }
        end
    end
end

function defaultaction (name, action)
   if os.is (name) then
      _ACTION = _ACTION or action
   end
end

defaultaction ("windows", "vs2015")
defaultaction ("linux", "gmake")
defaultaction ("macosx", "gmake")

newoption
{
    trigger = "to",
    value   = "path",
    description = "Set the output location for the generated files"
}

if os.is ("macosx") then
    osname = "mac"
else
    osname = os.get()
end

destination = _OPTIONS["to"] or "./" .. osname .. "/" .. _ACTION
copybase = path.rebase ("..", os.getcwd (), os.getcwd () .. "/" .. destination)


--
-- Solution: orx
--

solution "orxFontGen"

    language ("C")

    location (destination)

    kind ("ConsoleApp")

    configurations
    {
        initconfigurations ()
    }

    platforms
    {
        initplatforms ()
    }

    includedirs
    {
        "../include",
        "../../../code/include",
        "$(ORX)/include",
        "../../../extern/stb_image",
        "../../../extern/freetype/include"
    }

    configuration {"not macosx"}
        libdirs
        {
            "../lib",
            "../../../code/lib/static",
            "$(ORX)/lib/static"
        }

    configuration {"macosx"}
        libdirs
        {
            "../../../code/lib/dynamic",
            "$(ORX)/lib/dynamic"
        }

    configuration {}

    targetdir ("../bin/")

    flags
    {
        "NoPCH",
        "NoManifest",
        "FloatFast",
        "NoNativeWChar",
        "NoExceptions",
        "Symbols",
        "StaticRuntime"
    }

    configuration {"not vs2013", "not vs2015", "not vs2017"}
        flags {"EnableSSE2"}

    configuration {"not x64"}
        flags {"EnableSSE2"}

    configuration {"not windows"}
        flags {"Unicode"}

    configuration {"*Debug*"}
        defines {"__orxDEBUG__"}
        links {"orxd"}

    configuration {"*Profile*"}
        defines {"__orxPROFILER__"}
        flags {"Optimize", "NoRTTI"}
        links {"orxp"}

    configuration {"*Release*"}
        flags {"Optimize", "NoRTTI"}
        links {"orx"}

    configuration {}
        defines {"__orxSTATIC__"}


-- Linux

    configuration {"linux", "x32"}
        libdirs
        {
            "../../../extern/freetype/lib/linux",
        }

    configuration {"linux", "x64"}
        libdirs
        {
            "../../../extern/freetype/lib/linux64"
        }

    configuration {"linux"}
        buildoptions {"-Wno-unused-function"}

    -- This prevents an optimization bug from happening with some versions of gcc on linux
    configuration {"linux", "not *Debug*"}
        buildoptions {"-fschedule-insns"}


-- Mac OS X

    configuration {"macosx"}
        libdirs
        {
            "../../../extern/freetype/lib/mac"
        }
        buildoptions
        {
            "-mmacosx-version-min=10.7",
            "-gdwarf-2",
            "-Wno-write-strings"
        }
        linkoptions
        {
            "-mmacosx-version-min=10.7",
            "-dead_strip"
        }
        postbuildcommands {"$(shell [ -f " .. copybase .. "/../../code/lib/dynamic/liborx.dylib ] && cp -f " .. copybase .. "/../../code/lib/dynamic/liborx*.dylib " .. copybase .. "/bin)"}

    configuration {"macosx", "x32"}
        buildoptions
        {
            "-mfix-and-continue"
        }


-- Windows

    configuration {"windows", "vs*"}
        buildoptions
        {
            "/MP"
        }

    configuration {"windows", "vs*", "*Debug*"}
        linkoptions {"/NODEFAULTLIB:LIBCMT /FdorxFontGen_lnk.pdb"}

    configuration {"vs2012"}
        libdirs
        {
            "../../../extern/freetype/lib/vc2012"
        }

    configuration {"vs2013", "x32"}
        libdirs
        {
            "../../../extern/freetype/lib/vc2013/32"
        }

    configuration {"vs2013", "x64"}
        libdirs
        {
            "../../../extern/freetype/lib/vc2013/64"
        }

    configuration {"vs2015 or vs2017", "x32"}
        libdirs
        {
            "../../../extern/freetype/lib/vc2015/32"
        }

    configuration {"vs2015 or vs2017", "x64"}
        libdirs
        {
            "../../../extern/freetype/lib/vc2015/64"
        }

    configuration {"windows", "codeblocks or codelite or gmake"}
        libdirs
        {
            "../../../extern/freetype/lib/mingw"
        }


--
-- Project: orxFontGen
--

project "orxFontGen"

    files {"../src/orxFontGen.c"}
    targetname ("orxfontgen")
    links
    {
        "freetype"
    }


-- Linux

    configuration {"linux"}
        links
        {
            "dl",
            "m",
            "z",
            "rt",
            "pthread"
        }


-- Mac OS X

    configuration {"macosx", "not codelite", "not codeblocks"}
        links
        {
            "Foundation.framework",
            "AppKit.framework",
            "OpenGL.framework"
        }

    configuration {"macosx", "codelite or codeblocks"}
        linkoptions
        {
            "-framework Foundation",
            "-framework AppKit",
            "-framework OpenGL"
        }

    configuration {"macosx"}
        links
        {
            "z",
            "pthread"
        }


-- Windows

    configuration {"windows"}
        links
        {
            "winmm",
            "OpenGL32"
        }
