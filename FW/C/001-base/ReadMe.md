# 说明

ExtBoard-002固件源代码。

# 硬件资源

## 指示灯

| GPIO   | 说明         | 备注 |
| ------ | ------------ | ---- |
| GPIO26 | STATUS灯     |      |
| GPIO27 | NET_STATUS灯 |      |



# 脚本说明

- [build.bat](build.bat):构建固件，构建完成后可在[sdk](../sdk)目录中找到相应固件。
- [build_RNDIS.bat](build_RNDIS.bat):构建固件(启用RNDIS)，构建完成后可在[sdk](../sdk)目录中找到相应固件。
- [clean.bat](clean.bat):清理已编译的文件(也包括[sdk](../sdk)中的其它固件)。

