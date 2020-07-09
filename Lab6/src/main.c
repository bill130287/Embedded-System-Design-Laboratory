/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"
#include "system_mt7687.h"
#include "top.h"
#include "font_8x8.h"

#define slave_address 0x3C
#define MPU_address 0x68
#define MAG_address 0x0C
#define rad 0.0011
int16_t  x_acc,y_acc,z_acc,x_gy,y_gy,z_gy,x_mag,y_mag,z_mag;
float temp,X_acc,Y_acc,Z_acc,X_gy,Y_gy,Z_gy,X_mag,Y_mag,Z_mag;
int16_t main_data[20];// 0~1 X_acc(H-L)  2~3 Y_acc(H-L) 4~5 Z_acc(H-L) 6~7 Temp(H-L) 8~9 X_GY(H-L) 10~11 Y_GY(H-L) 12-13 Z_GY(H-L) 14-15 X_MAG(L-H) 16-17 Y_MAG(L-H) 18-19 Z_MAG(L-H)
uint8_t mag_data[6];//={0x01,0x02,0x03,0x04,0x05,0x06};
const unsigned char ascii_short[4][3]={{0x00,0x00,0x00},{0x00,0x36,0x36},{0x00,0x60,0x60},{0x08,0x08,0x08}};//[0][]=space  [1][]=: [2][]=.
int16_t row1_cnt=16;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_0, ch);
    return ch;
}

/**
*@brief Set pinmux to UART and initialize UART hardware initialization for logging.
*@param None.
*@return None.
*/
static void plain_log_uart_init(void)
{
    hal_uart_config_t uart_config;
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_UART1_RTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);
		
	  

    /* COM port settings */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);
}

/**
*@brief Configure and initialize the systerm clock.
*@param None.
*@return None.
*/
static void SystemClock_Config(void)
{
    top_xtal_init();
}


/**
*@brief  Initialize the periperal driver in this function. In this example, we initialize UART drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    plain_log_uart_init();
}

/**
*@brief  In this function we get the corresponding voltage to the raw 2 from ADC.
*@param[in] adc_data: the raw data from ADC.
*@return This example returns the value of corresponding voltage to adc_data.
*@note If "adc_data" represents the raw data from ADC, the corresponding voltage is: (reference voltage/ ((2^resolution)-1)))*adc_data.
The reference voltage of MT7687 is 2.5V and resolution of MT7687 is 12bit.
*/
static uint32_t adc_raw_to_voltage(uint32_t adc_data)
{
    /* According to the formulation described above, the corresponding voltage of the raw data "adc_data" is
    2500/(2^12-1)*adc_data, and the uint of the voltage is mV */
    uint32_t voltage = (adc_data * 2500) / 4095;
    return voltage;
}

/**
*@brief  Get the ADC raw data with the polling mode.
*@param None.
*@return None.
*/
static void adc_get_data_polling_example(void)
{
    uint32_t adc_data;
    uint32_t adc_voltage;
    uint32_t test_times;

    printf("\r\n ---adc_example begin---\r\n");

    hal_adc_init();

    for (test_times = 0; test_times < 20; test_times++) {
        printf("\r\n\r\n###### test_times = %d ######\r\n", (int)test_times);

        hal_adc_get_data_polling(HAL_ADC_CHANNEL_0, &adc_data);
        adc_voltage = adc_raw_to_voltage(adc_data);
        printf("Channel, Data, Voltage(mV)\r\n");
        printf("%7d, 0x%04x, %d\r\n", HAL_ADC_CHANNEL_0, (unsigned int)adc_data, (int)adc_voltage);

        hal_gpt_delay_ms(1000);
    }

    hal_adc_deinit();

    printf("\r\n ---adc_example finished!!!---\r\n");
}

static void i2c_ini(void)
{
		hal_i2c_config_t i2c_port;
		i2c_port.frequency= HAL_I2C_FREQUENCY_400K;
		hal_i2c_master_init(HAL_I2C_MASTER_0,&i2c_port);
}

void gpio_application_ini_57(void)
{
  	hal_gpio_init(HAL_GPIO_57);
		hal_pinmux_set_function(HAL_GPIO_57, HAL_GPIO_57_GPIO57); // Set the pin tooperate in GPIO mode.
		hal_gpio_set_direction(HAL_GPIO_57, HAL_GPIO_DIRECTION_INPUT);
		/*hal_gpio_get_input (HAL_GPIO_37,a);
	  return *a;*/
		//hal_gpio_deinit(37);
}

void gpio_application_ini_27(void)
{
	
  	hal_gpio_init(HAL_GPIO_27);
		hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_GPIO27); // Set the pin tooperate in GPIO mode.
		hal_gpio_set_direction(HAL_GPIO_27, HAL_GPIO_DIRECTION_OUTPUT);
		hal_gpio_set_output(HAL_GPIO_27, HAL_GPIO_DATA_LOW);
		/*hal_gpio_get_input (HAL_GPIO_37,a);
	  return *a;*/
		//hal_gpio_deinit(37);

}

void gpio_application_ini_28(void)
{
	
  	hal_gpio_init(HAL_GPIO_28);
		hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_GPIO28); // Set the pin tooperate in GPIO mode.
		hal_gpio_set_direction(HAL_GPIO_28, HAL_GPIO_DIRECTION_OUTPUT);
		hal_gpio_set_output(HAL_GPIO_28, HAL_GPIO_DATA_LOW);
		/*hal_gpio_get_input (HAL_GPIO_37,a);
	  return *a;*/
		//hal_gpio_deinit(37);

}

void FULLscale_setting(void)
{
	uint8_t GyAcc_config[3]={0x1B,0x18,0x18};
	
	uint8_t test[1]={0x6A};
	uint8_t bypass_setting[2]={0x37,0x02};
	uint8_t MAG_mode[2]={0x0A,0x16};
	uint8_t add[1]={0x00};
	uint8_t add2[1]={0x1C};
	uint8_t data[1];
	uint8_t data2[2];
	
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&GyAcc_config[0],3);
	hal_gpt_delay_ms(10);
	/*hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&bypass_setting[0],2);//Bypass setting
	hal_gpt_delay_ms(10);
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&MAG_mode[0],2);//MAG_mode  MAG_Address
	hal_gpt_delay_ms(10);*/
	
	
	//test
	/*
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&test[0],1);
	hal_i2c_master_receive_polling	(HAL_I2C_MASTER_0 ,	MPU_address,&data[0],1 );	
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&bypass_setting[0],1);
	hal_i2c_master_receive_polling	(HAL_I2C_MASTER_0 ,	MPU_address,&data2[0],1 );	
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&MAG_mode[0],1);
	hal_i2c_master_receive_polling	(HAL_I2C_MASTER_0 ,	MAG_address,&data2[1],1 );	
	printf("=========\r\n");
	printf("%d\r\n",data[0]);
	printf("=========\r\n");
	printf("%d",data2[0]);
	printf("=========\r\n");
	printf("%d",data2[1]);
	printf("=========\r\n");*/
	/*hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&add[0],1);
	hal_i2c_master_receive_polling	(HAL_I2C_MASTER_0 ,	MPU_address,&data[0],1 );	
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&add2[0],1);
	hal_i2c_master_receive_polling	(HAL_I2C_MASTER_0 ,	MPU_address,&data2[0],1 );	
	
	printf("data:%d\n",data[0]);
	printf("data2:%d\n",data2[0]);*/
}

void address_write(char reg_add,uint8_t slave_add)
{
	uint8_t data[1];
	data[0]=reg_add;
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,slave_add,&data[0],1);
}

uint8_t address_receive(uint8_t slave_add)
{
	 uint8_t data[1];	
	 hal_i2c_master_receive_polling	(HAL_I2C_MASTER_0 ,slave_add,&data[0],1 );	
	 return data[0];
}

void data_write(char a)
{
	uint8_t data[2];
	data[0]=0x40;
	data[1]=a;
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,slave_address,&data[0],2);
}

void command_write(char a)
{
	uint8_t command[2];
	command[0]=0x80;
	command[1]=a;
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,slave_address,&command[0],2);
}

void display_off(void)
{
	const uint8_t command_off[2]={0x80,0xAE};
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,slave_address,&command_off[0],2);	
}

void display_on(void)
{
	const uint8_t command_on[2]={0x80,0xAF};
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,slave_address,&command_on[0],2);	
}

void normal_display(void)
{
	const uint8_t normal_command[2]={0x80,0xA6};
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,slave_address,&normal_command[0],2);	
}
void ini_command(void)
{ 
	command_write(0xAE);
	hal_gpt_delay_ms	(	1000	)	;
	command_write(0xAF);
	command_write(0xA6);
	command_write(0x20);
	command_write(0x02);
}

void clear_oled(void)
{
//	uint8_t set_PA_CA[6]={0x80,0xB0,0x80,0x00,0x80,0x10};
	//const uint8_t data_clear[2]={0x40,0x00};
	for(int i=0;i<8;i++)
	{
		command_write(0xB0+i);
		command_write(0x00);
		command_write(0x10);
		for(int j=0;j<128;j++)
		{
			data_write(0x00);
		}
	}
	
}

void set_PA_CA(int pa)
{
	uint8_t set_PA_CA[6]={0x80,0x00,0x80,0x00,0x80,0x10};
	switch(pa)
	{
		case 0: set_PA_CA[1]=0xB0; break;
		case 1: set_PA_CA[1]=0xB1; break;
		case 2: set_PA_CA[1]=0xB2; break;
		case 3: set_PA_CA[1]=0xB3; break;
		case 4: set_PA_CA[1]=0xB4; break;
		case 5: set_PA_CA[1]=0xB5; break;
		case 6: set_PA_CA[1]=0xB6; break;
		default: set_PA_CA[1]=0xB7; break;
	}
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,slave_address,&set_PA_CA[0],6);
}



void Show_Word(void)
{
	char line1[]="     NTUST";
	int i=0,k=0,j=0,word=0;
	char temp;		  
		for(word=0;word<10;word++)
	{
	  for(i=0;i<7;i++)
	  {
		  temp=Font_8x8[line1[word]-32][i];
			data_write(temp);
		}
	}
    
}

void Show_Word2(void)
{
	char line1[]="Temperature=";
	int i=0,k=0,j=0,word=0;
	char temp;		  
		for(word=0;word<12;word++)
	{
	  for(i=0;i<7;i++)
	  {
		  temp=Font_8x8[line1[word]-32][i];
			data_write(temp);
		}
	}
	
    
}

void Show_Word3(void)
{
	char line1[]="G:";
	int i=0,k=0,j=0,word=0;
	char temp;		  
		for(word=0;word<2;word++)
	{
	  for(i=0;i<7;i++)
	  {
		  temp=Font_8x8[line1[word]-32][i];
			data_write(temp);
		}
	}
	
    
}

void Show_Word4(void)
{
	char line1[]="A:";
	int i=0,k=0,j=0,word=0;
	char temp;		  
		for(word=0;word<2;word++)
	{
	  for(i=0;i<7;i++)
	  {
		  temp=Font_8x8[line1[word]-32][i];
			data_write(temp);
		}
	}
	
    
}

void Show_Word5(void)
{
	char line1[]="M:";
	int i=0,k=0,j=0,word=0;
	char temp;		  
		for(word=0;word<2;word++)
	{
	  for(i=0;i<7;i++)
	  {
		  temp=Font_8x8[line1[word]-32][i];
			data_write(temp);
		}
	}
	
    
}

void put_data(void)
{
	int addressMPU_start=0x3B,addressMAG_start=0x03;
	uint8_t addressMAG_start_a[1]={0x03};
	uint8_t bypass_setting[2]={0x37,0x02};
	uint8_t reset_bypass[2]={0x37,0x00};
	uint8_t MAG_mode[2]={0x0A,0x11};
	
	for(int i=0;i<=13;i++)
	{
		address_write(addressMPU_start,MPU_address);
		main_data[i]=address_receive(MPU_address);
		addressMPU_start+=1;
	}
	
 /*	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&bypass_setting[0],2);//Bypass setting
	
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&MAG_mode[0],2);//MAG_mode  MAG_Address
		//hal_gpt_delay_us(100);
	  //hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&addressMAG_start_a[0],1);
		//hal_i2c_master_receive_polling	(HAL_I2C_MASTER_0 ,MAG_address ,&mag_data[0],6 );
	for(int j=14;j<20;j++)
	{
		address_write(addressMAG_start,MAG_address);
		main_data[j]=address_receive(MAG_address);
		addressMAG_start+=1;
	}
	
	for(int i=14;i<20;i++)
	{
		printf("main data %d=%d\r\n",i,main_data[i]);
	}*/
	/*printf("======\r\n");
	printf("x_mag 0:%d\r\n",main_data[14]);
			printf("y_mag 1:%d\r\n",main_data[15]);
			 printf("x_mag 2:%d\r\n",main_data[16]);
			printf("y_mag 3:%d\r\n",main_data[17]);
	    printf("x_mag 4:%d\r\n",main_data[18]);
			printf("y_mag 5:%d\r\n",main_data[19]);
			printf("======\r\n");	*/	
}

void MAG_put_data(void)
{
	
	uint8_t addressMAG_start[1]={0x03};
	uint8_t bypass_setting[2]={0x37,0x02};
	uint8_t reset_bypass[2]={0x37,0x00};
	uint8_t MAG_mode[2]={0x0A,0x16};
	
	uint8_t mode_check[1];
	uint8_t bypass_check[1];
	/*hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&bypass_setting[0],2);//Bypass setting
	
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&MAG_mode[0],2);//MAG_mode  MAG_Address
	hal_gpt_delay_us(100);*/
	i2c_ini();
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&bypass_setting[0],2);//Bypass setting
	hal_gpt_delay_ms(10);
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&MAG_mode[0],2);//MAG_mode  MAG_Address
	hal_gpt_delay_ms(10);
	
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&addressMAG_start[0],1);
	hal_i2c_master_receive_polling(HAL_I2C_MASTER_0 ,MAG_address ,&mag_data[0],6);
	hal_i2c_master_deinit(HAL_I2C_MASTER_0);
	  /*hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&MAG_mode[0],1);
		hal_gpt_delay_ms(10);
		hal_i2c_master_receive_polling(HAL_I2C_MASTER_0 ,MAG_address ,&mode_check[0],1);
		hal_gpt_delay_ms(10);
		hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&bypass_setting[0],1);
		hal_gpt_delay_ms(10);
		hal_i2c_master_receive_polling(HAL_I2C_MASTER_0 ,MPU_address ,&bypass_check[0],1);
		hal_gpt_delay_ms(10);
		printf("mode check:%d\r\n",mode_check[0]);
		printf("bypass check:%d\r\n",bypass_check[0]);
		//hal_gpt_delay_us(100);
	
	  hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MAG_address,&addressMAG_start[0],1);
		hal_i2c_master_receive_polling(HAL_I2C_MASTER_0 ,MAG_address ,&mag_data[0],6);
		hal_i2c_master_deinit(HAL_I2C_MASTER_0);
		for(int i=0;i<6;i++)
	{
		printf("data %d = %d\r\n",i,mag_data[i]);
	}*/
		//main_data[j]=address_receive(MAG_address);
		
	
	/*hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&reset_bypass[0],2);//Bypass setting
	//reset bypass check
	hal_i2c_master_send_polling(HAL_I2C_MASTER_0,MPU_address,&reset_bypass[0],1);
	//hal_gpt_delay_us(100);
	hal_i2c_master_receive_polling(HAL_I2C_MASTER_0 ,MPU_address ,&bypass_check[0],1);
	printf("reset bypass check:%d\r\n",bypass_check[0]);*/
}

void data_trans(void)
{
			x_acc=(main_data[0]<<8)| (main_data[1]);
			y_acc=(main_data[2]<<8)| (main_data[3]);
			z_acc=(main_data[4]<<8)| (main_data[5]);
			temp =(main_data[6]<<8)| (main_data[7]);
			x_gy =(main_data[8]<<8)| (main_data[9]);
			y_gy =(main_data[10]<<8)|(main_data[11]);
			z_gy =(main_data[12]<<8)|(main_data[13]);
			x_mag =(int16_t)(mag_data[1]<<8)|(mag_data[0]);
			y_mag =(int16_t)(mag_data[3]<<8)|(mag_data[2]);
			z_mag =(int16_t)(mag_data[5]<<8)|(mag_data[4]);
	    /*printf("=======\r\n");
			printf("x_mag:=%d",x_mag);
			printf("y_mag:=%d",y_mag);
			printf("z_mag:=%d",z_mag);
			printf("=======\r\n");*/
			/*for(int i=14;i<20;i++)
	{
		printf("%d\r\n",main_data[i]);
	}*/
			/*printf("======\r\n");
	    printf("x_mag 0:%d\r\n",mag_data[0]);
			printf("y_mag 1:%d\r\n",mag_data[1]);
			printf("x_mag 2:%d\r\n",mag_data[2]);
			printf("y_mag 3:%d\r\n",mag_data[3]);
	    printf("x_mag 4:%d\r\n",mag_data[4]);
			printf("y_mag 5:%d\r\n",mag_data[5]);
			printf("======\r\n");	*/	
	
	
			/*printf("======\r\n");
	    printf("x_mag:%d\r\n",x_mag);
			printf("y_mag:%d\r\n",y_mag);
			printf("z_mag:%d\r\n",z_mag);
			printf("======\r\n");*/
			X_acc=(float)x_acc*0.0048;
			Y_acc=(float)y_acc*0.0048;
			Z_acc=(float)z_acc*0.0048;
			temp =(float)(temp/333.87)+21;
		  X_gy =(float)(x_gy*rad);
			Y_gy =(float)(y_gy*rad);
			Z_gy =(float)(z_gy*rad);
			X_mag =(float)(x_mag*0.146);
			Y_mag =(float)(y_mag*0.146);
			Z_mag =(float)(z_mag*0.146);
			
			/*printf("======\r\n");
	    printf("X_mag:%.1f\r\n",X_mag);
			printf("Y_mag:%.1f\r\n",Y_mag);
			printf("Z_mag:%.1f\r\n",Z_mag);
			printf("======\r\n");*/
			
}
void temp_oled(float Temp)
{
	char Temp_array[3];
	int16_t Temp_ten,Temp_unit,Temp_float,Temp_int,zero_print=0;
	Temp_int=Temp*10;
	Temp_ten=Temp_int/100; if(Temp_ten==0) zero_print=1;
	Temp_unit=Temp_int/10%10;
	Temp_float=Temp_int%10;
	
	Temp_array[0]=Temp_ten;
	Temp_array[1]=Temp_unit;
	Temp_array[2]=Temp_float;
  //printf("xxxx %d xxxx\n",Temp_int);
	for(int i=zero_print;i<3;i++)
			{
				if(i==2)
				{
					for(int j =0;j<8;j++)
				 {
					data_write(Font_8x8[14][j]);
					
				 }
				}
				for(int j =0;j<8;j++)
				{
					data_write(Font_8x8[Temp_array[i]+16][j]);
					
				}
			}
}

void G_oled(float X_gy)
{
	char  Xgy_array[3];
	int16_t ten,unit,gfloat,gint,zero_print;
	zero_print=0;
	gint=X_gy*10;  if(gint<0) gint=~gint+1;
	ten=gint/100;  if(ten==0) zero_print=1;
	unit=gint/10%10;
	gfloat=gint%10;
	Xgy_array[0]=ten;
	Xgy_array[1]=unit;
	Xgy_array[2]=gfloat;
	if(X_gy<0&&((ten+unit+gfloat)>0))//­t¼Æ®É
	{
		
			for(int h =0;h<3;h++)
			{
				data_write(ascii_short[3][h]);
			}
			row1_cnt+=3;
	}
		for(int i=zero_print;i<3;i++)
		{
			if(i==2)
			{
				for(int j =0;j<3;j++)
				{
				data_write(ascii_short[2][j]);
				}
			}
			for(int j =0;j<8;j++)
			{
				data_write(Font_8x8[Xgy_array[i]+16][j]);	
			}
		}
		
		
		row1_cnt+=(3-zero_print)*8+3;
		
	
	
	
}

void clear_row(int16_t row_cnt)
{
	for(int clear =0;clear<(128-row_cnt);clear++)
	{
			data_write(0x00);	
	}
	row1_cnt=16;
}
/*void YG_oled(float Y_gy)
{
	char Ygy_array[3];
	int16_t ten,unit,gfloat,gint,zero_print;
	gint=Y_gy*10;  if(gint<0) gint=gint*-1;
	ten=gint/100;	if(ten==0)  zero_print=1;
	unit=gint/10%10;
	gfloat=gint%10;
	Ygy_array[0]=ten;
	Ygy_array[1]=unit;
	Ygy_array[2]=gfloat;
	if(Y_gy<0)
	{
		
			for(int h =0;h<8;h++)
			{
				data_write(Font_8x8[13][h]);
			}
			column1_cnt+=8;
	} 
		for(int i=zero_print;i<3;i++)
		{
			if(i==2)
			{
				for(int j =0;j<3;j++)
				{
				data_write(ascii_short[2][j]);
				}
			}
			for(int j =0;j<8;j++)
			{
				data_write(Font_8x8[Ygy_array[i]+16][j]);	
			}
		}
		for(int k =0;k<3;k++)
		{
			data_write(ascii_short[0][k]);	
		}
		column1_cnt+=(3-zero_print)*8+3+3;
	
	
	
}*/

/*void ZG_oled(float Z_gy)
{
	char Zgy_array[3];
	int16_t ten,unit,gfloat,gint,zero_print;
	gint=Z_gy*10;  if(gint<0) gint=gint*-1;
	ten=gint/100;	 if(ten==0)  zero_print=1;
	unit=gint/10%10;
	gfloat=gint%10;
	Zgy_array[0]=ten;
	Zgy_array[1]=unit;
	Zgy_array[2]=gfloat;
	if(Z_gy<0)
	{
		
			for(int h =0;h<8;h++)
			{
				data_write(Font_8x8[13][h]);
			}
			column1_cnt+=8;
	}
		for(int i=zero_print;i<3;i++)
		{
			if(i==2)
			{
				for(int j =0;j<3;j++)
				{
				data_write(ascii_short[2][j]);
				}
			}
			for(int a =0;a<8;a++)
			{
				data_write(Font_8x8[Zgy_array[i]+16][a]);	
			}
		}
		for(int k =0;k<3;k++)
		{
			data_write(ascii_short[0][k]);	
		}
		column1_cnt+=(3-zero_print)*8+3+3;
	
	

	//printf("%d\n",column1_cnt);
	
}*/

/*float Ultrasonic_Transducer_gpio_0(void)
{ 
	hal_gpio_data_t input_data;
	uint32_t  start_count=0,end_count=0,duration_count;
	uint32_t humid_data=0,temper_data=0,test_data=0;
	float time;
	int flag=0,end=0,bits_num=0,bits_shift=15;
	hal_pinmux_set_function(HAL_GPIO_0,HAL_GPIO_0_GPIO0);
	hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_OUTPUT);
	hal_gpio_set_output	(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
	hal_gpio_set_output	(HAL_GPIO_0,HAL_GPIO_DATA_HIGH);
	hal_gpt_delay_us(10);
	hal_gpio_set_output	(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
	//hal_gpt_delay_ms(1);
	//printf("uuuu\r\n");
	
	hal_pinmux_set_function(HAL_GPIO_0,HAL_GPIO_0_GPIO0);
	hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_INPUT);
	for(bits_num=0;bits_num<=40;bits_num++)
	{
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==0)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
			break;
		}
	}
	while(1)
	{
		
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		if(input_data==1)
		{
			
			break;
		}
	}
	
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==0)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_count);
			break;
		}
	}
	
	hal_gpt_get_duration_count(start_count,end_count,&duration_count);	
	
	if(bits_num<=16)
	{
		if(duration_count>=72&&duration_count<=80)
		{
			humid_data=(humid_data||(0<<bits_shift));
			bits_shift-=1;
			if(bits_shift<0) bits_shift=15;
		}
		else if(duration_count>=114&&duration_count<=126&&bits_shift>0)
		{
			humid_data=(humid_data||(1<<bits_shift));
			bits_shift-=1;
			if(bits_shift<0) bits_shift=15;
		}
	}
	
	else if(bits_num>=16&&bits_num<=32)
	{
		if(duration_count>=72&&duration_count<=80&&bits_shift>0)
		{
			temper_data=(temper_data||(0<<bits_shift));
			bits_shift-=1;
			if(bits_shift<0) bits_shift=15;
		}
		else if(duration_count>=114&&duration_count<=126&&bits_shift>0)
		{
			temper_data=(temper_data||(1<<bits_shift));
			bits_shift-=1;
			if(bits_shift<0) bits_shift=15;
		}
	}
	
	else
	{
		if(duration_count>=72&&duration_count<=80&&bits_shift>0)
		{
			test_data=(test_data||(0<<bits_shift));
			bits_shift-=1;
			if(bits_shift<0) bits_shift=15;
		}
		else if(duration_count>=114&&duration_count<=126&&bits_shift>0)
		{
			test_data=(test_data||(1<<bits_shift));
			bits_shift-=1;
			if(bits_shift<0) bits_shift=15;
		}
	}
 }
}*/

//humid_data
signed int humid_data(void)
{
	hal_gpio_data_t input_data;
	uint32_t  start_count=0,end_count=0,duration_count=0;
	uint32_t  humid_data=0;
	//float time;
	signed int bits_shift=15;
	//humid_data=0;
	for(bits_shift=15;bits_shift>=0;bits_shift--)
	{
		
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		//printf("%d ,input",input_data);
		if(input_data==1)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
			
			break;
		}
	}
	//printf("%d ,start\n",start_count);
	/*while(1)
	{
		
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
	
		if(input_data==0)
		{
			//printf("%d ,input",input_data);
			break;
		}
	}*/
	
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==0)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_count);
			
			break;
		}
	}
	//printf("%d ,input\n",end_count);
	
	hal_gpt_get_duration_count(start_count,end_count,&duration_count);	
	//printf("%d ,duration\n",duration_count);
	if(duration_count>=22&&duration_count<=30)
	{
		humid_data=(humid_data|(0<<bits_shift));
	}
	else if(duration_count>=66&&duration_count<=76)
	{
		humid_data=(humid_data|(1<<bits_shift));
	}
	}
	//humid_data=humid_data/10;
	return humid_data;
	
}
//temp_data
signed int temp_data(void)
{
	hal_gpio_data_t input_data;
	uint32_t  start_count=0,end_count=0,duration_count=0;
	uint32_t  temp_data=0;
	signed int bits_shift=15;
	 //temp_data=0;
	for(bits_shift=15;bits_shift>=0;bits_shift--)
	{
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==1)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
			break;
		}
	}
	/*while(1)
	{
		
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		if(input_data==1)
		{
			break;
		}
	}*/
	
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==0)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_count);
			break;
		}
	}
	
	hal_gpt_get_duration_count(start_count,end_count,&duration_count);	
	
	if(duration_count>=22&&duration_count<=30)
	{
		temp_data=(temp_data|(0<<bits_shift));
		
	}
	else //if(duration_count>=114&&duration_count<=126&&bits_shift>0)
	{
		temp_data=(temp_data|(1<<bits_shift));
		
	}
	}
	//temp_data=temp_data/10;
	return temp_data;
}
//test_data
int test_data(void)
{
	hal_gpio_data_t input_data;
	uint32_t  start_count=0,end_count=0,duration_count;
	uint32_t  test_data=0;
	signed int bits_shift=7;
	test_data=0;
	for(bits_shift=7;bits_shift>=0;bits_shift--)
	{
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==1)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
			break;
		}
	}
	/*while(1)
	{
		
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		if(input_data==1)
		{
			break;
		}
	}*/
	
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==0)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_count);
			break;
		}
	}
	
	hal_gpt_get_duration_count(start_count,end_count,&duration_count);	
	
	if(duration_count>=22&&duration_count<=30)
	{
		test_data=(test_data|(0<<bits_shift));
		
		
	}
	else //if(duration_count>=114&&duration_count<=126&&bits_shift>0)
	{
		test_data=(test_data|(1<<bits_shift));
		
	}
	}
	//test_data=test_data/10;
	return test_data;
}
void outmode_AM2302(void)
{
	hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_OUTPUT);
	hal_gpio_set_output	(HAL_GPIO_0,HAL_GPIO_DATA_HIGH);
	hal_gpt_delay_us(1);
	hal_gpio_set_output	(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
	hal_gpt_delay_ms(1);
	hal_gpio_set_output	(HAL_GPIO_0,HAL_GPIO_DATA_HIGH);
	hal_gpt_delay_us(30);
}

void inputmode_AM2302(void)
{
	hal_gpio_data_t input_data;
	//uint32_t   start_cnt,end_cnt,duration_cnt;
	hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_INPUT);
	
		while(1)
	{
		
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		if(input_data==0)
		{
			//printf("%d mode\n",input_data);
			//hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_cnt);
			break;
		}
	}
	
	while(1)
	{
		
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		if(input_data==1)
		{
			//printf("%d mode2\n",input_data);
			//hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_cnt);
			break;
		}
	}
	//hal_gpt_get_duration_count(start_cnt,end_cnt,&duration_cnt);
	//hal_gpt_delay_us(85);
	while(1)
	{
		
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		if(input_data==0)
		{
			//printf("%d mode\n",input_data);
			//hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_cnt);
			break;
		}
	}
	
	
	
}

int main(void)
{
	
		
		
    /* Configure system clock */
    SystemClock_Config();

    /* Configure the hardware */
	  prvSetupHardware();
		
    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
	  printf("welcome");
	  printf("\r\n\r\n");
		hal_gpio_init(HAL_GPIO_0);
		hal_gpio_init(HAL_GPIO_27);
		hal_gpio_init(HAL_GPIO_28);
    hal_pinmux_set_function(HAL_GPIO_27,HAL_GPIO_27_I2C1_CLK);
	  hal_pinmux_set_function(HAL_GPIO_28,HAL_GPIO_28_I2C1_DATA );
		hal_pinmux_set_function(HAL_GPIO_0,HAL_GPIO_0_GPIO0);
		i2c_ini();
	  hal_adc_init();	
		
		
		
		ini_command();
		clear_oled();   
		
		FULLscale_setting();
		while(1)
		{
			put_data();
			MAG_put_data();
			data_trans();
			
			printf("Temperature:%.1f \r\n",temp);
			printf("G:%.1f %.1f %.1f \r\n",X_gy,Y_gy,Z_gy);
			printf("A:%.1f %.1f %.1f \r\n",X_acc,Y_acc,Z_acc);
		  printf("M:%.1f %.1f %.1f \r\n",X_mag,Y_mag,Z_mag);
			printf("\r\n");
			
			/*printf("%d %d %d\r\n",x_gy,y_gy,z_gy);
			printf("%d %d %d\r\n",x_acc,y_acc,z_acc);*/
			/*for(int j=0;j<=13;j++)
			{
				printf("%d\r\n",main_data[j]);
			}*/
			set_PA_CA(0);
			Show_Word();
			
			set_PA_CA(1);
			Show_Word2();
			temp_oled(temp);
			
			set_PA_CA(2);
			Show_Word3();
			G_oled(X_gy);
			G_oled(Y_gy);
			G_oled(Z_gy);
			clear_row(row1_cnt);
			
			set_PA_CA(3);
			Show_Word4();
			G_oled(X_acc);
			G_oled(Y_acc);
			G_oled(Z_acc);
			clear_row(row1_cnt);
			
			set_PA_CA(4);
			Show_Word5();
			G_oled(X_mag);
			G_oled(Y_mag);
			G_oled(Z_mag);
			clear_row(row1_cnt);
			
			hal_gpt_delay_ms(30);
		}
		
   
}

