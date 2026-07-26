#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int main()//猜数字小游戏：生成1-100之间的随机数，使用键盘录入去猜，猜中为止
{
	printf("猜数字小游戏：生成某个范围内的数字，使用键盘录入去猜，猜中为止\n\n\n");
	while (1)
	{
		int x, y, a, b;
		srand(time(NULL));
		printf("请输入猜的范围(输两个数) ");
		scanf("%d %d", &x, &y);
		if (x > y)
		{
			//printf("错误请重新输入。。。\n");
			continue;
		}
		a = rand() % (y - x + 1) + x;
		printf("请输入一个数，在%d～%d范围内\n", x, y);
		while (1)
		{
			scanf("%d", &b);
			if (b == 14514)
			{
				printf("答案：%d\n", a);
				continue;
			}
			if (b == a)
			{
				printf("对了！！！太NB了！！！\n");
				printf("\n～～～～～～～～～～～～～～～～～～～～\n");
				printf("----------------------------------------\n\n\n\n");
				break;
			}
			else
			{
				printf("错错错，再试一次吧（看答案输入14514）\n");
			}
		}
		printf("继续游戏按1\n结束游戏按0\n");
		int play;
		scanf("%d", &play);
		if (play == 0)
		{
			printf("已退出\n");
			break;
		}
	}
	return 0;
}