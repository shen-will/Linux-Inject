# Linux-Inject
ELF file inject for x86 Linux

ELF文件感染的几种常见方式 
* text段扩充  
* data段扩充 
* text段填充

通过寄生代码调用宿主进程libc符号/加载动态库实现
