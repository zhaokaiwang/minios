#elf文件的学习
elf文件大概的分为elf header, program header table, sections, sections header table.
##elf header
elf header包含的内容如下：

*  e_version 确定文件的版本
*  e_entry程序的入口地址
*  e_phoff program header table在文件中的偏移量（以字节计数）。
*  e_shoff section header table在文件中的偏移量。
*  e_flags 对于ia32来说，这项为0
*  e_phentsize program header table中每一个条目的大小
*  e_phnum program header table中条目的个数
*  e_shentsize SECTION header table中每一个条目的大小
*  e_shnum section header table中多少个条目
*  e_shstrndx包含节名称的字符串表是第几个节

## program header
program header描述的是系统准备程序运行所需的一个段（segment）或者其他信息。

* p_type当前program header段类型
* p_offset第一个字节在文件中的偏移
* p_vaddr段的第一个自己在内存中的虚拟地址
* p_paddr在物理地址定位相关的系统中，此项为物理地址保留
* p_filesz段在文件中的长度
* p_memsz段在内存中长度
* p_flags与段相关的标志
* p_align根据此项来确定段在文件中以及内存中如何对齐

#从loader到内核
加载内核到内存这一步和引导扇区的工作非常的相似，只是处理内核时我们需要根据program header table中的值把内核相应的段放在正确的位置。在这里orange处理的方式是先吧内核放在内存中然后在保护模式下移动他的位置。
