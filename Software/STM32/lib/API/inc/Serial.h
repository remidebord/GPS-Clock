#ifndef __SERIAL_H
#define __SERIAL_H

/* includes ----------------------------------------------------------------- */
#include "stm32f0xx.h"
#include "Common.h"

/* defines ------------------------------------------------------------------ */
#define DEFAULT_BAUDRATE 					(9600)
#define MAX_USART_RX_BUF_LEN			(512)
#define USART_ENDOFFRAME_RX_TIME	(20)	// 20 ms

/* public variables --------------------------------------------------------- */
extern "C"
{
	extern volatile char USART1_usartRxTimer;
	extern volatile char USART2_usartRxTimer;
}

/* class -------------------------------------------------------------------- */
class Serial
{
	private:
		
		GPIO_TypeDef* m_rxPort;
		int m_rxPin;
		int m_rxPinSource;
	
		GPIO_TypeDef* m_txPort;
		int m_txPin;
		int m_txPinSource;
	
		int m_baudrate;
		USART_TypeDef* m_usart;
	
		volatile char* m_rxTimer;
		int m_rxHead;
		int m_rxTail;
		//char* m_rxBuffer;
		char m_rxBuffer[MAX_USART_RX_BUF_LEN];
		
		char* m_txBuffer;
		int* m_txIndex;
		int* m_txLength;
		
		char m_rs485;
		char m_mode;
		char m_timeout;
	
	public:
		
		Serial(USART_TypeDef* USARTx, GPIO_common GPIO_c_rx, GPIO_common GPIO_c_tx, GPIO_common GPIO_c_de = PF0);
		Serial(USART_TypeDef* USARTx, GPIO_common GPIO_c_pin, char mode);
		
		void receive(void);
		void baud(int baudrate);
		void timeout(char value);
		int write(char *buffer, int length);
		int write_b(char *buffer, int length);
		int read(char *buffer);
		char busy(void);
};

#endif /* __SERIAL_H */
