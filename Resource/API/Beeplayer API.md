## Beeplayer API

**注意：所有的`CoreTalker`类不建议暴露在任何非`Wapper`类之外使用，`CoreTalker`类只用于于核心库形成交互，实现底层的操作。**

### Decoder   核心封装解码器

- `Decoder`类是一个封装了解码功能的简易`CoreTalker` ，`Decoder`类由`Player`类操作。

API :
```cpp
void InitDecoder(const std::string& FilePath);
```

 - 此函数接受一个字符串文件地址，调用`miniaudio`库中对应的`load`函数加载音乐文件到内存，实现解析。但请注意，在Windows中，如果文件目录存在非英文字符，您应该要知道此函数会对字符串转换(u8str -> u16str)

   

### Device 核心封装设备
- `Device`类是一个封装了初始化播放设备，初始化播放设备配置的`CoreTalker`，`Device`类由`Player`类操作。

- 

   
