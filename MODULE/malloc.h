#ifndef __MALLOC_H
#define __MALLOC_H

#include "stm32f4xx_hal.h"

#ifndef NULL
#define NULL 0
#endif	

//mem1�ڴ�����趨.�����ڲ�SRAM����.
#define MEM1_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			40*1024  						//�������ڴ� 100K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С
		 
//�ڴ���������
struct _m_mallco_dev
{
	void (*init)(void);					//��ʼ��
	uint8_t (*perused)(void);	  //�ڴ�ʹ����
	uint8_t *membase;				//�ڴ�� ����SRAMBANK��������ڴ�
	uint16_t *memmap; 				//�ڴ����״̬��
	uint8_t  memrdy; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s,uint8_t c,uint32_t count);	//�����ڴ�
void mymemcpy(void *des,void *src,uint32_t n);//�����ڴ�     
void my_mem_init(void);				//�ڴ�����ʼ������(��/�ڲ�����)
uint32_t my_mem_malloc(uint32_t size);	//�ڴ����(�ڲ�����)
uint8_t my_mem_free(uint32_t offset);		//�ڴ��ͷ�(�ڲ�����)
uint8_t my_mem_perused(void);				//����ڴ�ʹ����(��/�ڲ�����) 
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(uint32_t size);			//�ڴ����(�ⲿ����)
void *myrealloc(void *ptr,uint32_t size);//���·����ڴ�(�ⲿ����)

#endif

