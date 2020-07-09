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
	char line1[]="Distance=";
	int i=0,k=0,j=0,word=0;
	char temp;		  
		for(word=0;word<9;word++)
	{
	  for(i=0;i<7;i++)
	  {
		  temp=Font_8x8[line1[word]-32][i];
			data_write(temp);
		}
	}
	
    
}

float Ultrasonic_Transducer_gpio_0(void)
{ 
	hal_gpio_data_t input_data;
	uint32_t  start_count=0,end_count=0,duration_count;
	float time;
	int flag=0,end=0;
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
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		
		if(input_data==1)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
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
	time=duration_count;
	hal_gpt_delay_ms(1);
	return time;
	
}
float iutput_mode_gpio_0(void)
{
	hal_gpio_data_t input_data;
	uint32_t  start_count=0,end_count=0,duration_count;
	float time;
	int flag=0,end=0;
	hal_pinmux_set_function(HAL_GPIO_0,HAL_GPIO_0);
	hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_INPUT);
	while(1)
	{
		hal_gpio_get_input(HAL_GPIO_0,&input_data);
		if(input_data==1)
		{
			hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_count);
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
	time=duration_count;
	return time;
}	



int main(void)
{
		
	  uint32_t adc;
		float time,distance;
	  int a,b,c,d,distance_int;
	  char distancedata[4];
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
		i2c_ini();
	  hal_adc_init();	
		
		//gpio_application_ini_27();
		//gpio_application_ini_28();
		//gpio_application_ini_57();
		ini_command();
		clear_oled();
		
		
		while(1)
		{
			//output_mode_gpio_0();
			time=Ultrasonic_Transducer_gpio_0();
			distance=time/58;
			distance_int=distance*10;
			a=distance_int/1000;
			b=distance_int/100%10;
			c=distance_int/10%10;
			d=distance_int%10;
			
			printf("Distance=%d%d%d.%d \r\n",a,b,c,d);
			
			distancedata[0]=a;
			distancedata[1]=b;
			distancedata[2]=c;
			distancedata[3]=d;
			set_PA_CA(0);
			Show_Word();
			set_PA_CA(1);
			Show_Word2();
			for(int i=0;i<4;i++)
			{
				if(i==3)
				{
					for(int j =0;j<8;j++)
				 {
					data_write(Font_8x8[14][j]);
					
				 }
				}
				for(int j =0;j<8;j++)
				{
					data_write(Font_8x8[distancedata[i]+16][j]);
					
				}
			}
		 hal_gpt_delay_ms	(500	)	;

			}
		
   
}

