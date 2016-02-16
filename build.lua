local lfs = require "lfs"

local build_include_files = "#pragma once\n"

function string.ends_with(str, e)
   return e == '' or string.sub(str,-string.len(e)) == e
end

for filename in lfs.dir(".") do
    if filename ~= "main.cpp" and filename ~= "build_include.h" and filename ~= "memory_test.cpp" and string.ends_with(filename, ".cpp") then
        build_include_files = build_include_files .. "#include \"" .. filename .. "\"\n"
    end
end

file = io.open("build_include.h", "w")
file:write(build_include_files)
file:close()

function arg_contain(str)
  for _, value in pairs(arg) do
    if value == str then
      return true
    end
  end
  return false
end

local set_env = arg_contain("set_env")
local build = arg_contain("build")
local run = arg_contain("run")
local cmd = ""

if set_env then
    vs_dir = os.getenv("VS140COMNTOOLS")
    cmd = "\"" .. vs_dir .. "..\\..\\VC\\vcvarsall.bat\" amd64"
end

if build then
    if cmd:len() ~= 0 then
        cmd = cmd .. " && "
    end

    cmd = cmd .. "cl.exe /D _HAS_EXCEPTIONS=0 /W4 /WX /TP /DUNICODE /wd4201 /wd4100 /D _CRT_SECURE_NO_WARNINGS /Zi /MTd /D DEBUG main.cpp /link /subsystem:windows /entry:mainCRTStartup /incremental:no d3d11.lib user32.lib D3DCompiler.lib /out:skugga.exe"
end

if run then
    if cmd:len() ~= 0 then
        cmd = cmd .. " && "
    end

    cmd = cmd .. "skugga.exe"
end

os.execute(cmd)
