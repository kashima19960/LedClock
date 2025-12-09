#include "buzzer.h"
#include <stdbool.h>
/*
电路设计采用的是PNP三极管，因此蜂鸣器写低电平响铃，高电平关闭
*/
void buzzer_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = BUZZER_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

void buzzer_toggle(void)
{
    static bool buzzer_state = false;
    if (buzzer_state)
    {
        HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET); 
        buzzer_state = false;
    }
    else
    {
        HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
        buzzer_state = true;
    }
}
