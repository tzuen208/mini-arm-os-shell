#include <stddef.h>
#include <stdint.h>
#include "reg.h"
#include "threads.h"
#include <string.h>
#include <stdlib.h>

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE	((uint16_t) 0x0080)


void usart_init(void)
{
	*(RCC_APB2ENR) |= (uint32_t) (0x00000001 | 0x00000004);
	*(RCC_APB1ENR) |= (uint32_t) (0x00020000);

	/* USART2 Configuration, Rx->PA3, Tx->PA2 */
	*(GPIOA_CRL) = 0x00004B00;
	*(GPIOA_CRH) = 0x00000800;
	*(GPIOA_ODR) = 0x00000000;
	*(GPIOA_BSRR) = 0x00000000;
	*(GPIOA_BRR) = 0x00000000;

	*(USART2_CR1) = 0x0000000C;
	*(USART2_CR2) = 0x00000000;
	*(USART2_CR3) = 0x00000000;
	*(USART2_CR1) |= 0x2000;
	*(USART1_CR1) = 0x0000000C;
	*(USART1_CR2) = 0x00000000;
	*(USART1_CR3) = 0x00000000;
	*(USART1_CR1) |= 0x2000;
}




void print_str(const char *str)
{
	while (*str) {
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = (*str & 0xFF);
		str++;
	}
}
void print_chr(const char c)
{
	
		while (!(*(USART2_SR) & USART_FLAG_TXE));
		*(USART2_DR) = ( c & 0xFF);
		
	
}

void print_int(int i)
{	
	int l;
	//char number;
	char buff[11]= {0};
	buff[10] = '\0';
	int b = 9;
	do{
		l = i % 10;
		i = i / 10;
	//	number = l +'0';
		buff[b] = l+ '0';
		b--;
		//print_chr(number);
	}while(i > 0);
	print_str(buff+b+1);
}
static void delay(volatile int count)
{
	count *= 50000;
	while (count--);
}

static void busy_loop(void *str)
{
	while (1) {
		print_str(str);
		print_str(": Running...\n");
		delay(1000);
	}
}

void test1(void *userdata)
{
	busy_loop(userdata);

}

void test2(void *userdata)
{
	busy_loop(userdata);
}

void test3(void *userdata)
{
	busy_loop(userdata);
}

void fib(int f) {
    int previous = -1;
    int result = 1;
    int i=0;
    int sum=0;
    for (i = 0; i <= f; i++) {
        sum = result + previous;
        previous = result;
        result = sum;
    }
	print_int(result);
  //  return result;
}
void findGCD(int a, int b) {
    //int a = 33, b = 9999;
    while (1) {
        if (a > b) a -= b;
        else if (a < b) b -= a;
        else{
		print_int(a);
		break;
	}
    }
}
/* 72MHz */
#define CPU_CLOCK_HZ 72000000

/* 100 ms per tick. */
#define TICK_RATE_HZ 10
int find_cmd( char* cmd){
	char *delim = " ";
	char *c;
	char *para;
	c = strtok(cmd, delim);
	para = strtok(NULL, delim);
	//print_str(c);
	//print_str(para);
	if( strncmp(c, "start", 5) == 0	){
		const char *str1 = "Task1";
		thread_create(test1, (void *) str1);
		return 1;
	}
	else if( strncmp(c, "task2", 5) == 0	){
		const char *str1 = "Task2";
		thread_create(test2, (void *) str1);
		return 1;
	}
	else if( strncmp(c, "gcd", 3) == 0	){
		char *para2 = strtok(NULL, delim);
		//print_str(para);
		//print_str(para2);
		int *a = (int *)atoi(para);
		int *b = (int *)atoi(para2);
		thread_create_int2(findGCD, a, b);
		return 1;
	}
	else if( strncmp(c, "fib", 3) == 0	){
		int *f = (int *)atoi(para);
		thread_create_int(fib, f);
		return 1;
	}
	return 0;
		
}
void shell(){
	char c[20] = {0}; 
	int i = 0;
	int x;
	print_str("shell:~$ ");
	while(1){
		if(*(USART1_SR) & 0x0020){
			c[i] = *(USART1_DR) & 0x0FF;
			if( c[i] == 0x0d){
				i = 0;
		//		print_chr('\n');
		//		print_str(c);
				if(find_cmd(c))
					print_chr('\n');
		//		break;
				else{
					print_str("\nshell:~$ ");
				}
			}
			else{
				if( c[i] == 0x8 || c[i] ==0x7f ){
					c[i] = '\0';
					c[i-1] = ' ';
					for( x = 0; x<i; x++){
						print_str("\b");
					}
					print_str(c);
					print_str("\b");
					i--;
				}else{
					print_chr(c[i]);
					i++;
				}
			}
		}
	}
}
int main(void)
{

	usart_init();
	const char *str0 = "SHELL";
       	thread_create(shell, (void *) str0);
	/* SysTick configuration */
	//*SYSTICK_LOAD = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
	*SYSTICK_LOAD = 7200000;
	
	*SYSTICK_VAL = 1;
	*SYSTICK_CTRL = 0x07;
	__asm__("cpsie i");

	thread_start();

	return 0;
}
