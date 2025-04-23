/******** !!!Please use EUC encoding!!! ******
 * このファイルはブラウザでも読めるようにUTF-8にしています。
 * 実際に利用する場合はEUCエンコードを設定し下記のファイルのコードをすべてコピー&ペーストしてください。
 *
 * main.c
 * ssd1309.c .h
 * font24.c
 *
 ********************************************/
#include "main.h"
#include "ssd1309.h"

#include <string.h>

static void RCC_Config(void);
static void GPIO_OutputConfig(GPIO_TypeDef *GPIOx,uint32_t Pos);
static void SPI1_Config(void);

int main(void)
{
	char str[100] = "SSD1309\n日本語表示テスト\nSPI接続と\nEUC文字コードで\n表示します";

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	RCC_Config();
	SysTick_Init(MILL_SECOND);
	GPIO_OutputConfig(GPIOA, Pin0);
	SPI1_Config();

	Delay(100);
	SetHandle(SPI1, GPIOA, Pin5);
	OLEDinit(0x1F);

	ClearLCD(Normal);
	StringLCD(str, strlen(str));

	GPIO_WRITE(GPIOA,Pin0);

	while(1)
	{
		;
	}

}
static void RCC_Config(void)
{
	RCC_InitTypedef rcc;
	rcc.Latency = LL_FLASH_LATENCY_2;
	rcc.PLLSrc = LL_RCC_PLLSOURCE_HSI;
	rcc.PLLM = LL_RCC_PLLM_DIV_1;
	rcc.PLLN = 8;
	rcc.PLLR = LL_RCC_PLLR_DIV_2;
	rcc.AHBdiv = LL_RCC_SYSCLK_DIV_1;
	rcc.SysClkSrc = LL_RCC_SYS_CLKSOURCE_PLL;
	rcc.APBdiv = LL_RCC_APB1_DIV_1;
	rcc.clock = 64000000;

	RCC_InitG0(&rcc);
}
static void GPIO_OutputConfig(GPIO_TypeDef *GPIOx,uint32_t Pos)
{
	uint32_t Periphs = 0;
	GPIO_InitTypedef init;
	init.PinPos = Pos;
	init.Pull = LL_GPIO_PULL_NO;
	init.Mode = LL_GPIO_MODE_OUTPUT;
	init.Speed = LL_GPIO_SPEED_FREQ_LOW;
	init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

	Periphs = GetPortNumber(GPIOx);

	if(!Periphs)
	{
		while(1);
	}

	LL_IOP_GRP1_EnableClock(Periphs);
	GPIO_OutputInit(GPIOx, &init);
}
static void SPI1_Config(void)
{
	GPIO_InitTypedef init = {
			.Mode = LL_GPIO_MODE_ALTERNATE,
			.Speed = LL_GPIO_SPEED_FREQ_LOW,
			.OutputType = LL_GPIO_OUTPUT_PUSHPULL,
			.Pull = LL_GPIO_PULL_NO,
			.Alternate = LL_GPIO_AF_0,
	};
	SPI_InitTypedef config = {0};

	if(LL_IOP_GRP1_IsEnabledClock(1 << PORTA) == 0)
	{
		LL_IOP_GRP1_EnableClock(1 << PORTA);
	}

	init.PinPos = Pin1;			//SCK
	GPIO_OutputInit(GPIOA, &init);

	init.PinPos = Pin2;			//MOSI
	GPIO_OutputInit(GPIOA, &init);

	init.PinPos = Pin4;			//NSS
	GPIO_OutputInit(GPIOA, &init);

//	init.PinPos = Pin4;			//SoftNSS
//	init.Mode = LL_GPIO_MODE_OUTPUT;
//	GPIO_OutputInit(GPIOA, &init);

//	init.PinPos = Pin6;			//MISO
//	GPIO_OutputInit(GPIOA, &init);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

	SPI_StructInit(&config, MSTR_MASTER,NSS_AUTO_CONTROL);
	SPI_Init(SPI1, &config);
}
static void TIM14_Begin(uint32_t Presclaer,uint32_t Reload)
{
	uint32_t prc = (Presclaer != 0)? Presclaer-1:1;
	uint32_t load = (Reload != 0)? Reload-1:0;

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);

	LL_TIM_SetClockDivision(TIM14, LL_TIM_CLOCKDIVISION_DIV1);
	LL_TIM_SetPrescaler(TIM14, prc);
	LL_TIM_SetAutoReload(TIM14, load);
	LL_TIM_GenerateEvent_UPDATE(TIM14);

	LL_TIM_ClearFlag_UPDATE(TIM14);

	LL_TIM_EnableARRPreload(TIM14);
}
