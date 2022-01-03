// November 24, 2021
// Written with sample code provided by: Bernie Roehl, August 2021
// Worked together from CMH - committed from Leon's machine

#define GAME

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"

void display_LED (int interval_on, int interval_off, int num_on);

int main(void)
{
    HAL_Init(); // initialize the Hardware Abstraction Layer
    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)
    // initialize the pins to be input, output, alternate function, etc...
    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // on-board LED

#ifdef GAME 
    int round = 3; //start with round 3 (3 flashes)
    int user_input[6][8]; //store user button inputs
    int comp_input[6][8]; //store computer pattern
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)); //wait until the blue button has been pressed

    while(round<=8) { //loop for 8 rounds (entirety of game)

        //////////////////////////////////////////////////////
        ///////DISPLAY LED PATTERN FOR THE CURRENT ROUND//////
        //////////////////////////////////////////////////////

        for(int i=1; i<=round; i++) { //for each round, loop for appropriate number of LED pulses
            srand(HAL_GetTick()); //get new random seed 
            int pulse_len_determiner = random()%2; //number determines if long or short pulse will be displayed
            ///if number even
            if(pulse_len_determiner==0) { 
                comp_input[round-3][i-1] = 2; //store that the pulse that was displayed was a long pattern into computer array
                display_LED(1000, 150, 1); //Turn on LED for 1s
            }
            ///if number is odd
            else { 
                comp_input[round-3][i-1] = 1; //store that the pulse that was displayed was a short pattern into computer array
                display_LED(350, 150, 1); //Turn on LED For 350 ms
            }
        }

        ///////////////////////////////////////////////////
        //AFTER THE LEDS HAVE BEEN DISPLAYED FOR EACH ROUND
        ///////////////////////////////////////////////////

        uint32_t buttonclick1 = HAL_GetTick(); //current time
        uint32_t time_elapsed = 0; //variable to store time passed
        bool lostgame = false; //variable for storing current game status

        while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) { //loops for how long button has not been pressed for
            time_elapsed = HAL_GetTick() - buttonclick1; //set time passed
            if(time_elapsed>3000) { //check if time > 3s
                lostgame=true; break; //if time > 3s, break loop and set game status to game over
            }
        }

        int col = 0; //col for storing user input
        uint32_t time_pressed = 0; //variable for storing when button was pressed
        uint32_t current_time = 0; //variable for storing current time
    
        while(lostgame!=true && col<round) { //loop for input depending on round conditions (if game is not lost and user still needs more input to match pattern displayed)
            time_pressed = HAL_GetTick(); //get current time
            while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) { //loop if button is pressed
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);  //turn on LED for duration of button press
                current_time = HAL_GetTick(); //get current time
            }
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); //turn off LED
            time_elapsed = current_time - time_pressed; //get time button was pressed
            ///Save user input into array
            if(time_elapsed>0 && time_elapsed<=200) { //if button press was short
                user_input[round-3][col] = 1; //set user input to 1
            }
            else if(time_elapsed>200 && time_elapsed<=750) { //if button press was long
                user_input[round-3][col] = 2; //set user input to 2
            }
            else if(time_elapsed>750) { //if button press was too long
                lostgame=true;break; //set game status to lost
            }
            col=col+1; //increment col to save next user input
            if(col==round) {break;} //if last input of round, break out of while loop and check if user moves on to next level
            else { //if not last input
                buttonclick1 = HAL_GetTick(); //get current time
                while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) { //loop for duration button is not pressed
                    time_elapsed = HAL_GetTick() - buttonclick1;  //save time elapsed
                    if(time_elapsed>1000) { //if time between button presses is > 1s
                        lostgame=true; break; //set game status to lost
                    }
                }
            }
        }

        /////////////////////////////////////////
        ////////////CHECK GAME STATUS////////////
        /////////////////////////////////////////

        if(lostgame!=true) { //if game status is not game over
            for(int p = 0; p<round; p++) { //loop for each user input
                if(comp_input[round-3][p] != user_input[round-3][p]) { //if user input doesn't match what was displayed
                    lostgame=true; break; //set game status to game over and break out of loop
                }
            }

            if(lostgame!=true && round<8) { //if game status is not over and not final round
                display_LED(0, 500, 1);
                display_LED(3000, 500, 1); //display that user successfully passed current round by turning on LED for 3s
                round++; //go to next round
            }
            else if (lostgame!=true && round==8) { //if game status is not over and it is final round
                display_LED(0, 500, 1);
                display_LED(100, 100, 15); //display that user won the game with 15 LED flashes
                round++;//increment round so we can break out of round<=8 while loop
            }
        }

        //if user lost the game
        if(lostgame==true) { 
            display_LED(0, 500, 1);
            display_LED(200, 200, 5); //display losing LED pattern (blink 5 times)
            break;
        }   
    }
#endif
    return 0;
}

//Function display_LED essentially controls all the LED display. Turns LED on or off for appropriate intervals and also flashes for desired number of times given by parameters.  
void display_LED (int interval_on, int interval_off, int num_on) {
    for(int i = 1; i<=num_on; i++) { //repeat for number of LED on intervals
        uint32_t time1 = HAL_GetTick(); //get current system time
        uint32_t time2 = time1+interval_on; //get second time depending on how long the LED should be on
        if(interval_on!=0) { //if LED needs to be turned on
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true); //turn on LED
            while(HAL_GetTick()<=time2 && HAL_GetTick()>=time1); //stall for appropriate time
        }
        time1 = HAL_GetTick(); //get current time
        time2 = time1+interval_off; //get second time depending on how long the LED should be off for
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); // turn off LED
        while(HAL_GetTick()<=time2 && HAL_GetTick()>=time1); //stall for appropriate time
    }
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void){HAL_IncTick();} // tell HAL that a new tick has happened
