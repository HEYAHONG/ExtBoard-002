--设置xmake最低版本
set_xmakever("2.9.0")
--获取当前目录
local current_dir=os.scriptdir()
--设置HRC目录
HRC_FS_ROOT_DIR=current_dir.."/fs/"
--添加第三方源代码
includes(current_dir.."/../3rdparty/")
--添加应用源代码
local TARGET_NAME = "ExtBoard-002-001-base"
local LIB_DIR = "$(buildir)/".. TARGET_NAME .. "/"
local LIB_NAME = "lib" .. TARGET_NAME .. ".a "

target(TARGET_NAME)
    set_kind("static")
    set_targetdir(LIB_DIR)
    
    --加入代码和头文件
    add_includedirs("./inc",{public = true})
    add_files("./src/*.c",{public = true})



    --路径可以随便写,可以加任意路径的代码,下面代码等效上方代码
    -- add_includedirs(SDK_TOP .. "/project/" .. TARGET_NAME .. "/inc",{public = true})
    -- add_files(SDK_TOP .. "/project/" .. TARGET_NAME .. "/src/*.c",{public = true})

    add_deps("HCppBox") --加入HCppBox支持

    --可以继续增加add_includedirs和add_files
    --自动链接
    LIB_USER = LIB_USER .. SDK_TOP ..  "/".. LIB_DIR .. LIB_NAME .. " "
    --甚至可以加入自己的库
target_end()
