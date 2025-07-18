# Data-Structure-and-Algorithm
a cpp code repo for DSA-teaching

数据结构与算法的c++代码库，做了一些设计，希望在robustness和cleanness之间取得一个平衡点

注意：
    使用哨兵节点的数据结构目前的实现都要求存储的值的类型能够默认构造；
    修复的方法是让节点类型继承一个不持有值的基类，并哨兵节点为基类类型；
    但是这会很大程度增加代码的“噪声”，所以本库不修复这个问题

代码注释待完善
