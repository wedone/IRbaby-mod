#ifndef IRBABYHA_H // 头文件保护宏
#define IRBABYHA_H // 头文件保护宏
#include <WString.h> // 引入 WString 库
#include "../lib/Irext/include/ir_ac_control.h" // 引入空调控制库

// 函数声明：返回空调状态
void returnACStatus(String filename, t_remote_ac_status ac_status); 

// 函数声明：注册空调
void registAC(String filename, bool flag); 

#endif // 头文件保护宏结束