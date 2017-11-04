#Makefile
Makefile就是Linux的编译环境。
“#”开头的是注释， = 用来定义变量。

**target : prereqquistites**

**command**
 上面的形式代表了两层意思：

 1. 要想达到target, 需要先执行命令command
 1. target依赖与prerequisite， 当prerequisite 中至少有一个文件比 target 文件新时，command才会被执行
 
 **$ (asm) $(asmflags) - o $@ @<**的含义
 
 * $@ 代表 target
 * $< 代表prerequisite 的第一个名字
 
 everything all， clean是三个动作名称。
 .PHONY 代表它后面的名字并不是一个文件而是一种行为的标号，make 程序是由果寻因，首先要看生成什么，再找生成他需要的条件。  