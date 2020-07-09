/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* hal includes */
#include "hal.h"
#include "system_mt7687.h"
#include "top.h"
#include "hal_gpt.h"


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

void sent_data(int state)
{
	unsigned int /*LED_number*/data_13_th;
			
	for(data_13_th=0;data_13_th<=7;data_13_th++)
	{
		hal_gpio_set_output(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
		//hal_gpt_delay_us(10);
		hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
		hal_gpt_delay_us(10);
		hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
		//hal_gpt_delay_us(10);
		hal_gpio_set_output(HAL_GPIO_39,HAL_GPIO_DATA_LOW);
		hal_gpt_delay_us(10);		
	}
		
	for(int pin_10=0;pin_10<=9;pin_10++)
	{
		for(int bit=0;bit<=7;bit++)
		{
			if(bit<=3)
			{
				hal_gpio_set_output(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
				//hal_gpt_delay_us(10);
				hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
				hal_gpt_delay_us(10);
				hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
				//hal_gpt_delay_us(10);
				hal_gpio_set_output(HAL_GPIO_39,HAL_GPIO_DATA_LOW);
				hal_gpt_delay_us(10);	
			}
			else
			{
				if(pin_10<state)
				{
					hal_gpio_set_output(HAL_GPIO_0,HAL_GPIO_DATA_HIGH);
					//	hal_gpt_delay_us(10);
					hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
					hal_gpt_delay_us(10);
					hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_HIGH);
					//	hal_gpt_delay_us(10);
					hal_gpio_set_output(HAL_GPIO_39,HAL_GPIO_DATA_LOW);
					hal_gpt_delay_us(10);	
				}
				else
				{
					hal_gpio_set_output(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
					//hal_gpt_delay_us(10);
					hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
					hal_gpt_delay_us(10);
					hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
					//hal_gpt_delay_us(10);
					hal_gpio_set_output(HAL_GPIO_39,HAL_GPIO_DATA_LOW);
					hal_gpt_delay_us(10);	
				}
			}
		}				
	}
		
	for(int final_2=0;final_2<=1;final_2++)
	{
		for(int i=0;i<=7;i++)
		{
			hal_gpio_set_output(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
			//hal_gpt_delay_us(10);
			hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
			hal_gpt_delay_us(10);
			hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
			//hal_gpt_delay_us(10);
			hal_gpio_set_output(HAL_GPIO_39,HAL_GPIO_DATA_LOW);
			hal_gpt_delay_us(10);	
		}				
	}
	
	hal_gpio_set_output(HAL_GPIO_39,HAL_GPIO_DATA_LOW);
	hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
	hal_gpt_delay_us(300);
	
	for(int last_four=0;last_four<=3;last_four++)
	{
		hal_gpio_set_output(HAL_GPIO_0,HAL_GPIO_DATA_HIGH);
		hal_gpt_delay_us(1);
		hal_gpio_set_output(HAL_GPIO_0,HAL_GPIO_DATA_LOW);
		hal_gpt_delay_us(1);
	}

		
}

static void log_uart_init(void)
{
    hal_uart_config_t uart_config;
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_GPIO0);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);
	hal_pinmux_set_function(HAL_GPIO_37,HAL_GPIO_37_GPIO37);
	hal_pinmux_set_function(HAL_GPIO_39,HAL_GPIO_39_GPIO39);

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
    log_uart_init();
}


/**
*@brief  Configure the GPIO pull state to pull-up and to pull-down. Verify the pull states by inputs.
*@param None.
*@return None.
*/
static void gpio_configure_pull_state_example()
{

    hal_gpio_data_t data_pull_up;
    hal_gpio_data_t data_pull_down;
    hal_gpio_status_t ret;
    hal_pinmux_status_t ret_pinmux_status;

    printf("\r\n ---gpio_example begin---\r\n");

    ret = hal_gpio_init(HAL_GPIO_36);
    if (HAL_GPIO_STATUS_OK != ret) 
	{
    	printf("hal_gpio_init failed\r\n");
        hal_gpio_deinit(HAL_GPIO_36);
        return;
    }

    /*Step1: Set pinmux as GPIO mode, configure GPIO 33 to input and pull up it.  Afetr that we read the input data for further validation*/

    /* Set pin as GPIO mode.*/
    ret_pinmux_status = hal_pinmux_set_function(HAL_GPIO_36, HAL_GPIO_36_GPIO36);
    
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) 
	{
    	printf("hal_pinmux_set_function failed\r\n");
        hal_gpio_deinit(HAL_GPIO_36);
        return;
    }

    /* Set GPIO as input.*/
    ret = hal_gpio_set_direction(HAL_GPIO_36, HAL_GPIO_DIRECTION_INPUT);
    
    if (HAL_GPIO_STATUS_OK != ret) 
	{
        printf("hal_gpio_set_direction failed\r\n");
        hal_gpio_deinit(HAL_GPIO_36);
        return;
    }

    /* Configure the pull state to pull-up. */
    ret = hal_gpio_pull_up(HAL_GPIO_36);
    if (HAL_GPIO_STATUS_OK != ret) 
	{
        printf("hal_gpio_pull_up failed\r\n");
        hal_gpio_deinit(HAL_GPIO_36);
        return;
    }

    /* Read the input data of the pin for further validation.*/
    ret = hal_gpio_get_input(HAL_GPIO_36, &data_pull_up);
    if (HAL_GPIO_STATUS_OK != ret) 
	{
        printf("hal_gpio_get_input failed\r\n");
        hal_gpio_deinit(HAL_GPIO_36);
        return;
    }

    /*Step2: Change the pull state to pull-down and read the input data.*/

    /* Configure the pull state to pull-down.*/
    ret = hal_gpio_pull_down(HAL_GPIO_36);
    if (HAL_GPIO_STATUS_OK != ret) 
	{
        printf("hal_gpio_pull_down failed\r\n");
        hal_gpio_deinit(HAL_GPIO_36);
        return;
    }

    /* Read the input data of the pin for further validation.*/
    ret = hal_gpio_get_input(HAL_GPIO_36, &data_pull_down);
    if (HAL_GPIO_STATUS_OK != ret) 
	{
        printf("hal_gpio_get_input failed\r\n");
        hal_gpio_deinit(HAL_GPIO_36);
        return;
    }

    /*Step3: Verify the success of pull state.*/

    if ((data_pull_down == HAL_GPIO_DATA_LOW) && (data_pull_up == HAL_GPIO_DATA_HIGH)) 
	{
        printf("GPIO pull state configuration is successful\r\n");
    } 
	else 
	{
        printf("GPIO pull state configuration failed\r\n");
    }

    ret = hal_gpio_deinit(HAL_GPIO_36);
    if (HAL_GPIO_STATUS_OK != ret) 
	{
        printf("hal_gpio_deinit failed\r\n");
        return;
    }

   printf("\r\n ---gpio_example finished!!!---\r\n");
}

void gpio_application_ini_0(void)
{

  	hal_gpio_init(HAL_GPIO_0);
	hal_pinmux_set_function(HAL_GPIO_0,HAL_GPIO_0_GPIO0 ); // Set the pin tooperate in GPIO mode.
	hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_OUTPUT);
	hal_gpio_set_output(HAL_GPIO_0, HAL_GPIO_DATA_LOW);
	//hal_gpio_deinit(36);
}

void gpio_application_ini_39(void)
{
  	hal_gpio_init(HAL_GPIO_39);
	hal_pinmux_set_function(HAL_GPIO_39, HAL_GPIO_39_GPIO39); // Set the pin tooperate in GPIO mode.
	hal_gpio_set_direction(HAL_GPIO_39, HAL_GPIO_DIRECTION_OUTPUT);
	hal_gpio_set_output(HAL_GPIO_39, HAL_GPIO_DATA_LOW);
	/*hal_gpio_get_input (HAL_GPIO_37,a);
	return *a;*/
	//hal_gpio_deinit(37);
}

void gpio_application_ini_37(void)
{
  	hal_gpio_init(HAL_GPIO_37);
	hal_pinmux_set_function(HAL_GPIO_37, HAL_GPIO_37_GPIO37); // Set the pin tooperate in GPIO mode.
	hal_gpio_set_direction(HAL_GPIO_37, HAL_GPIO_DIRECTION_INPUT);
	/*hal_gpio_get_input (HAL_GPIO_37,a);
	return *a;*/
	//hal_gpio_deinit(37);
}

void SYS_Delay(unsigned int us)
{
	static unsigned char repeat;
	
	// If sys clock is 25M Hz.
	repeat = 25;
	hal_gpio_data_t  a;
	SysTick->CTRL &= ~( 1 | 1 << 16 ); 
	SysTick->LOAD = us;
	SysTick->VAL  = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	while(repeat--)
	{
			
		/* Waiting for down-count to zero */
		while((SysTick->CTRL & (1 << 16)) == 0);
		SysTick->VAL  = 0;
		hal_gpio_get_input (HAL_GPIO_37,&a);
		if(a) break;
	}
}



int main(void)
{
	volatile int cnt=0,bump=0,state=0,flag=0;
	//	unsigned int led[13]={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
	hal_gpio_data_t  button;
    /* Configure system clock. */
    SystemClock_Config();

    /* Configure the hardware. */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();
    /* Add your application code here */
    printf("\r\n\r\n");/* The output UART used by printf is set by log_uart_init() */
    printf("welcome to main()\r\n");
	
	gpio_application_ini_0();
	gpio_application_ini_39();
	gpio_application_ini_37();
			
	sent_data(state);
	
	while(1)
	{
		hal_gpio_get_input (HAL_GPIO_37,&button);
		hal_gpt_delay_ms(30);
		if(button==1&&flag==0)
		{
			flag=1;
			state=state+1;
		}
		else if (button==0&&flag==1)
		{
			flag=0;
			switch(state)
			{
				case 1:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;
				
				case 2:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));	
					break;
				case 3:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;
				case 4:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));	
					break;
				case 5:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;
				case 6:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;
				case 7:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;
				case 8:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;
				case 9:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));	
					break;
				case 10:
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;
				default:
					state=1;
					sent_data(state);
					printf("LED BAR = %d \r\n",(0x3ff>>(10-state)));
					break;				
			}
		}	
	}
}	
