//定义UART通信波特率
#define BAUDRATE  (115200)

#define NUM_AXIS 4  //打印轴的个数
#define HOMING_FEEDRATE {50*60, 50*60, 4*60, 0} //设置归位速度
#define AXIS_RELATIVE_MODES {false, false, false, false}  //（0:相对坐标、1:绝对坐标）


#define BUFSIZE 4            //队列缓存器：最多存储多少个命令
#define MAX_CMD_SIZE 96      //每条命令的字节数


#define ONBOARD_SD   //使用板载的SD卡打印
//#define LCD_SD       //使用LCD液晶模块上的SD卡打印

