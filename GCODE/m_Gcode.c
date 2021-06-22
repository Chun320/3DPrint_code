#include "m_Gcode.h"

float homing_feedrate[] = HOMING_FEEDRATE;  //���Ƹ���Ĺ�λ�ٶ�
bool axis_relative_modes[] = AXIS_RELATIVE_MODES;  //���Ƹ�����˶�ģʽ����0:������ꡢ1:�������꣩
static bool relative_mode = false;  //ȷ����ӡ��������˶�ģʽ��0:������ꡢ1:�������꣩

const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};     //�洢����ı��
static float destination[NUM_AXIS] = {0.0, 0.0, 0.0, 0.0};  //�洢�����Ŀ��λ��
float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };  //�洢����ĵ�ǰλ��

static float feedrate = 1500.0, next_feedrate;

void G0_G1(void)
{
    get_coordinates();
    //prepare_move();
}
/*
 *����������л�ȡ��һ�������л�ȡ�������һ��Ŀ��λ���Լ��˶��ٶ�
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
