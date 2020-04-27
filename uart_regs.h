#ifndef UART_REGS_H_
#define UART_REGS_H_

// Register definitions of STM8S208 for UART1.

#define UART1_SR (*(volatile uint8_t *)(0x5230))
#define UART1_SR_PE 0
#define UART1_SR_FE 1
#define UART1_SR_NF 2
#define UART1_SR_OR 3
#define UART1_SR_LHE 3
#define UART1_SR_IDLE 4
#define UART1_SR_RXNE 5
#define UART1_SR_TC 6
#define UART1_SR_TXE 7

#define UART1_DR (*(volatile uint8_t *)(0x5231))

#define UART1_BRR1 (*(volatile uint8_t *)(0x5232))
#define UART1_BRR1_DIV4 0
#define UART1_BRR1_DIV5 1
#define UART1_BRR1_DIV6 2
#define UART1_BRR1_DIV7 3
#define UART1_BRR1_DIV8 4
#define UART1_BRR1_DIV9 5
#define UART1_BRR1_DIV10 6
#define UART1_BRR1_DIV11 7

#define UART1_BRR2 (*(volatile uint8_t *)(0x5233))
#define UART1_BRR2_DIV0 0
#define UART1_BRR2_DIV1 1
#define UART1_BRR2_DIV2 2
#define UART1_BRR2_DIV3 3
#define UART1_BRR2_DIV12 4
#define UART1_BRR2_DIV13 5
#define UART1_BRR2_DIV14 6
#define UART1_BRR2_DIV15 7

#define UART1_CR1 (*(volatile uint8_t *)(0x5234))
#define UART1_CR1_PIEN 0
#define UART1_CR1_PS 1
#define UART1_CR1_PCEN 2
#define UART1_CR1_WAKE 3
#define UART1_CR1_M 4
#define UART1_CR1_UARTD 5
#define UART1_CR1_T8 6
#define UART1_CR1_R8 7

#define UART1_CR2 (*(volatile uint8_t *)(0x5235))
#define UART1_CR2_SBK 0
#define UART1_CR2_RWU 1
#define UART1_CR2_REN 2
#define UART1_CR2_TEN 3
#define UART1_CR2_ILIEN 4
#define UART1_CR2_RIEN 5
#define UART1_CR2_TCIEN 6
#define UART1_CR2_TIEN 7

#define UART1_CR3 (*(volatile uint8_t *)(0x5236))
#define UART1_CR3_LBCL 0
#define UART1_CR3_CPHA 1
#define UART1_CR3_CPOL 2
#define UART1_CR3_CLKEN 3
#define UART1_CR3_STOP0 4
#define UART1_CR3_STOP1 5
#define UART1_CR3_LINEN 6

#define UART1_CR4 (*(volatile uint8_t *)(0x5237))
#define UART1_CR4_ADD0 0
#define UART1_CR4_ADD1 1
#define UART1_CR4_ADD2 2
#define UART1_CR4_ADD3 3
#define UART1_CR4_LBDF 4
#define UART1_CR4_LBDL 5
#define UART1_CR4_LBDIEN 6

#define UART1_CR5 (*(volatile uint8_t *)(0x5238))
#define UART1_CR5_IREN 1
#define UART1_CR5_IRLP 2
#define UART1_CR5_HDSEL 3
#define UART1_CR5_NACK 4
#define UART1_CR5_SCEN 5

#define UART1_GTR (*(volatile uint8_t *)(0x5239))
#define UART1_PSCR (*(volatile uint8_t *)(0x523A))

#endif // UART_REGS_H_
