//头文件
#include <reg52.h>
#include <ctype.h>		//isdigit()函数
#include <stdlib.h>		//atol()函数
//宏定义
#define uint unsigned int
#define uchar unsigned char
#define ulint unsigned long
//位定义
sbit LS_WEA = P1^0;
sbit LS_WEB = P1^1;
sbit LS_WEC = P1^2;		//74HC138译码器位定义
sbit LED = P1^3;		//LED
sbit BUZZER = P1^4;		//蜂鸣器
//全局变量
uchar handle_x[9], handle_y[9];		//键盘返回字符存放数组，用于存放键盘返回的字符
uchar code display[] = {
		0x3f,  //  0
		0x06,  //  1
		0x5b,  //  2
		0x4f,  //  3
		0x66,  //  4
		0x6d,  //  5
		0x7d,  //  6
		0x07,  //  7
		0x7f,  //  8
		0x6f,  //  9
		0x00   //  全灭
};//字符码表，共阴字段
uchar display_cache[] = {
				10,
				10,
				10,
				10,
				10,
				10,
				10,
				10,
				10,
};//显示缓存
//函数声明
void delay(uint z);			  //延时函数
ulint operation(ulint operand_xx,ulint operand_yy,uchar operator_z);
//运算，将运算数x，运算数y，送入函数，返回运算结果
uchar keyscan();					//键盘扫描程序，'@','#'分别表示清零和没有按键按下
void cache(ulint input);	//将输入送入显示缓存
void smg_display();				//显示，共阴极数码管位选显示，配合三极管及138译码器
//主函数
void main()
{
	ulint operand_x, operand_y, final_result;	//运算数x，运算数y，最终结果
	uchar ckey, a = 0, b = 0;		//ckey=键盘返回字符，a，b初值为零，通过循环实现将输入的数字以字符的形式存放至键盘字符存放数组
	uchar operator;					    //运算符
	uchar c, judge_operand_xy = 0;	//循环体标志数c，被运算判断数judge_operand_xy
init:								//goto语句定位标签
	cache(0);					//初始化显示缓存
	smg_display();
	final_result = 0;
	a = b = 0;
	judge_operand_xy = 0;			//初始化
	for(c = 0; c < 9; c++)
	{
		handle_x[c] = '\0';
		handle_y[c] = '\0';
	}								//???
	while(1)
	{
		ckey = keyscan();			//初始化
		if(ckey != '#')				//如果键盘被按下
		{
			if(isdigit(ckey))		//isdigit函数,若参数为阿拉伯数字，则返回TRUE(真),否则返回NULL(0)
			{
				switch(judge_operand_xy)
				{
					case 0:
							handle_x[a] = ckey;			//将键盘返回字符存放至键盘返回字符存放数组
							handle_x[a+1] = '\0';		//字符'\0' : ASCII码为0,表示一个字符串结束
							operand_x = atol(handle_x);	//atol()函数,将键盘返回字符存放数组中的字符串转换成长整型数
							a++;
							cache(operand_x);			//将运算数x送入显示缓存
							smg_display();				//显示
							break;
					case 1:
							handle_y[b] = ckey;			//将键盘返回字符存放至键盘返回字符存放数组
							handle_y[b+1] = '\0';		//字符'\0' : ASCII码为0，表示一个字符串结束
							operand_y = atol(handle_y);	//atol()函数,将键盘返回字符存放数组中的字符串转换成长整型数
							b++;
							cache(operand_y);			//将运算数y送入显示缓存
							smg_display();				//显示
							break;
					default:break;
				}
			}
			else if(ckey == '+' || ckey == '-' || ckey== '*' || ckey == '/')	//如果键盘返回字符为'+','-','*','/'
			{
				judge_operand_xy = 1;//令被运算判断数=1,传递给switch()函数，准备为运算数y赋值
				operator = ckey;		 //保存运算符
				cache(0);					   //清空显示缓存
				display_cache[8] = 10;		//显示缓存为空
				smg_display();				//显示
			}
			else if(ckey == '=')		//如果按键返回字符为'='
			{
				final_result = operation(operand_x,operand_y,operator);	//计算最终结果
				cache(final_result);		//将最终结果送入显示缓存
				smg_display();				//显示
				while(1)					//计算结束等待清零按键按下
				{
					ckey = keyscan();
					if(ckey == '@')			//如果清零键被按下
						goto init;			//开始跳转
					else					//如果清零键没被按下
						{
							cache(final_result);
							smg_display();		//继续显示结果
						}
				}
			}
			else if(ckey == '@')		//如果按键返回字符为清零
			{
				goto init;			//跳转
			}
		}
		smg_display();				//如果按键没被按下，则显示初始值
	}
}
//子函数
//延时函数
void delay(uint z)
{
	uint x, y;
	for(x = z; x > 0; x--)
		for(y = 120; y > 0; y--);
}
//运算，将运算数x，运算数y，送入函数，返回运算结果
ulint operation(ulint operand_xx,ulint operand_yy,uchar operator_z)
{
	ulint results;
	switch(operator_z)		//判断符号
	{
		case '+' : results = operand_xx + operand_yy;	break;
		case '-' : results = operand_xx - operand_yy;	break;
		case '*' : results = operand_xx * operand_yy;	break;
		case '/' : results = operand_xx / operand_yy;	break;
		default :  break;
	}
	return results;
}
//键盘扫描程序，'@','#'分别表示清零和没有按键按下
uchar keyscan()
{
	uchar skey;                     //按键值标记变量
//扫描键盘第1行
	P2 = 0x7f;						//第1行为0
	if((P2 & 0x0f) != 0x0f)	    //有案件按下
	{
		delay(3);		            //去抖动延时
		if((P2 & 0x0f) != 0x0f)	//仍有按键按下
		{
			LED = 0;				//点亮LED
			BUZZER = 0;			//蜂鸣器响
			switch(P2)			//识别案件并赋值
			{
				case 0x77: skey = '7'; break;	//0111 0111 1行1列
				case 0x7b: skey = '8'; break;	//0111 1011 1行2列
				case 0x7d: skey = '9'; break;	//0111 1101 1行3列
				case 0x7e: skey = '/'; break;	//0111 1110 1行4列
				default:   skey = '#';			  //没有按键按下
			}
			while((P2 & 0x0f) != 0x0f) //等待按键松开
				;
			LED = 1;			//熄灭LED
			BUZZER = 1;		//蜂鸣器静
		}
	}
//扫描键盘第2行
	P2 = 0xbf;			  //第2行为0
	if((P2 & 0x0f) != 0x0f)
	{
		delay(3);
		if((P2 & 0x0f) != 0x0f)
		{
			LED = 0;				//点亮LED
			BUZZER = 0;			//蜂鸣器响
			switch(P2)
			{
				case 0xb7: skey = '4'; break;	//1011 0111 2行1列
				case 0xbb: skey = '5'; break;	//1011 1011 2行2列
				case 0xbd: skey = '6'; break;	//1011 1101 2行3列
				case 0xbe: skey = '*'; break;	//1011 1110 2行4列
				default:   skey = '#';			  //没有按键按下
			}
			while((P2 & 0x0f) != 0x0f) //等待按键松开
				;
			LED = 1;			//熄灭LED
			BUZZER = 1;		//蜂鸣器静
		}
	}
//扫描键盘第3行
	P2 = 0xdf;				//第3行为0
	if((P2 & 0x0f) != 0x0f)
	{
		delay(3);
		if((P2 & 0x0f) != 0x0f)
		{
			LED = 0;				//点亮LED
			BUZZER = 0;			//蜂鸣器响
			switch(P2)
			{
				case 0xd7: skey = '1'; break;	//1101 0111 3行1列
				case 0xdb: skey = '2'; break;	//1101 1011 3行2列
				case 0xdd: skey = '3'; break;	//1101 1101 3行3列
				case 0xde: skey = '-'; break;	//1101 1110 3行4列
				default:   skey = '#';			  //没有按键按下
			}
			while((P2 & 0x0f) != 0x0f) //等待按键松开
				;
			LED = 1;			//熄灭LED
			BUZZER = 1;		//蜂鸣器静
		}
	}
//扫描键盘4行
	P2 = 0xef;				//第4行为0
	if((P2 & 0x0f) != 0x0f)
	{
		delay(3);
		if((P2 & 0x0f) != 0x0f)
		{
			LED = 0;				//点亮LED
			BUZZER = 0;			//蜂鸣器响
			switch(P2)
			{
				case 0xe7: skey = '@'; break;	//1110 0111 4行1列
				case 0xeb: skey = '0'; break;	//1110 1011 4行2列
				case 0xed: skey = '='; break;	//1110 1101 4行3列
				case 0xee: skey = '+'; break;	//1110 1110 4行4列
				default:   skey = '#';			  //没有按键按下
			}
			while((P2 & 0x0f) != 0x0f) //等待按键松开
				;
			LED = 1;			//熄灭LED
			BUZZER = 1;		//蜂鸣器静
		}
	}
	return skey;
}
//将输入送入显示缓存
void cache(ulint input)
{
	uchar i;
	if(input == 0)
	{
		display_cache[0] = 0;				//如果输入为0，则将0送入显示缓存
		i = 1;
	}
	else
		for(i = 0; input > 0; i++)
		{
			display_cache[i] = input % 10;	//取得输入的最低位并送入显示缓存
			input = input / 10;				//丢弃输入的最低位
		}
	for( ; i < 8; i++)
		display_cache[i] = 10;			//无输入或者无显示位则将全灭状态送入缓存
}
//显示，共阴极数码管位选显示，配合三极管及138译码器
void smg_display(void)
{
	uchar i;
	for(i = 0; i < 8; i++)
	{
		switch(i)										//位选
		{
			case(0):
				LS_WEA = 1; LS_WEB = 1; LS_WEC = 1;		//ABC=000,Y7~Y0=1111 1110,个位
			break;
			case(1):
				LS_WEA = 0; LS_WEB = 1; LS_WEC = 1;		//ABC=100,Y7~Y0=1111 1101,十位
			break;
			case(2):
				LS_WEA = 1; LS_WEB = 0; LS_WEC = 1;		//ABC=010,Y7~Y0=1111 1011,百位
			break;
			case(3):
				LS_WEA = 0; LS_WEB = 0; LS_WEC = 1;		//ABC=110,Y7~Y0=1111 0111,千位
			break;
			case(4):
				LS_WEA = 1; LS_WEB = 1; LS_WEC = 0;		//ABC=001,Y7~Y0=1110 1111,万位
			break;
			case(5):
				LS_WEA = 0; LS_WEB = 1; LS_WEC = 0;		//ABC=101,Y7~Y0=1101 1111,十万位
			break;
			case(6):
				LS_WEA = 1; LS_WEB = 0; LS_WEC = 0;	  //ABC=011,Y7~Y0=1011 1111,百万位
			break;
			case(7):
				LS_WEA = 0; LS_WEB = 0; LS_WEC = 0;		//ABC=111,Y7~Y0=0111 1111,千万位
			break;
		}
		P3=display[display_cache[i]];	//显示缓存送入显示
		delay(3);										  //延时
		P3 = 0x00;										//消隐
	}
}