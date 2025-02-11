/*
 * Kwajo Burrs, Hezekiah Destinoble
 * Spring 2025
 * Section 2
 * 01/29/2025
 * LAb2
 *
 ******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define PORT1 GPIO_PORT_P1
#define PORT2 GPIO_PORT_P2
#define PIN0 GPIO_PIN0
#define PIN1 GPIO_PIN1
#define PIN4 GPIO_PIN4
#define PIN2 GPIO_PIN2

typedef enum Led1State {OFF1, ONE, TWO, FOUR} Led1State;
typedef enum Led2State {OFF2, YELLOW, CYAN, MAGENTA} Led2State;

uint32_t timer = TIMER_A0_BASE;

Led1State led1State;
Led2State led2State;
uint16_t clockPeriodArray[4]={30000, 37500, 18750, 9375};
uint16_t clockDividerArray[4]={40, 40, 40, 40};
uint16_t stateCounter0 = 0, stateCounter1 = 0;

void configIO(void);
void pushButtons(void);
void toggleLEDs(void);
//void myTimeDelay(led1State);//---------old my time delay prototype
void myTimeDelay(uint32_t timer, uint16_t clockPeriod, uint16_t clockDivider);

uint32_t delay;
Timer_A_UpModeConfig timerConfig;

//![Simple GPIO Config]
int main(void)
{
    volatile uint32_t ii;

    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();

    configIO();
    led1State = OFF1;
    led2State = OFF2;


    //---------------------while statement begins----------------------------------------
    while (1)
    {

      pushButtons();
      toggleLEDs();
      //myTimeDelay(led1State);
      myTimeDelay(timer, clockPeriodArray[stateCounter0], clockDividerArray[stateCounter1]);

    }
    //-----------------------------------while statement ends-----------------------------------------------------

}
//![Simple GPIO Config]
void myTimeDelay(uint32_t timer, uint16_t clockPeriod, uint16_t clockDivider)
{
    if(led1State == ONE)
    {
        stateCounter0 = 1;
    }
    else if(led1State == TWO)
    {
        stateCounter0 = 2;
    }
    else if(led1State == FOUR)
    {
        stateCounter0 = 3;
    }
    else if(led1State == OFF1)
    {
        stateCounter0 = 0;
    }


    timerConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    timerConfig.clockSourceDivider = clockDivider;
    timerConfig.timerPeriod = clockPeriod;
    timerConfig.timerClear = TIMER_A_DO_CLEAR;

    Timer_A_configureUpMode(timer, &timerConfig);
    Timer_A_startCounter(timer, TIMER_A_UP_MODE);
    while(Timer_A_getInterruptStatus(timer) == TIMER_A_INTERRUPT_NOT_PENDING);
    Timer_A_stopTimer(timer);
    Timer_A_clearInterruptFlag(timer);
}
//---------old my time delay function
/*
void myTimeDelay(led1State)
{
    if(led1State == ONE){
        delay = 3000000;
    }
    else if (led1State == TWO){
        delay = 1500000;
    }
    else if (led1State == FOUR){
        delay = 750000;
    }
    else{
        delay = 3000000;
    }

    const uint32_t COUNTFLAG = BIT(16);
    MAP_SysTick_setPeriod(delay); // Set Counter Value
    MAP_SysTick_enableModule(); // Turn ON Counter
    // Poll COUNTFLAG until it is ONE
    while((SysTick->CTRL & COUNTFLAG) == 0);
    MAP_SysTick_disableModule(); //Turn OFF Counter and return
}
*/
void configIO(void)
{



    /* Configuring Outputs */
    MAP_GPIO_setAsOutputPin(PORT1, PIN0); //P1.0
    MAP_GPIO_setAsOutputPin(PORT2, PIN0); //P2.0
    MAP_GPIO_setAsOutputPin(PORT2, PIN1); //P2.1
    MAP_GPIO_setAsOutputPin(PORT2, PIN2); //P2.2

    //Enable Interrupts for Switches (S1, S2)
    MAP_GPIO_enableInterrupt(PORT1, PIN1);
    MAP_GPIO_enableInterrupt(PORT1, PIN4);

    //HIGH-TO-LOW TRANSITIONS
    MAP_GPIO_interruptEdgeSelect(PORT1, PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_GPIO_interruptEdgeSelect(PORT1, PIN4, GPIO_HIGH_TO_LOW_TRANSITION);

    MAP_GPIO_setAsInputPinWithPullUpResistor(PORT1, PIN1); //P1.1
    MAP_GPIO_setAsInputPinWithPullUpResistor(PORT1, PIN4); //P1.4

    //Clear Interrupt Flags
    MAP_GPIO_clearInterruptFlag(PORT1, PIN1);
    MAP_GPIO_clearInterruptFlag(PORT1, PIN4);


    //Set All Outputs to LOW
    MAP_GPIO_setOutputLowOnPin(PORT1, PIN0); //P1.0
    MAP_GPIO_setOutputLowOnPin(PORT2, PIN0); //P2.0
    MAP_GPIO_setOutputLowOnPin(PORT2, PIN1); //P2.0
    MAP_GPIO_setOutputLowOnPin(PORT2, PIN2); //P2.0


}

void pushButtons(void){
    //PUSH BUTTON S1
    if((GPIO_getInterruptStatus(PORT1, PIN1)) != 0)
    {
        if(led1State == OFF1)
        {
            led1State = ONE;
        }
        else if (led1State == ONE)
        {
            led1State = TWO;
        }
        else if (led1State == TWO)
        {
            led1State = FOUR;
        }
        else if(led1State == FOUR)
        {
            led1State = OFF1;
        }

        MAP_GPIO_clearInterruptFlag(PORT1, PIN1);

    }

    //PUSH BUTTON S2
    if((GPIO_getInterruptStatus(PORT1, PIN4)) != 0)
    {
        if(led2State == OFF2)
        {
            led2State = YELLOW;
        }
        else if (led2State == YELLOW)
        {
            led2State = CYAN;
        }
        else if (led2State == CYAN)
        {
            led2State = MAGENTA;
        }
        else
        {
            led2State = OFF2;
        }

        MAP_GPIO_clearInterruptFlag(PORT1, PIN4);
    }

}

void toggleLEDs(void){
    //RED LED1
    if((led1State == ONE) || (led1State == TWO) || (led1State == FOUR))
    {
        MAP_GPIO_toggleOutputOnPin(PORT1, PIN0);
    }
    else if(led1State == OFF1)
    {
        MAP_GPIO_setOutputLowOnPin(PORT1, PIN0);
    }

    //RGB LED2
    if(led2State == YELLOW)
    {
        MAP_GPIO_setOutputHighOnPin(PORT2, PIN0);
        MAP_GPIO_setOutputHighOnPin(PORT2, PIN1);
        MAP_GPIO_setOutputLowOnPin(PORT2, PIN2);
    }
    else if(led2State == CYAN)
    {
        MAP_GPIO_setOutputLowOnPin(PORT2, PIN0);
        MAP_GPIO_setOutputHighOnPin(PORT2, PIN1);
        MAP_GPIO_setOutputHighOnPin(PORT2, PIN2);
    }
    else if(led2State == MAGENTA)
    {
        MAP_GPIO_setOutputHighOnPin(PORT2, PIN0);
        MAP_GPIO_setOutputLowOnPin(PORT2, PIN1);
        MAP_GPIO_setOutputHighOnPin(PORT2, PIN2);
    }
    else
    {
        MAP_GPIO_setOutputLowOnPin(PORT2, PIN0 | PIN1 | PIN2);
    }

}

