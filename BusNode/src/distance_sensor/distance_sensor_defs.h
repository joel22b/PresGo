#define I2C_ADDRESS 0x52

/** Uncomment this line to use the board as master, if not it is used as slave */
//#define MASTER_BOARD

#define MASTER_REQ_READ    0x12
#define MASTER_REQ_WRITE   0x34

#if defined(STEVAL_IDB011V1)
#define I2Cx                                            I2C2
#define I2Cx_SCL_PORT                                   GPIOA
#define I2Cx_SCL_PIN                                    GPIO_PIN_13
#define I2Cx_SCL_AF                                     GPIO_AF0_I2C2
#define I2Cx_SDA_PORT                                   GPIOA
#define I2Cx_SDA_PIN                                    GPIO_PIN_14
#define I2Cx_SDA_AF                                     GPIO_AF0_I2C2
#define __HAL_RCC_I2Cx_CLK_ENABLE                       __HAL_RCC_I2C2_CLK_ENABLE
#define __HAL_RCC_I2Cx_CLK_DISABLE                      __HAL_RCC_I2C2_CLK_DISABLE
#define __HAL_RCC_I2Cx_SCL_GPIO_CLK_ENABLE              __HAL_RCC_GPIOA_CLK_ENABLE
#define __HAL_RCC_I2Cx_SDA_GPIO_CLK_ENABLE              __HAL_RCC_GPIOA_CLK_ENABLE
#define I2Cx_IRQn                                       I2C2_IRQn
#define I2Cx_IRQHandler                                 I2C2_IRQHandler
#define DMA_REQUEST_I2Cx_TX                             DMA_REQUEST_I2C2_TX
#define DMA_REQUEST_I2Cx_RX                             DMA_REQUEST_I2C2_RX
#endif /* STEVAL_IDB011V1 */

#if defined(STEVAL_IDB012V1)
#define I2Cx                                            I2C1
#define I2Cx_SCL_PORT                                   GPIOB
#define I2Cx_SCL_PIN                                    GPIO_PIN_6
#define I2Cx_SCL_AF                                     GPIO_AF0_I2C1
#define I2Cx_SDA_PORT                                   GPIOB
#define I2Cx_SDA_PIN                                    GPIO_PIN_7
#define I2Cx_SDA_AF                                     GPIO_AF0_I2C1
#define __HAL_RCC_I2Cx_CLK_ENABLE                       __HAL_RCC_I2C1_CLK_ENABLE
#define __HAL_RCC_I2Cx_CLK_DISABLE                      __HAL_RCC_I2C1_CLK_DISABLE
#define __HAL_RCC_I2Cx_SCL_GPIO_CLK_ENABLE              __HAL_RCC_GPIOB_CLK_ENABLE
#define __HAL_RCC_I2Cx_SDA_GPIO_CLK_ENABLE              __HAL_RCC_GPIOB_CLK_ENABLE
#define I2Cx_IRQn                                       I2C1_IRQn
#define I2Cx_IRQHandler                                 I2C1_IRQHandler
#define DMA_REQUEST_I2Cx_TX                             DMA_REQUEST_I2C1_TX
#define DMA_REQUEST_I2Cx_RX                             DMA_REQUEST_I2C1_RX
#endif /* STEVAL_IDB012V1   */


#define DS_MODEL_ID		0xEA
#define DS_MODULE_TYPE	0xCC

#define DS_DISTANCE_IN_DOORWAY_THESHOLD	500
