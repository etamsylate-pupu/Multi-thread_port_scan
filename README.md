## description

多线程端口扫描程序；多线程并发服务器模型；Linux环境

## function

1. 能至少扫描5个ip地址；
2. 针对每个ip地址，开设100个线程同时对其进行扫描；
3. 如果端口打开，使用函数getservbyport获取其服务名，在屏幕上打印：ip port servername，如果是未知服务，则屏幕显示：ip port unknown