#include<stdio.h>
int func4(int x, int y, int z){
    int res = z - y;
    int a = res;
    a = (a >> 31) & 1;
    res += a;  // res += res > a;
    res >>= 1;
    a = res + y;

    if (a <= x){
        res = 0;
        if (a >= x) return res;
        y = a + 1;
        res = 2 * func4(x, y, z) + 1;
    }
    else {
        z = a - 1;
        res = func4(x, y, z) * 2;
    }

    return res;
}

int func42(int x,int y,int z){	
	int res= z - y;
	unsigned int temp = res;
	temp = temp >> 31;//取出z-y的最高位
	res += temp;
	res >>= 1;//算术右移1位
	int var = y + res;
	if(x < var){
	   z = var -1;
	   res = func4(x,y,z);
	   res *= 2;
	   return res;
	}else{
	   //400fe4的跳转逻辑
	   res = 0;
	   if(var >= x){
	      return res;
	   }else{
	      res += 1;
	      return 2*func4(x,res,z)+1;
	   }
	}
}	  

int main(){
   printf("%d",func42(7,0,14));
}