# Oak-16
========  
![Oak-16](icon.ico)
## 1. Oak-16是什么?
Oak-16是由*哔哩哔哩-一只喜欢mc的甜瓜*制作的**十六进制编辑器**,使用 ***C++*** 编写.  
>尽管大部分来自[百度AI](https://chat.baidu.com)  
Oak-16是开源的免费软件.  
>不然怎么会在Github,还是public仓库
## 2. Oak-16怎么使用?
1. 使用编译器编译
    >不推荐使用这种方法.
    如果使用这种方法,请下载[MinGW-w64](https://github.com/niXman/mingw-builds-binaries/releases),选择最新版本,解压  
    >注意,i686是32位,x86_64或ARM64才是64位
    添加环境变量,或使用完整路径.使用这种办法检查  
    打开Powershell.exe  
    >添加环境变量 gcc -v;g++ -v;gdb -v  
    >使用完整路径 path/gcc -v;path/g++ -v;path/gdb -v
    确定没有问题,就使用`cd ${Oak-16x路径};g++ main.cpp -o Release/main.exe -lgdi32 -lcomdlg32 -fexec-charset=GBK -finput-charset=UTF-8 -luxtheme`编译,运行输出文件.  
2. 直接运行
    - 运行Relsase版本
    进入`./Release`目录,运行程序
    - 运行Debug版本
    进入`./Debug`目录后,有两种方式  
    直接运行或gdb调试
## 3. Oak-16历代功能有什么?
- v_1.1.0
    此版本界面和功能很简陋,仅支持手动打开文件和查看.`不支持下载`
- v_2.1.0
    此版本更换了图标和其它功能,但是存在少量问题.
- 古菇顾
    还得等
## 4. Oak-16适合哪些系统版本?
*Windows10,Windows11*