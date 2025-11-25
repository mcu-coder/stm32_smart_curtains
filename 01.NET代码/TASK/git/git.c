#include "git.h"

Data_TypeDef Data_init;						  // 设备数据结构体
Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
Device_Satte_Typedef device_state_init;		  // 设备状态


// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data)
{

	return MY_SUCCESSFUL;
}
// 初始化
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{

	//Value->Distance_value = 100;
  // 写
  //W_Test();
	// 读
	R_Test();

	return MY_SUCCESSFUL;
}
// 更新OLED显示屏中内容
mySta Update_oled_massage()
{
#if OLED // 是否打开
		char str[50];
		sprintf(str, "旋转角度: %03d ",device_state_init.check_device);
		OLED_ShowCH(0, 3, (unsigned char *)str);

#endif

	return MY_SUCCESSFUL;
}

// 更新设备状态
mySta Update_device_massage()
{
	
	// 执行语音模块发来消息
	if (Data_init.cmd)
	{
		switch (Data_init.cmd)
		{
		case 1:
			device_state_init.check_device = 0;
			Data_init.Device_State = 0;
			break;
		case 2:
			device_state_init.check_device = 180;
			Data_init.Device_State = 1;
			break;
		case 3:
			device_state_init.check_device = 90;
			break;
		}
		Data_init.cmd = 0;
		W_Test();
	}	
	// 赋值旋转度数
	if(device_state_init.check_device_copy != device_state_init.check_device){
		// 大 正转
		if(device_state_init.check_device > device_state_init.check_device_copy){
			//Motor_Ctrl_Angle_Z(device_state_init.check_device-device_state_init.check_device_copy, 3);
			Stepper_RotateByStep(Forward,
			(device_state_init.check_device-device_state_init.check_device_copy)*12,3);
		
		}else{
			//Motor_Ctrl_Angle_Z(device_state_init.check_device_copy-device_state_init.check_device, 3);
			Stepper_RotateByStep(Reversal,
			(device_state_init.check_device_copy-device_state_init.check_device)*6,3);
		
		}
		device_state_init.check_device_copy = device_state_init.check_device ;  
	}	

	

	return MY_SUCCESSFUL;
}

// 定时器
void Automation_Close(void)
{


}
// 检测按键是否按下
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// 与上一次的键值比较 如果不相等，表明有键值的变化，开始计时
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // 时间记录器
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // 短按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
			if(device_state_init.check_device > 0){
				device_state_init.check_device -=30;
			}else{
				device_state_init.check_device = 0;
				Data_init.Device_State = 0;
			}
		
			break;
		case KEY2_PRES:
			printf("Key2_Short\n");
			if(device_state_init.check_device < 180){
				device_state_init.check_device +=30;
			}else{
				device_state_init.check_device = 180;
				Data_init.Device_State = 1;
			}
	
			break;
		case KEY3_PRES:
			printf("Key3_Short\n");
			if (Data_init.Device_State == 1)
			{
				device_state_init.check_device = 0;
				Data_init.Device_State =0;
			}else{
				device_state_init.check_device = 180;
				Data_init.Device_State =1;
			}
		
			break;

		default:
			break;
		}
	  //写
		W_Test();
		num_on = 0;
	}
	else if (key == 0 && num_on >= Key_Scan_Time) // 长按
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Long\n");
			
			break;
		case KEY2_PRES:
			printf("Key2_Long\n");

			break;
		case KEY3_PRES:
			printf("Key3_Long\n");

			break;
		default:
			break;
		}
		num_on = 0;
	}

}
// 解析json数据
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // 检测json格式
	// const char *massage;
	// 定义数据类型
	u8 cjson_cmd; // 指令,方向

	/* 解析整段JSO数据 */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// 解析失败
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* 依次根据名称提取JSON数据（键值对） */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* 解析嵌套json数据 */


	switch (cjson_cmd)
	{
	case 0x01: // 消息包

		break;
	case 0x02: // 消息包

		break;
	case 0x03: // 数据包

		break;
	default:
		break;
	}

	/* 清空JSON对象(整条链表)的所有数据 */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
// 解析数据
mySta massage_speak(char *message)
{

	char *dataPtr = NULL;

	char numBuf[10];
	int num = 0;

	dataPtr = strchr(message, ':'); // 搜索':'

	if (dataPtr != NULL) // 如果找到了
	{
		dataPtr++;
		while (*dataPtr >= '0' && *dataPtr <= '9') // 判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		num = atoi((const char *)numBuf); // 转为数值形式
		if (strstr((char *)message, "cmd")) // 搜索"redled"
		{
			Data_init.cmd = num;
		}
	}
	return MY_SUCCESSFUL;
}
