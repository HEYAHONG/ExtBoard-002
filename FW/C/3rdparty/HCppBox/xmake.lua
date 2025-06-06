--获取当前目录
local current_dir=os.scriptdir()
--SDK补丁路径
local sdk_patch_src_file_name=current_dir.."/sdk_patch/ec618_0h00_flash.c"
local sdk_patch_dest_file_name=current_dir.."/../../sdk/PLAT/core/ld/ec618_0h00_flash.c"
--获取HRC_FS_ROOT_DIR
local hrc_fs_root_dir=os.getenv("HRC_FS_ROOT_DIR")
if  HRC_FS_ROOT_DIR then
    hrc_fs_root_dir=HRC_FS_ROOT_DIR
end
--添加TARGET
local TARGET_NAME = "HCppBox"
local LIB_DIR = "$(buildir)/3rdparty/".. TARGET_NAME .. "/"
local LIB_NAME = "lib" .. TARGET_NAME .. ".a "

target(TARGET_NAME)
    set_kind("static")
    set_targetdir(LIB_DIR)

    --构建HCppBox之前需要的操作
    before_build(function()
                    os.cp(sdk_patch_src_file_name,sdk_patch_dest_file_name)
                    if  hrc_fs_root_dir then
                        os.exec(current_dir.."/master/hrc/fsgen.exe".." "..hrc_fs_root_dir.." "..current_dir.."/RC_fs.c")
                    end
                end)


    --关闭不关注的警告
    add_cxxflags("-Wno-unused-parameter","-Wno-effc++","-Wno-sign-compare","-Wno-clobbered","-Wno-stack-usage")
    add_cflags("-Wno-unused-parameter","-Wno-sign-compare","-Wno-clobbered","-Wno-stack-usage")
    
    
    
    --设置C/C++标准
    add_cxxflags("-std=gnu++11")
    add_cflags("-std=gnu11",{force=true})

    --添加宏定义
    add_defines("HBOX_CONFIG_HEADER=hbox_config.h")
    --加入代码和头文件
    add_includedirs("./",{public = true})
    add_includedirs("./master/hbox",{public = true})
    add_includedirs("./master/hbox/cpp",{public = true})
    add_files("./*.c",{public = true})
    --add_files("./*.cpp",{public = true})
    add_files("./master/hbox/*.c",{public = true})
    add_files("./master/hbox/h3rdparty/*.c",{public = true,cflags = " -Wno-implicit-fallthrough "})
    add_files("./master/hbox/cpp/*.cpp",{public = true})

    if hrc_fs_root_dir then
        add_defines("HRC_ENABLED=1")
        add_includedirs("./master/hrc",{public = true})
        add_files("./master/hrc/*.c",{public = true})
        add_files("./RC_fs.c",{public = true})
    end

    --可以继续增加add_includedirs和add_files
    --自动链接
    LIB_USER = LIB_USER .. SDK_TOP ..  "/".. LIB_DIR .. LIB_NAME .. " "
    --甚至可以加入自己的库
target_end()

