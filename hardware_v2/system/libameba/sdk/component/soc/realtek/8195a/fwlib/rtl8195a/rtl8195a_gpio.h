/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


#ifndef _RTL8195A_GPIO_H_
#define _RTL8195A_GPIO_H_

#include "hal_api.h"
#include "hal_gpio.h"

#define GPIO_PORTA_DR           0x00        // data register
#define GPIO_PORTA_DDR          0x04        // data direction
#define GPIO_PORTA_CTRL         0x08        // data source control, we should keep it as default: data source from software

#define GPIO_PORTB_DR           0x0c        // data register
#define GPIO_PORTB_DDR          0x10        // data direction
#define GPIO_PORTB_CTRL         0x14        // data source control, we should keep it as default: data source from software

#define GPIO_PORTC_DR           0x18        // data register
#define GPIO_PORTC_DDR          0x1c        // data direction
#define GPIO_PORTC_CTRL         0x20        // data source control, we should keep it as default: data source from software

//1 Only the PORTA can be configured to generate interrupts
#define GPIO_INT_EN             0x30        // Interrupt enable register
#define GPIO_INT_MASK           0x34        // Interrupt mask
#define GPIO_INT_TYPE           0x38        // Interrupt type(level/edge) register
#define GPIO_INT_POLARITY       0x3C        // Interrupt polarity(Active low/high) register
#define GPIO_INT_STATUS         0x40        // Interrupt status
#define GPIO_INT_RAWSTATUS      0x44        // Interrupt status without mask
#define GPIO_DEBOUNCE           0x48        // Interrupt signal debounce
#define GPIO_PORTA_EOI          0x4c        // Clear interrupt

#define GPIO_EXT_PORTA          0x50        // GPIO IN read or OUT read back
#define GPIO_EXT_PORTB          0x54        // GPIO IN read or OUT read back
#define GPIO_EXT_PORTC          0x58        // GPIO IN read or OUT read back

#define GPIO_INT_SYNC           0x60        // Is level-sensitive interrupt being sync sith PCLK

enum {
    HAL_GPIO_HIGHZ      = 0,
    HAL_GPIO_PULL_LOW   = 1,
    HAL_GPIO_PULL_HIGH  = 2
};

typedef enum
{
	GPIO_Mode_IN				= 0x00, /*!< GPIO Input Mode             */
	GPIO_Mode_OUT				= 0x01, /*!< GPIO Output Mode                */
	GPIO_Mode_INT				= 0x02, /*!< GPIO Interrupt Mode                */
	GPIO_Mode_MAX				= 0x03,
}GPIOMode_TypeDef;

/** 
  * @brief  GPIO Configuration PullUp PullDown enumeration 
  */ 
typedef enum
{ 
	GPIO_PuPd_NOPULL	= 0x00, /*!< GPIO Interrnal HIGHZ */
	GPIO_PuPd_DOWN	= 0x01, /*!< GPIO Interrnal Pull DOWN */
	GPIO_PuPd_UP		= 0x02, /*!< GPIO Interrnal Pull UP */
}GPIOPuPd_TypeDef;

/**
 * @brief Setting interrupt's trigger type
 *
 * Setting interrupt's trigger type
 */
typedef enum
{
	GPIO_INT_Trigger_LEVEL = 0x0, /**< This interrupt is level trigger  */
	GPIO_INT_Trigger_EDGE  = 0x1, /**< This interrupt is edge trigger  */
}GPIOIT_LevelType;

/**
 * @brief Setting interrupt active mode
 *
 * Setting interrupt active mode
 */
typedef enum
{
	GPIO_INT_POLARITY_ACTIVE_LOW  = 0x0, /**< Setting interrupt to low active: falling edge or low level  */
	GPIO_INT_POLARITY_ACTIVE_HIGH = 0x1, /**< Setting interrupt to high active: rising edge or high level */
}GPIOIT_PolarityType;

/**
 * @brief Enable/Disable interrupt debounce mode
 *
 * Enable/Disable interrupt debounce mode
 */
typedef enum
{
	GPIO_INT_DEBOUNCE_DISABLE = 0x0, /**< Disable interrupt debounce  */
	GPIO_INT_DEBOUNCE_ENABLE  = 0x1, /**< Enable interrupt debounce   */
}GPIOIT_DebounceType;


typedef struct {
	GPIOMode_TypeDef	GPIO_Mode;      /*!< Specifies the operating mode for the selected pins. */
	GPIOPuPd_TypeDef	GPIO_PuPd;     /*!< Specifies the operating Pull-up/Pull down for the selected pins. */
	GPIOIT_LevelType		GPIO_ITTrigger;  /**< Interrupt mode is level or edge trigger */    
	GPIOIT_PolarityType	GPIO_ITPolarity; /**< Interrupt mode is high or low active trigger */
	GPIOIT_DebounceType	GPIO_ITDebounce; /**< Enable or disable de-bounce for interrupt */
	u32 GPIO_Pin;    // Pin: [7:5]: port number, [4:0]: pin number
}GPIO_InitTypeDef;

//======================================================
// ROM Function prototype
extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;
#ifndef CONFIG_RELEASE_BUILD_LIBRARIES
static __inline HAL_Status
GPIO_Lock (
    VOID
)
{
    HAL_Status Status;

    if (_pHAL_Gpio_Adapter->EnterCritical) {
        _pHAL_Gpio_Adapter->EnterCritical();
    }

    if(_pHAL_Gpio_Adapter->Locked) {
       Status = HAL_BUSY;
    }
    else {
       _pHAL_Gpio_Adapter->Locked = 1;
       Status = HAL_OK;
    }

    if (_pHAL_Gpio_Adapter->ExitCritical) {
        _pHAL_Gpio_Adapter->ExitCritical();
    }

    return Status;
}


static __inline VOID
GPIO_UnLock (
    VOID
)
{
    if (_pHAL_Gpio_Adapter->EnterCritical) {
        _pHAL_Gpio_Adapter->EnterCritical();
    }

    _pHAL_Gpio_Adapter->Locked = 0;

    if (_pHAL_Gpio_Adapter->ExitCritical) {
        _pHAL_Gpio_Adapter->ExitCritical();
    }
}
#endif  // #ifndef CONFIG_RELEASE_BUILD_LIBRARIES

_LONG_CALL_ extern u32
HAL_GPIO_IrqHandler_8195a(
    IN VOID *pData
);

_LONG_CALL_ extern u32
HAL_GPIO_MbedIrqHandler_8195a(
    IN VOID *pData
);

_LONG_CALL_ HAL_Status 
HAL_GPIO_IntCtrl_8195a(
    HAL_GPIO_PIN  *GPIO_Pin, 
    u32 En
);

_LONG_CALL_ HAL_Status 
HAL_GPIO_Init_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

_LONG_CALL_ HAL_Status 
HAL_GPIO_DeInit_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

_LONG_CALL_ HAL_GPIO_PIN_STATE 
HAL_GPIO_ReadPin_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

_LONG_CALL_ HAL_Status 
HAL_GPIO_WritePin_8195a(
    HAL_GPIO_PIN  *GPIO_Pin,
    HAL_GPIO_PIN_STATE Pin_State
);

_LONG_CALL_ HAL_Status
HAL_GPIO_RegIrq_8195a(
    IN PIRQ_HANDLE pIrqHandle
);

_LONG_CALL_ HAL_Status
HAL_GPIO_UnRegIrq_8195a(
    IN PIRQ_HANDLE pIrqHandle
);

_LONG_CALL_ HAL_Status
HAL_GPIO_UserRegIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin,
    VOID *IrqHandler,
    VOID *IrqData
);

_LONG_CALL_ HAL_Status
HAL_GPIO_UserUnRegIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

_LONG_CALL_ HAL_Status
HAL_GPIO_MaskIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

_LONG_CALL_ HAL_Status
HAL_GPIO_UnMaskIrq_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);

_LONG_CALL_ HAL_Status
HAL_GPIO_IntDebounce_8195a(
    HAL_GPIO_PIN  *GPIO_Pin,
    u8 Enable
);

_LONG_CALL_ u32
HAL_GPIO_GetIPPinName_8195a(
    u32 chip_pin
);

_LONG_CALL_ HAL_Status
HAL_GPIO_PullCtrl_8195a(
    u32 chip_pin,
    u8 pull_type
);

_LONG_CALL_ u32 
GPIO_GetChipPinName_8195a(
    u32 port,
    u32 pin
);

_LONG_CALL_ VOID 
GPIO_PullCtrl_8195a(
    u32 chip_pin,
    u8 pull_type
);

_LONG_CALL_ VOID 
GPIO_Int_SetType_8195a(
    u8 pin_num,
    u8 int_mode
);


_LONG_CALL_  HAL_Status HAL_GPIO_IntCtrl_8195aV02(HAL_GPIO_PIN  *GPIO_Pin, u32 En);
_LONG_CALL_ u32 GPIO_Int_Clear_8195aV02(u32 irq_clr);

HAL_Status
HAL_GPIO_ClearISR_8195a(
    HAL_GPIO_PIN  *GPIO_Pin
);


/********** HAL In-Line Functions **********/

/**
  * @brief  Reads the specified input port pin.
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @retval The input port pin current status(High or Low).
  */
static __inline s32 
HAL_GPIO_ReadPin(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    return (s32)HAL_GPIO_ReadPin_8195a(GPIO_Pin);    
}

/**
  * @brief  Write the specified output port pin.
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @param  Pin_State: The state going to be set to the assigned GPIO pin.
  *
  * @retval None
  */
static __inline VOID 
HAL_GPIO_WritePin(
    HAL_GPIO_PIN  *GPIO_Pin,
    u32 Value
)
{
    HAL_GPIO_WritePin_8195a(GPIO_Pin, (HAL_GPIO_PIN_STATE)Value);
}

/**
  * @brief  To register a user interrupt handler for a specified pin
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @param  IrqHandler: The IRQ handler to be assigned to the specified pin
  *
  * @param  IrqData: The pointer will be pass the the IRQ handler
  *
  * @retval None
  */
static __inline VOID
HAL_GPIO_UserRegIrq(
    HAL_GPIO_PIN  *GPIO_Pin,
    VOID *IrqHandler,
    VOID *IrqData
)
{
    HAL_GPIO_UserRegIrq_8195a(GPIO_Pin, IrqHandler, IrqData);
}

/**
  * @brief  To un-register a user interrupt handler for a specified pin
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @retval None
  */
static __inline VOID
HAL_GPIO_UserUnRegIrq(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    HAL_GPIO_UserUnRegIrq_8195a(GPIO_Pin);
}


/**
  * @brief  Enable/Disable GPIO interrupt
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin initialization.
  *
  * @param  En: Enable (1) or Disable (0)
  *
  * @retval HAL_Status
  */
static __inline VOID
HAL_GPIO_IntCtrl(
    HAL_GPIO_PIN  *GPIO_Pin, 
    u32 En
)
{
    HAL_GPIO_IntCtrl_8195a(GPIO_Pin, En);
}

/**
  * @brief  Mask the interrupt of a specified pin
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @retval None
  */
static __inline VOID
HAL_GPIO_MaskIrq(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    HAL_GPIO_MaskIrq_8195a(GPIO_Pin);
}


/**
  * @brief  UnMask the interrupt of a specified pin
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @retval None
  */
static __inline VOID
HAL_GPIO_UnMaskIrq(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    HAL_GPIO_ClearISR_8195a(GPIO_Pin);
    HAL_GPIO_UnMaskIrq_8195a(GPIO_Pin);
}


#endif  // end of "#define _RTL8195A_GPIO_H_"

