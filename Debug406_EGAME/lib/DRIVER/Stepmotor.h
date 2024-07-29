#ifndef _STEPMOTOR_H
#define _STEPMOTOR_H
#include <Arduino.h>
typedef enum {
  S_VER   = 0,      /* ��ȡ�̼��汾�Ͷ�Ӧ��Ӳ���汾 */
  S_RL    = 1,      /* ��ȡ��ȡ���������� */
  S_PID   = 2,      /* ��ȡPID���� */
  S_VBUS  = 3,      /* ��ȡ���ߵ�ѹ */
  S_CPHA  = 5,      /* ��ȡ����� */
  S_ENCL  = 7,      /* ��ȡ�������Ի�У׼��ı�����ֵ */
  S_TPOS  = 8,      /* ��ȡ���Ŀ��λ�ýǶ� */
  S_VEL   = 9,      /* ��ȡ���ʵʱת�� */
  S_CPOS  = 10,     /* ��ȡ���ʵʱλ�ýǶ� */
  S_PERR  = 11,     /* ��ȡ���λ�����Ƕ� */
  S_FLAG  = 13,     /* ��ȡʹ��/��λ/��ת״̬��־λ */
  S_Conf  = 14,     /* ��ȡ�������� */
  S_State = 15,     /* ��ȡϵͳ״̬���� */
  S_ORG   = 16,     /* ��ȡ���ڻ���/����ʧ��״̬��־λ */
}SysParams_t;

typedef enum Dir{
    CW = 0,
    CCW = 1
}Dir_t;

typedef enum{
  Motor_1 = 0,
  Motor_2 = 1,
  Motor_3 = 2,
  Motor_4 = 3
}MotorIndex_t;

typedef struct MotorState{
    uint8_t addr;
    bool isRun;
    Dir_t dir;             // �õ������
    uint8_t acc;           // �õ�����ٶ�
    int16_t targerSpeed;          // �õ���ٶ�
    float targetAngle;      // Ŀ��Ƕ�
    float currAngle;       // ��ǰ�Ƕ�
    float currSpeed;       // ��ǰ�ٶ�
}MotorState_t;

typedef struct MotorRunDiff
{
    float dx;
    float dy;
}MotorRunDiff_t;

extern volatile MotorState_t Motor;
extern MotorRunDiff_t pixelDiff;
extern uint8_t rxCmd[128];
extern uint8_t rxCount;

void AllMotors_Init(void);
void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr); // ����ǰλ������
void Emm_V5_Reset_Clog_Pro(uint8_t addr); // �����ת����
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t s); // ��ȡ����
void Emm_V5_Modify_Ctrl_Mode(uint8_t addr, bool svF, uint8_t ctrl_mode); // ���������޸Ŀ���/�ջ�����ģʽ
void Emm_V5_En_Control(uint8_t addr, bool state, bool snF); // ���ʹ�ܿ���
void Emm_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, bool snF); // �ٶ�ģʽ����
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF); // λ��ģʽ����
void Emm_V5_Stop_Now(uint8_t addr, bool snF); // �õ������ֹͣ�˶�
void Emm_V5_Synchronous_motion(uint8_t addr); // �������ͬ����ʼ�˶�
void Emm_V5_Origin_Set_O(uint8_t addr, bool svF); // ���õ�Ȧ��������λ��
void Emm_V5_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF); // �޸Ļ������
void Emm_V5_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF); // �������������
void Emm_V5_Origin_Interrupt(uint8_t addr); // ǿ���жϲ��˳�����
void Emm_V5_Receive_Data(uint8_t *rxCmd, uint8_t *rxCount); // �������ݽ��պ���
#endif