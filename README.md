[简体中文](README-CN.md)  [English machine-translation](README.md) [文言 机译](README-CC.md)  

**🤔What is this**
This is a small C++ program that can render a 4D hypercube in the terminal

**🧐What does it look like**
Limited by the screen, it can only display its 3D slices and project them onto the 2D screen
It is composed of 8 cubes. In order to fully demonstrate the motion positions, 12 colors must be used so that the color of opposite or adjacent edges of each edge is not repeated

**😋How to use it**
If you are a Windows XP or later user, please download and install Xiaoxiongmao C++, then on the GitHub web page of this repository copy the code in tesseract.cpp into Xiaoxiongmao C++ and click the run triangle
[Download Xiaoxiongmao C++ from this link]([https://wwe.lanzoui.com/b01os0mwd](https://wwe.lanzoui.com/b01os0mwd)分享码：f0tp

If you are a macOS user or a user of another non-Linux distribution, please figure out how to compile it yourself

If you are a Linux distribution user, the following will use Debian as an example to demonstrate the compilation operation

```zsh
git clone https://github.com/jitang-zoulou/tesseract-on-terminal  
apt update && apt install clang++ -y  
#Please make sure clang++ supports the c++11 standard  
cd tesseract-on-terminal  
clang++ -pthread -O3 -march=native -ffast-math -flto=auto tesseract.cpp  
./a.out  
```

**🥳Final result**
![Effect image](tesseract.gif)
