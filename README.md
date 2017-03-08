# LeO - A FLexibLe FiLesystem for Data Center
This is a simulation experiment to verify a design of multi-datacenter file system, which is based on Othello.

The baseline is consistant hash method.

In each directory has a more detailed README.

## Author
Yueqi Chen (Yueqichen.0x0@gmail.com)

## Acknowledge
Thanks a lot to [sdyy (Ye Yu)](https://github.com/sdyy1990) for his outstanding work on Othello as well as his selfless help.

Thanks a lot to [PASA Lab of Nanjing University](http://pasa-bigdata.nju.edu.cn/) for providing metadata

## Advantages
Compared with baseline, Lazy Dictator enjoys following vantages:

1. Datacenter-aware. Lazy Dictator will automatically choose the geo-closest datacenter to store file or directory based on client's location.  What's more, Lazy Dictator will try its best attempt to avoid cross-datacenter data flow to optimize performance and lower utilize of bandwidth.

2. Less dataflow. All Unix-like operation , except read, write and copy, will not lead to any data flow between servers or even datacenters. In other words, what we need to do to reponse client's request is to modify directory file in several servers, of which the number is well-controlled.

3. Scalable. To add server will not lead to data flow in large volume.

4. Fault-torelance. When several severs abruptly down, this condition will not results in fuctuation in the file system.

## About the name
The two primary characteristics of this design is its less dataflow, datacenter-aware. Therefore, I temporarily name this design as "Lazy Dictator".
