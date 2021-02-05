# vip-music-decoder-gui

[原CLI解码工具](https://github.com/NaviHX/vip-music-decoder)的图形界面版本

## 构建

利用 `Qt 6.0.0` 绘制GUI  
请安装Qt的动态链接库

ncmdump依赖于[taglib](https://github.com/taglib/taglib)  
如需使用ncm转换功能,请自行安装

本项目使用`qmake`进行构建

### For Linux

```bash
git clone https://github.com/NaviHX/vip-music-decoder-gui.git
cd ./vip-music-decoder-guui
chmod +x ./build.sh
./build.sh
```

### For Win10

确保成功安装`Qt`,并已经添加到环境变量  
在项目目录下打开`powershell`,逐步执行`build.sh`的语句

> 如果没有安装`taglib`将会导致`ncmdump`的编译失败

## 如何使用

点击菜单栏中的`File -> Open *** File`,选择文件即可
