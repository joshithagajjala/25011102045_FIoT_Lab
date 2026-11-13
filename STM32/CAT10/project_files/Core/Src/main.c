/* USER CODE BEGIN Header */
/* Task 9C: STM32 - Wemos (ESP8266) Wi-Fi connection + web server using AT commands */
/* USER CODE END Header */

#include "main.h"
#include <string.h>
#include <stdio.h>

UART_HandleTypeDef huart1;   // PC  (PA9 TX, PA10 RX)
UART_HandleTypeDef huart2;   // Wemos (PA2 TX, PA3 RX)

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

/* one reply buffer for every command, so each reply can be used on its own */
char rxAT[100];
char rxMode[100];
char rxDhcp[100];
char rxJoin[200];
char rxIP[200];
char rxMux[100];
char rxServer[100];
char rxReq[700];
char rxTmp[100];
char httpResp[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<h1>Hello from STM32</h1>";

int main(void)
{
    char title[] = "=== Task 9C: Connect to Wi-Fi ===\r\n";
    char cmd[40];
    uint8_t c;
    uint16_t i;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    HAL_Delay(2000);   // let the Wemos boot

    HAL_UART_Transmit(&huart1, (uint8_t *)title, strlen(title), 1000);

    /* 1. basic response */
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT\r\n", 4, 1000);
    HAL_UART_Receive(&huart2, (uint8_t *)rxAT, sizeof(rxAT) - 1, 1000);
    HAL_UART_Transmit(&huart1, (uint8_t *)rxAT, strlen(rxAT), 1000);

    /* 2. station mode */
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CWMODE=1\r\n", 13, 1000);
    HAL_UART_Receive(&huart2, (uint8_t *)rxMode, sizeof(rxMode) - 1, 2000);
    HAL_UART_Transmit(&huart1, (uint8_t *)rxMode, strlen(rxMode), 1000);

    /* 3. enable DHCP */
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CWDHCP=1,1\r\n", 15, 1000);
    HAL_UART_Receive(&huart2, (uint8_t *)rxDhcp, sizeof(rxDhcp) - 1, 2000);
    HAL_UART_Transmit(&huart1, (uint8_t *)rxDhcp, strlen(rxDhcp), 1000);

    /* 4. connect to Wi-Fi */
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CWJAP=\"SNUC\",\"snu12345\"\r\n", 28, 1000);
    HAL_UART_Receive(&huart2, (uint8_t *)rxJoin, sizeof(rxJoin) - 1, 15000);
    HAL_UART_Transmit(&huart1, (uint8_t *)rxJoin, strlen(rxJoin), 1000);

    /* 5. IP address  (reply is stored in rxIP) */
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CIFSR\r\n", 10, 1000);
    HAL_UART_Receive(&huart2, (uint8_t *)rxIP, sizeof(rxIP) - 1, 2000);
    HAL_UART_Transmit(&huart1, (uint8_t *)rxIP, strlen(rxIP), 1000);

    /* 6. allow multiple connections */
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CIPMUX=1\r\n", 13, 1000);
    HAL_UART_Receive(&huart2, (uint8_t *)rxMux, sizeof(rxMux) - 1, 2000);
    HAL_UART_Transmit(&huart1, (uint8_t *)rxMux, strlen(rxMux), 1000);

    /* 7. start TCP server on port 80 */
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CIPSERVER=1,80\r\n", 19, 1000);
    HAL_UART_Receive(&huart2, (uint8_t *)rxServer, sizeof(rxServer) - 1, 2000);
    HAL_UART_Transmit(&huart1, (uint8_t *)rxServer, strlen(rxServer), 1000);

    while (1)
    {
        /* wait for a browser request, keep reading until the line is quiet */
        memset(rxReq, 0, sizeof(rxReq));
        i = 0;
        while (i < sizeof(rxReq) - 1 &&
               HAL_UART_Receive(&huart2, &c, 1, (i == 0) ? 60000 : 300) == HAL_OK)
            rxReq[i++] = c;

        if (strstr(rxReq, "+IPD") != NULL)
        {
            /* show the request on the PC:  +IPD,0,415:GET / HTTP/1.1 ... */
            HAL_UART_Transmit(&huart1, (uint8_t *)rxReq, strlen(rxReq), 1000);

            /* send the HTTP response on connection 0 */
            sprintf(cmd, "AT+CIPSEND=0,%d\r\n", (int)strlen(httpResp));
            HAL_UART_Transmit(&huart2, (uint8_t *)cmd, strlen(cmd), 1000);
            memset(rxTmp, 0, sizeof(rxTmp));
            HAL_UART_Receive(&huart2, (uint8_t *)rxTmp, sizeof(rxTmp) - 1, 500);   // wait for '>'

            HAL_UART_Transmit(&huart2, (uint8_t *)httpResp, strlen(httpResp), 1000);
            memset(rxTmp, 0, sizeof(rxTmp));
            HAL_UART_Receive(&huart2, (uint8_t *)rxTmp, sizeof(rxTmp) - 1, 1500);  // SEND OK
            HAL_UART_Transmit(&huart1, (uint8_t *)rxTmp, strlen(rxTmp), 1000);

            /* close the connection so the browser finishes loading */
            HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CIPCLOSE=0\r\n", 15, 1000);
            memset(rxTmp, 0, sizeof(rxTmp));
            HAL_UART_Receive(&huart2, (uint8_t *)rxTmp, sizeof(rxTmp) - 1, 500);
        }
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}