#pragma once

/*
* So what about cpp as java?
* Cuz cpp lib sucks.
*
* Days later me: Sry about that cpp lib good
*
* Days later me: No cpp lib still sucks
*/

/**
 *                             _ooOoo_
 *                            o8888888o
 *                            88" . "88
 *                            (| -_- |)
 *                            O\  =  /O
 *                         ____/`---'\____
 *                       .'  \\|     |//  `.
 *                      /  \\|||  :  |||//  \
 *                     /  _||||| -:- |||||-  \
 *                     |   | \\\  -  /// |   |
 *                     | \_|  ''\---/''  |   |
 *                     \  .-\__  `-`  ___/-. /
 *                   ___`. .'  /--.--\  `. . __
 *                ."" '<  `.___\_<|>_/___.'  >'"".
 *               | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *               \  \ `-.   \_ __\ /__ _/   .-` /  /
 *          ======`-.____`-.___\_____/___.-`____.-'======
 *                             `=---='
 *          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*/

#include<stdint.h>
#include<Windows.h>

#include<assert.h>

#define ___DEL___

//delete
#define del(arg) delete (arg);

//delete[]
#define dela(arg) delete[] (arg);

//delete with safe check
#define sdel(arg) if((arg)!=nullptr){delete (arg);}

//delete[] with safe check
#define sdela(arg) if((arg)!=nullptr){delete[] (arg);}

//set null
#define nul(arg) (arg)=nullptr;

//addr of
#define addr(arg) (&(arg))

//deref
#define deref(arg) (*(arg))

#define MAX_U8  0xFFu
#define MAX_U16 0xFFFFu
#define MAX_U32 0xFFFFFFFFu
#define MAX_U64 0xFFFFFFFFFFFFFFFFu

namespace FuckingExam {

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
}

