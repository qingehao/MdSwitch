![image-20231217234235291](E:\freetime_works\JlinkSwitch\images\image.png)

# MdSwitch

一款方便嵌入式工程师调试的 三路SWD及UART信号多路复用器

### 它有什么作用

<img src="E:\freetime_works\JlinkSwitch\images\image1.png" alt="image-20231217224134746" style="zoom: 33%;" />

平时我们可能会面临着调试多个设备的场景，这种场景下意味着我们需要来回插拔不同设备的SWD或者UART的接线，人工插拔线显得不是那么优雅

MdSwitch旨在为这种场景下提供一种优雅的免人工接线的方案，注意：**其定位只是一个 Switch**。具体功能框图如下：

<img src="E:\freetime_works\JlinkSwitch\images\功能框图.png" alt="image-20231217230425459"  />

总的来说，这是我最近为了解决个人痛点设计制作的一个调试辅助设备，最终完成度比较高所以也就顺便开源分享出来了。

> 其中按键处理是基于我之前开源另一个项目MdButton，关联仓库地址在这里：
>
> [qingehao/MdButton (github.com)](https://github.com/qingehao/MdButton)

### 关于信号切换方案

​        这是整个设计花时间最多的一个地方，因为我想要保持最终PCB的宽度限制在JLINK的IDC简牛座的宽度范围内，也就是25.4mm。所以限制可供

我选择芯片方案的封装大小。最先想到是4052系列的模拟开关

![image-20231217231629122](E:\freetime_works\JlinkSwitch\images\image2.png)

但是其支持的信号带宽似乎不足以支持SWD的信号频率，目前市面上JLINK普遍支持的最高是12M~25M的速率。逛了很久的立创商城，发现了一款：

WAS4735Q。最高支持1.2GHz的带宽

![image-20231217232337080](E:\freetime_works\JlinkSwitch\images\image3.png)

### 

##### 关于电池

​       关于电池的那一部分，其实是有必要的，JLINK接口的定义，并不向外输出电源，但是板子的工作需要电源，意味着必须额外具备电源供给能力。
诚然，板上的TypeC接口具备供电能力，但是那仍然需要接着一根线，即使我们不需要用到串口。所以便加入了电池，为了具有边冲边用以及即使电池过放也可使用的特性，我选择了一款具备动态电源路径(DPPM)功能的charger芯片, 或者直接使用MOS做一个电源优先级切换电路也可。
但显然DPPM的加入，使得产品好像的电路方案看起来更加合理与优雅，即使成本比较贵。

​     另外，电池相关的电路是可以旁路掉的 短接R10，电池相关功能即被禁用，其相关芯片也可不焊接

![image-20231217233542765](E:\freetime_works\JlinkSwitch\images\image4.png)

##### 其他想起来再说吧~

#### 特别感谢

感谢DJI云台电子小伙伴们的试用，让我知道这玩意确实好用！

#### **最后，喜欢的话可以请我喝杯可乐~**

> 请注意，本项目是**GPL协议**，不支持任何形式的商业产品化（大家可以自行制作）