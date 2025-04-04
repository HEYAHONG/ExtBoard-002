# 说明

使用C/C++语言开发ExtBoard-002的固件。

主要开发Air780EG模块。

# 源代码下载

由于本源代码包含第三方源代码,故直接下载可能有部分源代码缺失，需要通过以下方法解决:

- 在进行git clone 使用--recurse-submodules参数。

- 若已通过git clone下载,则在源代码目录中执行以下命令下载子模块:

  ```bash
   git submodule update --init --recursive
  ```

# 目录结构

- [sdk](sdk):合宙官方SDK，理论上可用于移芯*EC618*平台的模组，包括Air780EG。
- [3rdparty](3rdparty)：第三方库。可在[xmake](https://xmake.io)中使用`includes()`函数将其添加至自己的目录。
- 固件目录，通常以[固件编号]-[固件名称]命名。

## 固件列表

固件的目录命名规则为 [固件编号]-[固件名称] 。

- [001-base](001-base):基本固件



# 调试

注意:若未特殊说明，本章节内容支持所有固件。

除了使用LuaTools调试，固件还支持使用USB串口的用户串口（通常是第一个由USB枚举的串口）进行调试。

推荐使用支持终端转义序列的软件（如[putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/)、minicom）打开USB用户串口(参数可使用串口软件的默认参数)。

![usb_shell_debug](usb_shell_debug.png)



