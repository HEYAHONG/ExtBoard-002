--获取当前目录
local current_dir=os.scriptdir()
--获取HRC_FS_ROOT_DIR并生成相应文件
local hrc_fs_root_dir=os.getenv("HRC_FS_ROOT_DIR")
if  HRC_FS_ROOT_DIR then
    hrc_fs_root_dir=HRC_FS_ROOT_DIR
end
if  hrc_fs_root_dir then
       before_build(function()
                        os.exec(current_dir.."/master/hrc/fsgen.exe".." "..hrc_fs_root_dir.." "..current_dir.."/RC_fs.c")
                    end)
end
--添加TARGET
local TARGET_NAME = "HCppBox"
local LIB_DIR = "$(buildir)/3rdparty/".. TARGET_NAME .. "/"
local LIB_NAME = "lib" .. TARGET_NAME .. ".a "

target(TARGET_NAME)
    set_kind("static")
    set_targetdir(LIB_DIR)

    --关闭不关注的警告
    add_cxxflags("-Wno-unused-parameter","-Wno-effc++","-Wno-sign-compare")
    add_cflags("-Wno-unused-parameter","-Wno-sign-compare")
    
    --添加宏定义
    add_defines("HBOX_CONFIG_HEADER=hbox_config.h")
    --加入代码和头文件
    add_includedirs("./",{public = true})
    add_includedirs("./master/hbox",{public = true})
    add_includedirs("./master/hbox/cpp",{public = true})
    add_files("./*.c",{public = true})
    add_files("./master/hbox/*.c",{public = true})
    add_files("./master/hbox/cpp/*.cpp",{public = true})
    add_files("./master/hbox/modbus/*.c",{public = true})
    add_files("./master/hbox/gui/*.c",{public = true})

    if hrc_fs_root_dir then
        add_includedirs("./master/hrc",{public = true})
        add_files("./master/hrc/*.c",{public = true})
        add_files("./RC_fs.c",{public = true})
    end

    --可以继续增加add_includedirs和add_files
    --自动链接
    LIB_USER = LIB_USER .. SDK_TOP ..  "/".. LIB_DIR .. LIB_NAME .. " "
    --甚至可以加入自己的库
target_end()

