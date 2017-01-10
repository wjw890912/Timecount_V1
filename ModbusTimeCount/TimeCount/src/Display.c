#include <stdio.h>
#include "M051Series.h"
#include "Display.h"


//����ܵ�ɨ��Ƶ��	50HZ
#define DISPLAY_SCAN_TIME_HZ   50
//������ɨ��Ƶ��	250HZ
//#define KEY_SCAN_TIME_HZ	   20

#define DG1OPEN	               P25=1
#define DG1CLOSE               P25=0

#define DG2OPEN	               P26=1
#define DG2CLOSE               P26=0

#define DG3OPEN	               P27=1
#define DG3CLOSE               P27=0

#define DG4OPEN	               P44=1
#define DG4CLOSE               P44=0

#define BUZZER                 P42



#define TIMESTARTDEC		    1
#define TIMEINIT		        0
#define TIMESTOP		        2


#define  GET_BUTTON  	       (P43)

#define  RUNKEY 		        0x01 

const uint8_t NumberTab[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};/*CA mode 7seg data */
static uint8_t DG1number=0, DG2number=0, DG3number=0,DG4number=0;  //locke save the display memoery
 int32_t TimeCount=0;// the RotaryEncoder value  is seconed  
static int32_t LastValue=0;// �����洢��ʱֵ���м���  ���=0�Ļ������κδ�����Ϊ0��ζ����������Ҫ���FLASH������
static uint8_t Arlm=0;//����ֵ 0������ 1����
static uint8_t ButtonNum=TIMEINIT;
static uint8_t Continuous=0;
int flash;


/*

  1���ϵ�״̬��ʼ��Ϊ TIMEINIT�������ضϵ�֮ǰ��ס�Ķ�ʱֵ ��
  2�����°�����ʼ����ʱ��״̬��Ϊ TIMESTARTDEC  ʱ�俪ʼ���٣���0�ͱ�����
  3���ڴ˰��°��������Ϊ0��ֹͣ���������ָ�����ʱ֮ǰ��ֵ����Ϊ0����ͣ��
  4���ٰ�һ�°�����������ʼ����ʱ��
  ����������

��������    0����������1����ʼ����ʱ����0֮���Ϊ2�����¼����ϴεļ���ֵ�� ����Ϊ0���ظ�֮ǰ�Ķ�����





*/
void TimeInit(void)
{

	 ButtonNum=TIMEINIT;//��ʼ��״̬
//   LastValue = GetFlashValue();	 //from EEPROM or FLASH
	FMC_Open();
//	FMC_Erase(0x1f000);
//	FMC_Write(0x1f000, 0x12345678);
	LastValue=FMC_Read(0x1f000);
	 if(LastValue>100*60)
	 {
	   LastValue=0;
	 }
	 else
	 {
	   LastValue= LastValue;
	 }
	 TimeCount=LastValue;//�����ϴ�״̬
	 LastValue=0;//���³�ʼ����--��Ϊ��ͽ�д��FLASH�У����ﲻ��Ҫд�����Բ����� 

}
void SaveFlash(int32_t *pt)
{

	if(*pt!=0)
	{
	  //save value to FLASH
	  // PutFlashData(*pt);

		flash= *pt;//���ڴ���ģ��

	FMC_Erase(0x1f000);
    FMC_Write(0x1f000, flash);

		*pt =0;//�޸�ֵ����Ϊ0��
	}
	else
	{
	   // do noting 
	
	}


}

int GetFlash(void)
{
	//get flash the time value 
	//return GetFlahData();

	flash=FMC_Read(0x1f000);

	return flash;

} 


void TimeRunHook(void)
{
  //����ɨ��
		
				  switch (ButtonNum)
				  {


				  
					 case TIMEINIT: {
					          if(TimeCount)
							  {P40=1;}
							  ButtonNum=TIMESTARTDEC;//��ʼ����ʱ
							  if((Continuous==0)|(Continuous==2))
							  {
							  Continuous=0;
							  LastValue=TimeCount;//�ݴ����ֵ��������FLASH�����ݴ�
							  SaveFlash(&LastValue); //����FLASH�У������ LastValue
							  
					 		  }

					 break;}

					 case 1: {
								 if(Arlm)
								 {
								 //����Ѿ�������ô
					 			 ButtonNum=TIMESTOP;//	ֹͣ�����������ظ�����ʱǰ��ֵ��
								 Arlm=0;//�رշ�����
								 TimeCount=GetFlash();
								 Continuous=2;

								 }
								 else
								 {
							
								 //�����û�б�������ô��ͣ��ǰ��ʱ����
								 ButtonNum=TIMESTOP;//	ֹͣ�����������ظ�����ʱǰ��ֵ��
								 Continuous=1;
								
								 }

								 ButtonNum=TIMEINIT;//	�ָ��ظ���ʼ
					 			 P40=0;
					 break;}

				
				


				  
				  }


				
			

}


void DisplayHWInit(void)
{

	/*
	   SEG : 
	    P0�˿ڶ�Ӧ����ܵ���������
		   P00--a
		   p01--b
		   p02--c									
		   p03--d								   
		   p04--e									
		   p05--f
		   p06--g
		   p07--dp
		COM :

		   P2.5--DG1
		   P2.6--DG2
		   P2.7--DG3
		   P4.4--DG4
		BEEP ��
		   P4.2--BEEP 0:CLOSE 1 OPEN .
	
	
	
	*/
   GPIO_SetMode(P0, BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7, GPIO_PMD_OUTPUT); // 8bit SEG data bus	 setting output 
   GPIO_SetMode(P2, BIT5|BIT6|BIT7, GPIO_PMD_OUTPUT);// 3 bit COM data bus 
   GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);// 4 com
   GPIO_SetMode(P4, BIT2, GPIO_PMD_OUTPUT);// BEEP
   GPIO_SetMode(P4, BIT3, GPIO_PMD_INPUT );// key
   DG1CLOSE;// close DG1
   DG2CLOSE;// close DG2
   DG3CLOSE;// close DG3


}
void TimeDec(void)
 {
 
 	
		
		static uint32_t	n=0; 


				    n++;
				if(n>(1000000/50)) //1S ʱ�䵽��ִ��һ��
				{
				            n=0;
				   
						   if(ButtonNum==TIMESTARTDEC)
						   {
								   	   if(TimeCount==0)
									   {
									     Continuous=0;
									   	  Arlm = 1;//������
									   }
									   else
									   {
									      Arlm = 0;//��������
									   	 TimeCount--;//ʱ�����һ��
									   
									   }
						   }
				   
				}

		 
 
 
 
 
 }
void DisplayPoll(void)
{
		static uint8_t count=1;
		
			

			switch (count)
				   {
				   	  case 1:
					  {
					  	DG4CLOSE;// close DG3
						DG1OPEN; // hold DG1 utill next option 
						GPIO_SET_OUT_DATA(P0,(NumberTab[DG1number]));
					    
						break;
					  }

					  case 2:
					  {
					  	DG1CLOSE;
						DG2OPEN;
					    GPIO_SET_OUT_DATA(P0,NumberTab[DG2number]&0x7f);
					    
						break;
					  }

				   	  case 3:
					  {
					  	DG2CLOSE;
						DG3OPEN;
					    GPIO_SET_OUT_DATA(P0,NumberTab[DG3number]);
					    
					  	break;
					  }
				   	  case 4:
					  {
					  	DG3CLOSE;
						DG4OPEN;
					    GPIO_SET_OUT_DATA(P0,NumberTab[DG4number]);
					    
					  	break;
					  }
				   
				   }
				   
				  count++;
				  if(count>4)count=0;
			  
				
}

 static uint32_t time=0,time1=0;
 
 /*it must be pic ever 50us poll once again and again */
 void Timetick50us(void)
 {
  		 TimeDec();
		 time++;
		 time1++;
		
 }

 
  

unsigned char Trg;//����
unsigned char Cont;//����

void KeyRead( void )
{
    unsigned char ReadData = 255;

		  ReadData =255;
	if(GET_BUTTON==1){ReadData=(ReadData&0xfe);}

//	if(!GET_BUTTON2){ReadData=(ReadData&0xfd);}

//	if(!GET_BUTTON3){ReadData=(ReadData&0xfb);}

//	if(!GET_BUTTON4){ReadData=(ReadData&0xf7);}

	ReadData =ReadData ^0xff;
    Trg = ReadData & (ReadData ^ Cont); 
    Cont = ReadData; 

}

void KeyProc(void)
{
  
	if(Cont|Trg )
	{
		
	}
	
       if (Cont & RUNKEY )
    {    




	}
	if (Trg & RUNKEY )
	{
	  	 
	    TimeRunHook();//scan key and process fast so fast 			
				 
	  
	}

 }
 void GPIOP2P3P4_IRQHandler	()
 {
 
 	 GPIO_CLR_INT_FLAG(P4, BIT3);

	  TimeRunHook();//scan key and process fast so fast 


 
 }

 void DisplayScan(void)
 {
 
  /* 
		 
		 timecount  = (((1/f)*1000000)/50)
		 1000000: much us in a second
		 50: timer gernraled time pluse is 20KHZ so it is 50us once 

		 eg:  f=50HZ
		   
		 timecount = (((1/50)*1000000)/50)
		         =400
		
		 
		 */
		 if(time>(((1/DISPLAY_SCAN_TIME_HZ)*1000000)/50))
		 {
		    DisplayPoll();
		 	time=0;
		 }
		 // if(time1>(((1/KEY_SCAN_TIME_HZ)*1000000)/50))
		 //{	
		 
		  //	KeyRead();
		  //	KeyProc();
		//	time1=0;
 	//	 }


 }


/*
	 set diplay number that the number isn't full over number "9"

	 eg :  SetNumber(1,2,3);

	 option in evertime .

*/
void SetNumber(uint8_t Num1,uint8_t Num2,uint8_t Num3,uint8_t Num4)
{


		if(Num1<=9)
		{
	    DG1number = Num1;
		}
		if(Num2<=9)
		{ 
		DG2number = Num2;
		}
		if(Num3<=9)
		{ 
		DG3number = Num3;
		}
		if(Num4<=9)
		{ 
		DG4number = Num4;
		}



}



 void SetBuzzer(uint8_t status)
 {
 		  if(status)
		  {
		   BUZZER=1;
		  }
		  else
		  {
		   BUZZER=0;
		  }
 
 }
void BuzzePoll(void )
{
   if(Arlm)
   {
	  P40=0;
   	  SetBuzzer(1);
   
   }
   else
   {
   	 SetBuzzer(0);
   }

}

 /*
 The External INT0(P3.2) default IRQ, declared in startup_M051Series.s.
 */
void EINT0_IRQHandler(void)
{
    /* For P3.2, clear the INT flag */
    GPIO_CLR_INT_FLAG(P3, BIT2);


		   if(ButtonNum!=1)
		   {

			  
				if(P33==0)
				{
				  //��ת
				  if(TimeCount<20*59)
				  {
				 TimeCount+=15;
				  }
				  else
				  {
				  
				   TimeCount+=59;
				  }
				}
				else
				{
				 //��ת
				 if(TimeCount<20*59)
				 {
				 TimeCount-=15;
				 }
				 else
				 {
				  TimeCount-=59;
				 }
				}
			 
			 

		   }



}
/*
The External INT1(P3.3) default IRQ, declared in startup_M051Series.s.
 */
void EINT1_IRQHandler(void)
{
    /* For P3.3, clear the INT flag */
    GPIO_CLR_INT_FLAG(P3, BIT3);
}


void RotaryEncoderHWinit()
{

    /* Configure P3.2 as EINT0 pin and enable interrupt by rising  edge trigger */
    GPIO_SetMode(P3, BIT2, GPIO_PMD_INPUT);
    GPIO_EnableEINT0(P3, 2, GPIO_INT_RISING);
    NVIC_EnableIRQ(EINT0_IRQn);

    /* Configure P3.3 as EINT1 pin and enable interrupt by rising  edge trigger */
    GPIO_SetMode(P3, BIT3, GPIO_PMD_INPUT);
    GPIO_EnableEINT1(P3, 3, GPIO_INT_RISING);
    NVIC_EnableIRQ(EINT1_IRQn);
		
    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 * 10 KHz clock */
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_4);
    GPIO_ENABLE_DEBOUNCE(P3, BIT2 | BIT3);

	 /* Configure P3.3 as EINT1 pin and enable interrupt by rising  edge trigger */
    GPIO_SetMode(P4, BIT3, GPIO_PMD_INPUT);
    GPIO_EnableEINT1(P4, 3, GPIO_INT_RISING);
    NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
    GPIO_ENABLE_DEBOUNCE(P4, BIT3);

	GPIO_SetMode(P4, BIT0, GPIO_PMD_OUTPUT);//set output mode 
	P40=0;

 }

 void CountPoll(void)
 {

 uint8_t g,s,b,q;
   

	  if(TimeCount>5999)
	   {
	   TimeCount=5999;
	   }
	   if(TimeCount<0)
	   {
	   	TimeCount=0;
	   }

	  q=(TimeCount/60)/10;

	  b=(TimeCount/60)%10;

	  s=TimeCount%60/10;
	  g=TimeCount%60%10;
	 
	 SetNumber(q,b,s,g);


 }

















