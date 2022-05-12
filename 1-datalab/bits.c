/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 *    sun
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 * 用按位与、按位取反实现按位异或
 */
int bitXor(int x, int y) {
  return ~((~(x&~y)) & (~(~x&y)));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 *   返回最小的补码整数,即0xA0000000 
 */
int tmin(void) {

  return 1<<31;

}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 *  判断x是否为最大的补码整数，是的话则输出1，否则输出0
 *  最大的补码整数为0x7fffffff
 *  若为最大值，则第一位为0，+1后第一位变为1,即x + 1 = 0x80000000
 *  因此 x & (x + 1) = 0
 *  但要注意 -1的补码为0xffffffff,-1+1后产生0x00000000，此时也是0，需要再判定x不是-1
 *  同时 0与0+1的与也为0
 *  但是只有 x = 最大补码和 x = -1这两种情况下，x与x+1的各位不同(因为这两个值刚好是两次进位产生的时候)
 *  只有这两种情况下，~(x ^ (x + 1)) = 0,此时取非即可
 *  但还是要去掉 x = -1的情形，x = -1时，x + 1为0，而 x为最大补码时，x+1不为0,将x+1通过!!转化为0和1
 *  然后再进行&运算即可
 *  x为最大补码时，为1与1进行&   x=-1时，是1与0进行&运算  
 */
int isTmax(int x) {
  return (!~((x ^ (x+1)))) & (!!(x + 1));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 *  判断一个二进制数奇数位是否都为1，是的话返回1,否则返回0
 *  0xAAAAAAAA是所有奇数位为1，偶数为为0的数，因此可以让x与其进行&运算，取出x的所有奇数位，然后再与0xAAAAAAAA判断是否相等
 *  如果相等，那么输出1，否则输出0
 *  判断使用异或操作,异或时，若x的所有奇数位与0xAAAAAAAA相等则输出0，此时需要取非操作
 */
int allOddBits(int x) {
  return !((x & 0xAAAAAAAA) ^ 0xAAAAAAAA);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 * 取反操作，即求补数操作，各位取反+1 
 */
int negate(int x) {
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 * 判断是否是ascii数字字符，只需要比较最后8bit即可，如果前面的bit位不等于0，则直接输出0
 * 因此,用0xFFFFFF00进行&运算，再取非，如果前6字节不为0，则取非值为0，而若为0，则取非后值为1
 * 此时再判断x是否处于 0x30-0x39这个区间即可,需要分两部分，判断倒数第二个字节是否为3,以及最后一个字节是否为0-9之间(即判断是否小于9)
 * 最后三部分进行&操作，因为要同时满足
 * 难点在于最后一部分如何判断小于9，由于最后一个字节最大可表示15，再大则会产生进位影响前面的字节，
 * 因此我们可以让最后一个字节的数字与7进行相加，再判断是否影响了前面的字节
 * 我们用0x00000010 & ((0xF & x) + 7) 取出第5bit，再右移四位，判断是否为1，如果为1，则说明产生了进位，说明最后的大于9
 */
int isAsciiDigit(int x) {
  return !(0xFFFFFF00 & x) & !((0xF0 & x)^0x30) & !((0x00000010 & ((0xF & x) + 6)) >> 4);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 * 需要判断x是否为true，即直接对x取非再取非即可转换为bool,然后再决定取y还是取z
 * 如果x为0，应该输出z
 * 如果x为1，应该输出y
 * 综上，x --> ~(!!x)+1,利用补码性质，1的补码为0xffffffff,0的补码为0，与y进行&运算实现 x = 0时，(~(!!x)+1)&y = 0,x=1,该公式等于y
 * x --> ~(!x)+1，实现x = 1时，生成全0，x = 0时，生成全1序列,然后与z进行 & 运算，实现x = 0时，(~(!x)+1)&z = 1，x=1,该式子等于0
 * 将这两部分相加
 */
int conditional(int x, int y, int z) {
  return ((~(!!x) + 1) & y) + ((~(!x) + 1) & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 * 转为：x-y <=0,先求x + y的补,即 x + (~y + 1)
 * 然后用这个式子与0进行异或判断是否为0
 * 再用这个式子右移31位，判断符号是否为1
 * 需要考虑溢出 
 */
int isLessOrEqual(int x, int y) {
  //x-y的值
  int res = (x + (~y + 1));
  //判断是否为0,当res为0时，返回1，符合要求，当res!=0,该部分为0，
  //看下一部分判断是否小于等于0
  int isZero = !res;
  int isLess = (res >> 31) & 1;
  //如果isLess为1，则说明是负数，即小于，当其最高为0时，需要判断是否发生了正溢出
  //x > 0 ,-y >0 判断是否正溢出
  //因此首先判断x与-y是否同号，再判断res是否异号，若异号则正溢出，此时应该去掉这种情况
  int isOver = (!((x >> 31 & 1) ^ (!(y >> 31 &1)))) & (((x >> 31 & 1) ^ (res >> 31 &1)));
  //该句是错误的，这是因为0x8000000的补码为其本身，-y应该是个正数，需要特殊对待
  //因此采用上一句，取y的符号位并对其取反来得到-y的符号位，而不采用补码的形式
  //int isOver = (!((x >> 31 & 1) ^ ((~y + 1) >> 31 &1))) & (((x >> 31 & 1) ^ (res >> 31 &1)));
  return (isOver ^ isLess) | isZero;
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 * 实现逻辑取非运算符
 * 判断x是否等于0,等于0的话将其转换为1，否则转换为0
 * 如何把不为0的数转为-1呢,为0的保持不变
 * 最后将结果+1即可
 * 此时需要考虑 0的补码为0,最小值的补码为其本身,其他数的补码与自身按位或可以得到-1
 * 因此 (x | (~x + 1)),此时只有最小值的情况下，为0x80000000 | 0x80000000 = 0x80000000
 * 因此再算术右移31位，产生全1序列
 */
int logicalNeg(int x) {
  return ((x | (~x + 1)) >> 31) + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 *  返回x的二进制补码所需要的最小的bit位，例如12为5位，不是四位是因为补码形式含有负数
 *  由于补码取反后所需的位数和补码所需的位数是一样的，因此先统一转为正数
 *  然后判断最左边的1，再加上符号位即可
 */
int howManyBits(int x) {

  int b16,b8,b4,b2,b1,b0;
    int flag=x>>31;
    x=(flag&~x)|(~flag&x); //x为非正数则不变 ,x 为负数 则相当于按位取反
    b16=!!(x>>16) <<4; //如果高16位不为0,则我们让b16=16
    x>>=b16; //如果高16位不为0 则我们右移动16位 来看高16位的情况，如果为0,则x不移动
    //下面过程基本类似
    //看x原来的16-24位的情况
    b8=!!(x>>8)<<3;
    x >>= b8;
    //看x原来24-28位的情况
    b4 = !!(x >> 4) << 2;
    x >>= b4;
    b2 = !!(x >> 2) << 1;
    x >>= b2;
    b1 = !!(x >> 1);
    x >>= b1;
    b0 = x;
  return b0+b1+b2+b4+b8+b16+1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 * 输出2*f的二进制表示
 */
unsigned floatScale2(unsigned uf) {
  unsigned s = (uf >> 31) &1; //取符号位
  unsigned e = (uf >> 23) & 0xFF;//取指数部分
  unsigned f = (uf & 0x7FFFFF);//取表示f的23位,f需向右移23位再乘以2的e次方才可以
  //当e为全1时，表示特殊值，全0时表示非规格化值，非0非255时，表示规格化值
  if(e == 0xff){
     return uf;
  }
  if(e){
    //e不为全0，表示规格化值,f*2相当于e+1
    e++;
    return (s << 31) | (e<<23) | f;
  }
  //e为0，表示非规格化值,此时e = 1-bias,e为固定的,2*f相当于f左移一位
  f <<= 1;
  return (s << 31)|f;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 *  将float转为int,需要考虑向偶舍入
 *  先找到浮点数的f和e部分，然后判断e-bias是否大于23，如果大于23，则需要向左移动，如果刚好为23，则f不变,将f所表示的二进制当作int来读取即可
 *  如果小于23，说明f需要向右移动
 *  这是因为将f部分看作整数,意味着f*2^23,因为在浮点数表示中,f这23位是小数点以后的数,相当于f1*1/2 + f2*1/4 + .... + f23*2^-23(假设f表示为f1f2f3..f23)
 * 
 */
int floatFloat2Int(unsigned uf) {
  unsigned s = (uf >> 31) &1; //取符号位
  unsigned e = (uf >> 23) & 0xFF;//取指数部分
  unsigned f = (uf & 0x7FFFFF);//取表示f的23位
  int E = e-127;
  if(E>=31){
     //E>31，说明f超出int的范围
     return 0x80000000u;
  }
  if(E < 0){
    //E小于0，表示f为0.x,转int后为0
    return 0;
  }
  //规格化数，需要加1
  f |= 1<<23;
  if(E < 23){
    //此时f右移
    f >>= (23-E);
  }else{
    //左移
    f <<= (E-23);
  }
  if(s){
    return -f;
  }
  return f;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 *   计算2.0的x次方,太小返回0,太大返回+INF
 *   X为2.0的指数部分
 *   2.0用浮点表示为:s为0，e的部分为127+1，f的部分为0
 *   2.0的x次方，在e部分加上x即可，但只有x>=-126(此时e部分才不为0)的时候才成立
 *   当x<-148时,2.0^x的e部分为全0(此时为2^-23 * 2^-126)
 *   当x > 127时，2.0^x的e部分为全1
 */
unsigned floatPower2(int x) {
    if(x > 127){
      //此时，到达足够大，返回+inf,即让e的部分为全1
      return 0xff << 23;
    }
    if(x < -149){
      //足够小
      return 0;
    }
    // 判断是否在规格化的范围内
    // E = e - Bias → e = E + Bias,  E = x, Bias = 127.
    if(x >= -126){
      unsigned exp = x + 127;
      return exp << 23;
    } 

    // 非规格化的数，因为x < -126，所以修改frac中的值即可。 
    // 对于非规格化的数，E = -126
    // 由于x处于-126到-148这个区间，因此e部分必须为0，此时指数部分才能得到最小的-127,然后再根据f部分1所在的位置，最多可以有-23位
    // 因此最多可为2的-148次方
    // 设 f = 2 ^ (a - 23)。 a代表1所在的位置索引，0（右） → 22（左）
    // x = -126 + (a - 23)
    // a = x + 126 + 23 = x + 149 (此时 -148<= x <= -126)
    return 1 << (x + 149);
}
