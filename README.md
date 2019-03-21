# gaotianming-Redis
一步一步学习Redis
先回想一下C语言的字符串：
          脑补图片……
Redis中有自己的字符串（simple dynamic string）结构
struct sdshdr{
    int len;
    int free;
    char buf[];
}
Redis只有在字符串不需要修改的时候采用C字符串，其余情况都采用SDS。
为什么Redis不沿用C语言的字符串呢？
 1.C语言不记录字符串长度
    C语言的字符串不记录自身长度，想要知道一个字符串的长度就必须遍历一遍字符串，复杂度为O(N)，而Redis的字符串同样使用命令STRLEN的时候，复杂度为O(1)，只需要查看len这个属性就可以；
    
2.杜绝缓冲区溢出问题
    当字符串需要修改（加入元素）时，调用的API首先会检查SDS的free是否足够分配，如果不够的话会自动扩展。（eg.strcat()函数，当需要拼接一个字符串的时候，如果不手动的去申请空间的话，或者忘记申请空间的话，这时新拼接上的字符串就会接到这个字符串的后边，即使空间不够，他也会向后覆盖，会造成安全问题）；
    
3.减少字符串扩容时内存重新分配的次数
    C语言中，字符串N次扩容，必然会有N次的内存分配；Redis根据自己特殊的数据结构和分配策略，可以保证最坏情况下，会有N次内存分配。分配策略主要是：
    空间预分配：
        当len<1M时，free = len（修改后的值）；当len>=1M时，free = 1M。                                                                           惰性空间释放：
        当SDS发生截断或者其他导致空间缩小时，Redis不会释放自己的空间，而是将其记录到free中去。（可能你会有这样的疑问，那样不会造成空间浪费么？Redis设计者也考虑到了这个问题，所以提供了API，保证我们可以去人为的真正释放free的空间）
        
4.二进制安全
    说到Redis的String的特色，不得不提这个String是可以存储非文本数据的，包括视频，音频，图片等。这就是因为SDS的二进制安全性，SDS并不是像传统的C字符串(字符数组)一样，而SDS常被称作字节数组，显然，SDS采用以字节为单位的形式存储数据，而最后的'\0'也是一个字节，这样数据怎么样存入的，取出来的时候还是怎么样的，因此是二进制安全的；
    
5.兼容部分C字符串的函数
    这也是为什么SDS这个结构里的len是buf中真正元素的个数，而buf中最后还添加一个'\0'的原因，例如printf(),strcmp()等函数，可以直接使用。


众所周知，Redis采用C语言编写，而C语言没有list这种数据结构。所以Redis的list长这样

struct listNode
{
    struct listNode* prev;
    struct listNode* next;
    void* value;
};

typedef struct list
{
    listNode* head;
    listNode* tail;
    unsigned long len;

    void* (*dup)(void* ptr);
    void （*free)(void* ptr);
    int (*match)(void* dest,void* src);
}list;

这里不得不说Redis的设计很精妙。如果让我(菜鸡的代表)实现，我觉得只有一个listNode结构就够了，要不咋说人家是大佬呢。

首先，看那三个函数指针。这就使得我们的链表具有多种形态，

dup用于复制链表节点所保存的值；                                                                                                           free用于释放链表节点所保存的值；                                                                                                           match用于对比链表节点所保存的值和另一个输入值是否相等。

解释一下dup就是复制一个节点的函数，因为有指针，所以复杂度O(1);free就是删除一个节点的函数O(1)；match就是一个比较节点value的函数。

这样设计有什么好处呢？
双端：链表节点有prev和next指针，获取头节点和尾节点的复杂度都是O(1)

无环：表头节点的prev和表尾节点的next都指向NULL

有表头和表尾指针：查表头和表尾元素的复杂度O(1)

带链表长度计数器：可以直接获取链表的长度O(1)

多态：因为链表节点的value是void* 类型，因此这个数据可以是各种类型的数据。dup、free和match可以为你存入的数据类型量身定制函数，因此这个数据结构是具有多种形态的。



