#include "m_Gcode.h"

float homing_feedrate[] = HOMING_FEEDRATE;  //控制各轴的归位速度
bool axis_relative_modes[] = AXIS_RELATIVE_MODES;  //控制各轴的运动模式（（0:相对坐标、1:绝对坐标）
static bool relative_mode = false;  //确定打印机总体的运动模式（0:相对坐标、1:绝对坐标）

const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};     //存储各轴的编号
static float destination[NUM_AXIS] = {0.0, 0.0, 0.0, 0.0};  //存储各轴的目标位置
float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };  //存储各轴的当前位置

static float feedrate = 1500.0, next_feedrate;

void G0_G1(void)
{
    get_coordinates();
    //prepare_move();
}
/*
 *从命令缓存区中获取下一条命令中获取各轴的下一个目标位置以及运动速度
*/
void get_coordinates(void)
{
  for(uint8_t i=0; i < NUM_AXIS; i++) {
    if(code_seen(axis_codes[i]))
    {
      destination[i] = (float)code_value() + (axis_relative_modes[i] || relative_mode)*current_position[i];
    }
    else destination[i] = current_position[i];
  }
  if(code_seen('F')) {
    next_feedrate = code_value();
    if(next_feedrate > 0.0f) feedrate = next_feedrate;
  }
}
