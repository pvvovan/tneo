#include "stm32f1xx.h"

#include "tn.h"


//-- system frequency
#define SYS_FREQ           8000000uL

//-- kernel ticks (system timer) frequency
#define SYS_TMR_FREQ       1000

//-- system timer period (auto-calculated)
#define SYS_TMR_PERIOD     (SYS_FREQ / SYS_TMR_FREQ)

//-- idle task stack size, in words
#define IDLE_TASK_STACK_SIZE          (TN_MIN_STACK_SIZE + 32)

//-- interrupt stack size, in words
#define INTERRUPT_STACK_SIZE          (TN_MIN_STACK_SIZE + 64)

//-- stack sizes of user tasks
#define TASK_A_STK_SIZE    (TN_MIN_STACK_SIZE + 96)
#define TASK_B_STK_SIZE    (TN_MIN_STACK_SIZE + 96)
#define TASK_C_STK_SIZE    (TN_MIN_STACK_SIZE + 96)

//-- user task priorities
#define TASK_A_PRIORITY    7
#define TASK_B_PRIORITY    6
#define TASK_C_PRIORITY    5


//-- Allocate arrays for stacks: stack for idle task
//   and for interrupts are the requirement of the kernel;
//   others are application-dependent.
//
//   We use convenience macro TN_STACK_ARR_DEF() for that.
TN_STACK_ARR_DEF(idle_task_stack, IDLE_TASK_STACK_SIZE);
TN_STACK_ARR_DEF(interrupt_stack, INTERRUPT_STACK_SIZE);

TN_STACK_ARR_DEF(task_a_stack, TASK_A_STK_SIZE);
TN_STACK_ARR_DEF(task_b_stack, TASK_B_STK_SIZE);
TN_STACK_ARR_DEF(task_c_stack, TASK_C_STK_SIZE);


//-- task structures
struct TN_Task task_a;
struct TN_Task task_b;
struct TN_Task task_c;


/* System timer ISR */
void SysTick_Handler(void)
{
    tn_tick_int_processing();
}

void SystemInit(void)
{
    ; // No action
}

static void idle_task_callback(void)
{
    ; // No action
}

/* Hardware init: called from main() with interrupts disabled */
static void hw_init(void)
{
   //-- init system timer
   SysTick_Config(SYS_TMR_PERIOD);
}

static void task_b_body(void *par)
{
    (void)par;
    tn_task_sleep(500);
    for ( ; ; ) {
        GPIOC->BSRR = GPIO_BSRR_BS13;
        tn_task_sleep(1000);
    }
}

void task_c_body(void *par)
{
    (void)par;
    for ( ; ; ) {
        GPIOC->BSRR = GPIO_BSRR_BR13;
        tn_task_sleep(1000);
    }
}

/* Application init: called from the first created application task */
void appl_init(void)
{
    //-- configure LED port pins: PC13
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // 10: Output mode, max speed 2 MHz
    GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;
    GPIOC->CRH |= GPIO_CRH_MODE13_1;

    // 00: General purpose output push-pull
    GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;

    //-- initialize various on-board peripherals, such as
    //   flash memory, displays, etc.
    //   (in this sample project there's nothing to init)

    //-- initialize various program modules
    //   (in this sample project there's nothing to init)

    //-- create all the rest application tasks
    tn_task_create(
        &task_b,
        task_b_body,
        TASK_B_PRIORITY,
        task_b_stack,
        TASK_B_STK_SIZE,
        0,
        (TN_TASK_CREATE_OPT_START)
    );

    tn_task_create(
        &task_c,
        task_c_body,
        TASK_C_PRIORITY,
        task_c_stack,
        TASK_C_STK_SIZE,
        0,
        (TN_TASK_CREATE_OPT_START)
    );
}

static void task_a_body(void *par)
{
    (void)par;
    //-- this is a first created application task, so it needs to perform
    //   all the application initialization.
    appl_init();

    //-- and then, let's get to the primary job of the task
    //   (job for which task was created at all)
    for ( ; ; ) {
        tn_task_sleep(500);
    }
}

//-- create first application task(s)
static void init_task_create(void)
{
    //-- task A performs complete application initialization,
    //   it's the first created application task
    tn_task_create(
        &task_a,                   //-- task structure
        task_a_body,               //-- task body function
        TASK_A_PRIORITY,           //-- task priority
        task_a_stack,              //-- task stack
        TASK_A_STK_SIZE,           //-- task stack size (in words)
        0,                         //-- task function parameter
        TN_TASK_CREATE_OPT_START   //-- creation option
    );
}

int main(void)
{
    //-- unconditionally disable interrupts
    tn_arch_int_dis();

    //-- init hardware
    hw_init();

    //-- call to tn_sys_start() never returns
    tn_sys_start(
            idle_task_stack,
            IDLE_TASK_STACK_SIZE,
            interrupt_stack,
            INTERRUPT_STACK_SIZE,
            init_task_create,
            idle_task_callback
            );

    //-- unreachable
    for ( ; ; );
}
