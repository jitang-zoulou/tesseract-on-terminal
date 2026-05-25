[简体中文](README-CN.md)  [English machine-translation](README.md) [文言 机译](README-CC.md)  

**🤔这是什么**  
这是一个可以在终端上渲染四维超立方体的cpp小程序  

**🧐它看起来怎么样**  
受限于屏幕，只能展示它在三维的切片并投影到二维屏幕中  
它是由8个立方体组成的，为了能充分演示运动位置，必须用12种颜色使每条边的对边或邻边颜色不重复  

**😋如何使用它**  
如果你是winxp以上的用户，请下载并安装小熊猫c++，然后在本仓库的github网页端把tesseract.cpp中的代码copy到小熊猫c++里面点击运行小三角即可  
[从这个链接下载小熊猫c++](https://wwe.lanzoui.com/b01os0mwd 分享码：f0tp)  

如果你是macos或其他非linux发行版系统的用户请自行摸索如何编译它  

如果你是linux发行版用户，那么以下将以debian系为例进行编译操作演示  
```zsh
git clone https://github.com/jitang-zoulou/tesseract-on-terminal  
apt update && apt install clang++ -y  
#请确保clang++支持c++11标准  
cd tesseract-on-terminal  
clang++ -pthread -O3 -march=native -ffast-math -flto=auto tesseract.cpp  
./a.out  
```
**🥳最终效果**  
![效果图](tesseract.gif)
