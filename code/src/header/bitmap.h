//copy form https://blog.csdn.net/jenie/article/details/106741221
#ifndef MYVM_BITMAP_H
#define MYVM_BITMAP_H

/**
 *存储bitmap的结构体
 *存储的顺序从左至右
 **/
struct _Bits;
typedef struct _Bits *bits;

/**
 *获得bitmap
 *@length bitmap的长度
 *@return 所有位都初始化为0的bitmap
 */
bits bit_new(unsigned int length);

/**
 *销毁一个bitmap
 **/
void bit_destroy(bits bit);

/**
 *获得y一个bitmap的长度
 *@bit 需要获得长度的bitmap
 *@return bit的长度
 **/
unsigned int bit_length(bits bit);

/**
 *设置bitmap中相应位置的值
 *@bit 待设置的bitmap
 *@pos  需要设置的位置
 **/
void bit_set(bits bit, unsigned int pos, unsigned char value);

/**
 *设置bitmap中相应位置的值
 *@bit  待获取的bitmap
 *@pos  获取的位置
 **/
char bit_get(bits bit, unsigned int pos);

#endif //MYVM_BITMAP_H
