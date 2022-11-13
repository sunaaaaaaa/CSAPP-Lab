#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <getopt.h>
#include "cachelab.h"


struct cache_line{
    int valid;//有效位，表示该缓存是否有效
    unsigned tag;//标记位，用于匹配缓存
    int time;//时间统计
};

//s表示组的幂，E表示每个组有多少行，b表示每个行有多少字节，S表示十进制有多少个组
struct cache_meta{
    int s;
    int S;
    int E;
    int b;
    int hit;
    int miss;
    int replace;
};

char *filepath;
//int s, E, b, S;
//int hit, miss, replace;

struct cache_line **cache;//缓存

void useag(){
    printf("-s<s>:Number of set index bits (S = 2^s is the number of sets)\n");
    printf("-E<E>:Associativity (number of lines per set)\n");
    printf("-b<b>:Number of block bits (B = 2^b is the block size)\n");
    printf("-t<tracefile>: Name of the valgrind trace to replay\n");
}

//初始化缓存
void init_cache(struct cache_meta *meta){
    cache = (struct cache_line **)malloc(sizeof(struct cache_line *) * meta->S);
    
    int i, j;
    for(i = 0;i < meta->S; i++){
        cache[i] = (struct cache_line *)malloc(sizeof(struct cache_line) * meta->E);
    }

    for(i = 0; i < meta->S; i++){
        for(j = 0; j < meta->E; j++){
            cache[i][j].valid = 0;
            cache[i][j].tag = 0xffffffff;
            cache[i][j].time = 0;
        }
    }
}
//释放缓存
void uint_cache(struct cache_meta *meta){
    int i;
    for(i = 0; i < meta->S; i++){
        free(cache[i]);
    }
    free(cache);
}

//地址组成，E，s，b
//使用组相联
void lru(struct cache_meta *meta, unsigned addr){
    unsigned addr_s;
    unsigned addr_E;
    int i;
    int max_time_nused = 0;
    int nused_index = 0;

    addr_s = (addr >> meta->b) & ((-1U) >> (32 - meta->s));//取出地址的s部分，即前s位;
    addr_E = (addr >> (meta->b + meta->s));
    if(addr_s > meta->S){
        printf("group err,input:%d,set:%d,addr:%d\n",addr_s,meta->S, addr);
        return;
    }
    
    for(i = 0; i < meta->E; i++){
        //缓存命中
        if(cache[addr_s][i].tag == addr_E){
            cache[addr_s][i].time = 0;
            meta->hit++;
            return;
        }
    }
    //未命中，寻找可用空间
    for(i = 0; i < meta->E; i++){
        if(!cache[addr_s][i].valid){
            cache[addr_s][i].valid = 1;
            cache[addr_s][i].tag = addr_E;
            cache[addr_s][i].time = 0;
            meta->miss++;
            return;
        }
    }
    //没有空间了，从该组中选择一行替换
    for(i = 0; i < meta->E; i++){
        if(cache[addr_s][i].time > max_time_nused){
            max_time_nused = cache[addr_s][i].time;
            nused_index = i;
        }
    }
    cache[addr_s][nused_index].tag = addr_E;
    cache[addr_s][nused_index].time = 0;
    meta->miss++;
    meta->replace++;
}

void lru_add_time(struct cache_meta *meta){
    int i, j;
    for(i = 0; i < meta->S; i++){
        for(j = 0; j < meta->E; j++){
            if(cache[i][j].valid == 1){
                cache[i][j].time++;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int opt;
    struct cache_meta meta = {0};
    char op;
    unsigned addr;
    int size;
    //解析命令行
    while((opt = getopt(argc, argv, "s:E:b:t:h")) != -1){
        switch (opt)
        {
            case 's':
                meta.s = atoi(optarg);
                break;
            case 'E':
                meta.E = atoi(optarg);
                break;
            case 'b':
                meta.b = atoi(optarg);
                break;
            case 't':
                filepath = optarg;
                break;
            case 'h':
                useag();
                break;
            default:
                printf("please input the -h args, loop up the help!");
                return 0;
        }
    }
    if(meta.s == 0){
        printf("param s is 0, exit!\n");
        return 0;
    }
    meta.S = 1<<meta.s;

    init_cache(&meta);
    //读取输入
    FILE *file = fopen(filepath, "r");
    if(!file){
        printf("cannot open file:%s\n",filepath);
        return 0;
    }

    while(fscanf(file, "%c %x,%d", &op, &addr, &size) > 0){
        switch (op)
        {
            case 'L':
                lru(&meta, addr);
                break;
            case 'M':
                lru(&meta, addr);
            case 'S':
                lru(&meta, addr);
                break;
        }
        lru_add_time(&meta);
    }

    uint_cache(&meta);
    fclose(file);
    printSummary(meta.hit, meta.miss, meta.replace);
    return 0;
}
