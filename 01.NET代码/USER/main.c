#include "git.h"


// 软件定时器设定
static Timer task1_id;
static Timer task2_id;
static Timer task3_id;
extern u8 time25ms;

// 获取全局变量
const char *topics[] = {S_TOPIC_NAME};

// 硬件初始化
void Hardware_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    HZ = GB16_NUM();                                // 字数
    delay_init();                                   // 延时函数初始化
    GENERAL_TIM_Init(TIM_4, 0, 1);
   
#if OLED // OLED文件存在
    OLED_Init();
    OLED_ColorTurn(0);   // 0正常显示，1 反色显示
    OLED_DisplayTurn(0); // 0正常显示 1 屏幕翻转显示
#endif

  
#if OLED // OLED文件存在
    OLED_Clear();
#endif
	
	 
	
}
// 网络初始化
void Net_Init()
{


}

// 任务1
void task1(void)
{
	//1秒计算器
 	Automation_Close();
	Update_oled_massage();   // 更新OLED
}
// 任务2
void task2(void)
{

   
                 
    State = ~State;
}
// 任务3
void task3(void)
{
		
}
// 软件初始化
void SoftWare_Init(void)
{
    // 定时器初始化
    timer_init(&task1_id, task1, 1000, 1); // 1s执行一次
    timer_init(&task2_id, task2, 200, 1);  // 200ms执行一次
   

}
// 主函数
int main(void)
{
    SoftWare_Init(); // 软件初始化
    Hardware_Init(); // 硬件初始化
    TIM_Cmd(TIM4, ENABLE); // 使能计数器

    while (1) {

        // 线程
        timer_loop(); // 定时器执行
  
				// 语音控制
				Send_Usart2();
#if KEY_OPEN
				// 按键监测
				if(time25ms == MY_TRUE){
						Check_Key_ON_OFF();
						time25ms = MY_FALSE;
				}
#endif
    }
}

