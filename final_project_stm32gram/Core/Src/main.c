/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "delay.h"
//#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "24l01.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define HEAD_LEN 2
#define HEART_BEAT (u8) 65
#define PAYLOAD (u8) 66
#define LAST (u8) 67
#define NOT_LAST (u8) 68

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t rxBuffer[20];
u8 key, mode;
u16 t = 0;
u8 send_buf[1024];
u8 rev_buf[1024];
u8 test[20] = {HEART_BEAT, LAST, 'T', 'E', 'S', 'T', 0};
int isChanged = 0;
int isOn = 0;
//360*240 display
int display_frame_upper = 110;
int display_frame_lower = 280;
int display_frame_left = 5;
int display_frame_right = 235;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void send(void);
void receive(void);
int onAndOff(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//display power icon
int diaplay_power_icon(on){
	POINT_COLOR = BLACK;
	LCD_ShowString(display_frame_right-64,10, 36, 24, 24, "PWR");

	if (on == 1){
		LCD_Fill(display_frame_right-24,10,display_frame_right,10+24,GREEN);
	} else{
		LCD_Fill(display_frame_right-24,10,display_frame_right,10+24,RED);
	}
	LCD_DrawRectangle(display_frame_right-24,10,display_frame_right,10+24);
}

int diaplay_nrf24_status(on){
	POINT_COLOR = BLACK;
	LCD_ShowString(display_frame_right-90 ,40, 78, 24, 24, "NRF24");
	if (on == 1){
		LCD_Fill(display_frame_right-24,40,display_frame_right,40+24,GREEN);
	} else{
		LCD_Fill(display_frame_right-24,40,display_frame_right,40+24,RED);
	}
	LCD_DrawRectangle(display_frame_right-24,40,display_frame_right,40+24);
}

int diaplay_connection_status(on){
	POINT_COLOR = BLACK;
	LCD_ShowString(display_frame_right-78 ,40+6+24, 78, 24, 24, "PAIR");
	if (on == 1){
		//LCD_ShowString(30, 70, 200, 16, 16, "NRF24L01 STATUS: 1234567");
		LCD_Fill(display_frame_right-24,40+6+24,display_frame_right,40+24+6+24,GREEN);
	} else if (on ==0){
		LCD_Fill(display_frame_right-24,40+6+24,display_frame_right,40+24+6+24,RED);
	} else {
		LCD_Fill(display_frame_right-24,40+6+24,display_frame_right,40+24+6+24,YELLOW);
	}
	LCD_DrawRectangle(display_frame_right-24,40+6+24,display_frame_right,40+24+6+24);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	delay_init(72);
	LED_Init();
	KEY_Init();
	LCD_Init();
	NRF24L01_Init();
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_SPI1_Init();
	/* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, (uint8_t*) rxBuffer, 1);

	POINT_COLOR = BLACK;
	LCD_ShowString(display_frame_left, display_frame_upper-30, 200, 24, 24, "STM32gram");
	//pwr status
	LCD_DrawRectangle(display_frame_right-24,10,display_frame_right,10+24);
	LCD_DrawRectangle(display_frame_left, display_frame_upper, display_frame_right, display_frame_lower);
	diaplay_connection_status(-1);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
//	//init the pair test array
//	test[0]='T';
//	test[1]='E';
//	test[2]='S';
//	test[3]='T';
//	test[4]=0;



	POINT_COLOR = RED;
	while (NRF24L01_Check()) {
		//LCD_ShowString(30, 70, 200, 16, 16, "NRF24L01 STATUS: Error");
		diaplay_nrf24_status(0);
		//delay_ms(200);
		//LCD_Fill(30, 70, 239, 70 + 16, WHITE);
		//delay_ms(200);
	}
	POINT_COLOR = BLACK;
	diaplay_nrf24_status(1);
	diaplay_power_icon(0);
	NRF24L01_TX_Mode();
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		onAndOff();
		if(isOn){
			if(isChanged){
				NRF24L01_TX_Mode();
				LED1=!LED1;
				send();
				LED1=!LED1;
				NRF24L01_RX_Mode();
			}
			receive();

			t++;
			if (t == 60000){
				NRF24L01_TX_Mode();
				LED1=!LED1;
				if(NRF24L01_TxPacket(test) != TX_OK){

					diaplay_connection_status(0);
				}else{

					diaplay_connection_status(1);
				}
				LED1=!LED1;
				NRF24L01_RX_Mode();
				t = 0;
			}
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */
char data[8][1024] = {{0},{0},{0},{0},{0},{0},{0},{0}};
int len[8] = {0};
int fir = 0;

//display the received data
void reload(u8* input,int isSend){
	//LCD_Fill(30, 295, 230, 295 + 16, WHITE);//NOTE clean
	//LCD_Fill(35, 135, 205, 291, WHITE);//content clean
	LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean
	int ptr1 = 2;// 0
	int ptr2 = 0;
	while(input[ptr1]){
		data[fir][ptr2]=input[ptr1];
		ptr1++;
		ptr2++;
		if(ptr2==21){
			len[fir]=isSend*22;//if isSend. length == 22, receive length == -2
			fir = (fir+1)%9;
			ptr2 = 0;
		}
	}
	len[fir]=isSend*ptr2;
	fir = (fir+1)%8;

	for(int i = 0;i<8;i++){
		if(len[(fir+i)%8]>0){
			POINT_COLOR = GREEN;
			LCD_ShowString(display_frame_right-5-8*len[(fir+i)%8], display_frame_upper+5+i*20, 200, 16, 16, (uint8_t*) data[(fir+i)%8]);
		}else{
			POINT_COLOR = BLUE;
			LCD_ShowString(display_frame_left + 10, display_frame_upper+5+i*20, 200, 16, 16, (uint8_t*) data[(fir+i)%8]);
		}
	}

}

int getCommand(char* input, int length){
	if(length==3&&input[0]=='O'&&input[1]=='N'){
		if(!isOn){//already connect
			isOn = 1;
			LED0 = !LED0;
			POINT_COLOR = BLACK;
			LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean
			diaplay_power_icon(1);
			NRF24L01_RX_Mode();
			return 1;
		}else{
			POINT_COLOR = RED;
			LCD_Fill(30, 295, 230, 295 + 16, WHITE);//lower banner clean
			LCD_ShowString(30, 295, 200, 16, 16, "Already ON");
			return 2;//succesfully connect
		}
	}else if(length == 4&&input[0]=='O'&&input[1]=='F'&&input[2]=='F'){
		if(isOn){
			isOn = 0;
			LED0 = !LED0;
			POINT_COLOR = BLACK;
			LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean
			//LCD_Fill(30, 110, 230, 110 + 16, WHITE);//connection status
			diaplay_power_icon(0);
			NRF24L01_TX_Mode();
			LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean
			fir = 0;
			for(int i =0;i<8;i++){
				len[i]=0;
				data[i][0]=0;
			}
			return 3;//successfully disconnect
		}else{
			POINT_COLOR = RED;
			LCD_Fill(30, 295, 230, 295 + 16, WHITE);
			LCD_ShowString(30, 295, 200, 16, 16, "Already OFF");
			return 4;//already disconnect
		}
	}else if(length == 6&&input[0]=='C'&&input[1]=='L'&&input[2]=='E'&&input[3]=='A'&&input[4]=='R'){
		//LCD_Fill(35, 135, 205, 291, WHITE);
		LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean
		fir = 0;
		for(int i =0;i<8;i++){
		len[i]=0;
		data[i][0]=0;
		}
		return 5;
	}
	return 0;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		static unsigned char uRx_Data[1024] = { 0 };
		static unsigned char uLength = 0;
		if (rxBuffer[0] == '\n') {
			HAL_UART_Transmit(&huart1, uRx_Data, uLength, 0xffff);
			if(getCommand(uRx_Data,uLength)==0){
				if(!isOn){
					POINT_COLOR = RED;
					LCD_Fill(30, 295, 230, 295 + 16, WHITE);
					LCD_ShowString(30, 295, 200, 16, 16, "Turn on first!");
				}else{
					if(uLength + 2 > TX_PLOAD_WIDTH){
						POINT_COLOR = RED;
						LCD_Fill(30, 295, 230, 295 + 16, WHITE);
						LCD_ShowString(30, 295, 200, 16, 16, "Input <= 32!");
					}else{
						send_buf[0] = PAYLOAD;
						send_buf[1] = LAST;
						for(int i = 0;i<1024;i++){
							if(i==uLength) {
								send_buf[i + 2]=0;
								break;
							}
							send_buf[i + 2]=uRx_Data[i];
						}
						isChanged = 1;
					}
//					LCD_ShowString(30, 190, 200, 16, 16, "isChanged is 1");
				}
			}
			uLength = 0;
		} else {
			uRx_Data[uLength] = rxBuffer[0];
			uLength++;
		}
	}
}
void send() {
	if (NRF24L01_TxPacket(send_buf) == TX_OK) {
		if(isChanged){
			isChanged = 0;
			reload(&send_buf,1);
		}
	} else {
		POINT_COLOR = RED;
		LCD_Fill(30, 295, lcddev.width, 295 + 16 * 3, WHITE); //清空显示
		LCD_ShowString(30, 295, lcddev.width - 1, 32, 16, "Send Failed!");
		isChanged = 0;
	};
}
void receive(){
	if (NRF24L01_RxPacket(rev_buf) == 0) //收到信息,则显示出
	{
		if(rev_buf[0] == HEART_BEAT) {
			return;
		}
//		if(rev_buf[0]==test[0]&&rev_buf[1]==test[1]&&rev_buf[2]==test[2]&&rev_buf[3]==test[3]&&rev_buf[4] == test[4]){
//			return;
//		}
		rev_buf[1023] = 0; //加入字符串结束符
		reload(&rev_buf,-1);
	}
}
int onAndOff(){
	key = KEY_Scan(0);
	if (key == KEY0_PRES && !isOn) {
		isOn = 1;
		LED0 = !LED0;
		POINT_COLOR = BLACK;
		LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean

		diaplay_power_icon(1);
		NRF24L01_RX_Mode();
		return 1;//on
	}else if(key == KEY0_PRES && isOn){
		POINT_COLOR = RED;
		LCD_Fill(30, 295, 230, 295 + 16, WHITE);
		LCD_ShowString(30, 295, 200, 16, 16, "Already ON");
		return 2;// already on
	}else if (key == KEY1_PRES && isOn) {
		isOn = 0;
		LED0 = !LED0;
		POINT_COLOR = BLACK;
		LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean


		diaplay_power_icon(0);
		NRF24L01_TX_Mode();
		LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean
		fir = 0;
		for(int i =0;i<8;i++){
			len[i]=0;
			data[i][0]=0;
		}
//		LCD_Fill(30, 170, 239, 170 + 16, WHITE);
		return 3;// off
	}else if(key == KEY1_PRES && !isOn){
		POINT_COLOR = RED;
		LCD_Fill(30, 295, 230, 295 + 16, WHITE);
		LCD_ShowString(30, 295, 200, 16, 16, "Already OFF");
		return 4;//already off
	}else if(key == WKUP_PRES){
		LCD_Fill(display_frame_left+1, display_frame_upper+1, display_frame_right-1, display_frame_lower-1, WHITE);//NOTE clean
		fir = 0;
		for(int i =0;i<8;i++){
			len[i]=0;
			data[i][0]=0;
		}
	}
	return 0;
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
