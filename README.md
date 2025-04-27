# Beeplayer  

*<h5 align="right">Thanks to music that makes the world so beautiful. : )</h5>*

<h3 align="center">A lightweight and cross-platform music player based on miniaudio and C++.</h3>  

> [!IMPORTANT]  
> Beeplayer 预构建版本发布啦！  
> 在PreBuild目录下，你可以找到编译好的可执行文件。  
> 不过这只是一个预先构建版本，你需要去仔细阅读文件夹中的README文档。  
> Beeplayer Prebuilt Version is Out!  
> In the PreBuild directory, you can find the compiled executable files.  
> However, this is just a prebuilt version, and you need to carefully read the README in the folder.

- [x] 用户目录接收
- [x] 自动下一首, (不过依旧有一些潜在问题 TuT)
- [x] 修复wav/mp3文件相互切换会有高频噪声的bug
- [ ] 修复在文件格式不同的情况下**貌似**会因为双缓冲而卡顿
- [x] 修复在不同曲目下SampleRate不同而产生时间及音调出现问题的bug
- [x] 优化逻辑，降低耦合
- [x] 在第一个正式版本实现隔离功能，分类编写 -> 预构建版本也算正式版本(bushi)
- [x] 修复多线程的已知bug
- [x] 修复目录迭代器的开始与结束的切换bug
- [ ] 音量控件
- [ ] 加入UI
- [x] 加入Log系统 -> 有些问题，需要修改一下
- [ ] 重写错误处理(事实上根本没有 :/)


- License:  
 Beeplayer complies with the [MIT No Attribution](LICENSE) License, For more information, refer to the [LICENSE](LICENSE) file.  

 > [miniaudio](https://miniaud.io/) has offered us two licenses, and I have chosen the MIT No Attribution one.  
 > Actually, I originally intended this project to follow this particular license.

