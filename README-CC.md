[简体中文](README-CN.md)  [English machine-translation](README.md) [文言 机译](README-CC.md)  

**🤔此谓何物**
夫此物者，乃以西洋“C++”格致之学所撰微型法序也。其用在于术数终端之荧幕，图摹四维超正方体之动静流转。

**🧐其象何如**
然荧幕平阔，止具二维之质；吾人肉眼，亦唯见三维之形。故四维超正方体之神妙，受制于器宇形质，不得不取其三维之剖面，复投影于二维之平缣。是故，视之者如观水中之月、镜中之花，虽非四维之全璧，而其交替流转之真理固在焉。
其体本由八重立方体交错偶合而成，内具十二面、二十四正方之枢机。为了然彰显其旋转运行之轨、相向背之势，特设十二色之彩。错综其用，使凡相向之对边、相接之邻边，其色皆不相侔，斑驳错落，条理不紊。

**😋何以御之**
若夫仁人学子，所御之机乃微软“视窗XP”（Windows XP）以上之统绪，则当涉网海之波，寻“小熊猫C++”之塾。此塾乃当世编纂西洋算学之利器。可循下文之津梁，下载并安置于机杼之内。启其灵机，摹拓本仓之法典（即 `tesseract.cpp` 中之代码），尽数謄写于“小熊猫”之规矩中。旋即点击其运行之朱符（即界面之小三角），则玄方之舞自现于眼前矣。

[自此津梁下载“小熊猫C++”书院](https://wwe.lanzoui.com/b01os0mwd 分享码：f0tp)

若乃操持“苹果”（macOS）或他类非“Linux”法统之机者，其编译御使之方，变幻不一，还望学者独辟蹊径，自行摸索格之。

若乃操持“Linux”法统者，则以“Debian”一系为例。学者当正襟危坐，宣其号令于符台（终端），克期集事，其步法如次：

```zsh
git clone https://github.com/jitang-zoulou/tesseract-on-terminal  
apt update && apt install clang++ -y  
# 务请审视“克朗”铸字御史（clang++）是否笃守C++11之规矩
cd tesseract-on-terminal  
clang++ -pthread -O3 -march=native -ffast-math -flto=auto tesseract.cpp  
./a.out  
```

**🥳终成之象**
![效果图](tesseract.gif)