#ifdef SIMULATION

# include "simulation.h"
# include <signal.h>

/*
** Declare memory registers
*/

# define SIG_NUM_MAX 32
typedef struct s_isr
{
  u8 flag;
  u8 enable;
  u8 priority;
  void (*isr)
} t_isr;

/*
 * SIGUSR1
 * SIGUSR2
 * SIGALRM
 * SIGFPE
 * SIGTTOU
 * SIGTTIN
 * SIGCHLD
 * SIGCONT
 * SIGPIPE
 * SIGHUP
*/

t_isr isr_table[SIG_NUM_MAX] = {
  {0, 0, 0, NULL}
}

void register_signals(void)
{
  (void)Timer3TapButton(void);
  (void)Main_encoder_A_Handler(void);
  (void)MCP_encoders_port_B_Handler(void);
  (void)HT16IntHandler(void);
  (void)Timer5Handler(void);
  (void)I2C1Handler(void);
  (void)Timer2QTime(void);
  (void)SPI1Handler(void);
  (void)Timer4Handler(void);
  (void)UART1_TX_handler(void);
  isr_table[SIGUSR1].isr = &Timer3TapButton;
  isr_table[SIGUSR2].isr = &Main_encoder_A_Handler;
  isr_table[SIGALRM].isr = &MCP_encoders_port_B_Handler;
  isr_table[SIGFPE].isr = &HT16IntHandler;
  isr_table[SIGTTOU].isr = &Timer5Handler;
  isr_table[SIGTTIN].isr = &I2C1Handler;
  isr_table[SIGCHLD].isr = &Timer2QTime;
  isr_table[SIGCONT].isr = &SPI1Handler;
  isr_table[SIGPIPE].isr = &Timer4Handler;
  isr_table[SIGHUP].isr = &UART1_TX_handler;
}


#endif
