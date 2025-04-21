# Beeplayer  

*<h5 align="right">Thanks to music that makes the world so beautiful. :)</h5>*

<h3 align="center">A minimal cross-platform music player based on miniaudio and C++.</h3>

> [!IMPORTANT]  
> 在目前这个版本，Beeplayer并不能以默认方式运行(即为直接从命令行运行), 
> 不过可以通过修改源代码中的rootPath来正常播放,
> 在下一个版本应该加入自定义检索目录。  
> In this current version,Beeplayer cannot operate as expected by default from the command line.
> However,normal playback can be achieved by modifying the rootPath in the source code.  
> The next version should include a custom search directory.

- [ ] 用户目录接收
- [x] 自动下一首, (不过依旧有一些潜在问题 TuT)
- [ ] 修复wav/mp3文件相互切换会有高频噪声的bug
- [ ] 修复在不同曲目下SampleRate不同而产生时间及音调出现问题的bug
- [ ] 优化逻辑，降低耦合
- [ ] 在第一个正式版本实现隔离功能，分类编写 
- [ ] 修复多线程的已知bug
- [x] 修复目录迭代器的已知bug
- [ ] 音量控件
- [ ] 加入UI


- License: [MIT No Attribution](LICENSE)  
 [miniaudio](https://miniaud.io/) provides us with two versions of the License, and I use MIT No Attribution.  
For more information and the content of the License, refer to the LICENSE file.  
