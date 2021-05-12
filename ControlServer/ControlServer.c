#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

/**
 * Clear the CLI
 *
 */
void clear(){
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
#endif

#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#endif
}

/**
 * Main menu of the application
 * @return response - choice of menu
 */
int callMenu() {
    int response;
    printf("This is the menu, choose a device to manage (1-5) \n");
    printf("********************************************************** \n");
    printf("1. The connected light \n");
    printf("2. The connected washer \n");
    printf("3. Check the gaz sensor state \n");
    printf("4. Check the alarm state \n");
    printf("5. Stop the program \n");
    printf("********************************************************** \n");
    scanf("%d", &response);
    clear();
    return response;
}

/**
 * Call to send the command to the broker for the light
 * @param result on or off
 */
void sendCommandToLight(int result) {
    if(result == 1){
        printf("---------------------------- \n");
        printf("The light is switching ON \n");
        printf("---------------------------- \n \n");
        //Todo send packet in UDP to the server

    }
    else if (result == 2) {
        printf("---------------------------- \n");
        printf("The light is switching OFF \n");
        printf("---------------------------- \n \n");
        //Todo send packet in UDP to the server

    }
}

/**
 * Light management
 */
void callLight() {
    int reset = 0;
    int result;
    while (reset == 0){
        printf("CONNECTED LIGHT \n");
        printf("Choose a action (1-3) : \n");
        printf("################################### \n");
        printf("1. ON \n");
        printf("2. OFF \n");
        printf("3. Return in menu \n");
        printf("################################### \n");
        scanf("%d", &result);
        clear();
        switch(result){
            case 1:
                sendCommandToLight(result);
                reset = 1;
                break;
            case 2:
                sendCommandToLight(result);
                reset = 1;
                break;
            case 3:
                reset = 1;
                break;
            default:
                printf("---------------------------- \n");
                printf("Enter a correct number \n");
                printf("---------------------------- \n \n");
                break;
        }
    }
}

/**
 *
 * @param result int of choice
 */
void sendCommandToWasher(int result) {
    if(result == 1){
        printf("---------------------------- \n");
        printf("A new cycle is sending... \n");
        printf("---------------------------- \n \n");
        //Todo send packet in UDP to the server
    }
    else if (result == 2) {
        printf("----------------------------------- \n");
        printf("The current cycle is stopping... \n");
        printf("----------------------------------- \n \n");
        //Todo send packet in UDP to the server
    }
}

/**
 * Washer management
 */
void callWasher() {
    int reset = 0;
    int result;
    while (reset == 0){
        //Todo Recover information from broker on the state
        printf("CONNECTED WASHER \n");
        printf("----------------------------------------------------------- \n");
        printf("|The current state is : %s                                | \n", "ON");
        printf("----------------------------------------------------------- \n \n");
        printf("Choose a action (1-3) : \n");
        printf("################################### \n");
        printf("1. Run a new cycle \n");
        printf("2. Stop the current cycle \n");
        printf("3. Return in menu \n");
        printf("################################### \n");
        scanf("%d", &result);
        clear();
        switch(result){
            case 1:
                sendCommandToWasher(result);
                reset = 1;
                break;
            case 2:
                sendCommandToWasher(result);
                reset = 1;
                break;
            case 3:
                reset = 1;
                break;
            default:
                printf("---------------------------- \n");
                printf("Enter a correct number \n");
                printf("---------------------------- \n \n");
                break;
        }
    }
}

/**
 * Gaz Sensor management
 */
void callGazSensor() {
    printf("GAZ SENSOR STATUS \n");
    printf("The current value is represented between 200 to 10000ppm \n");
    printf("The threshold to not exceed is 400ppm: \n");
    printf("----------------------------------------------------------- \n");
    printf("|The current value is :  %d ppm                            | \n", 5);
    printf("----------------------------------------------------------- \n");
    // Todo the value is > than 400, go activate the alarm
    printf("--------------------------------------- \n");
    printf("Enter to return in the main menu... \n");
    printf("--------------------------------------- \n");
    system("pause");
}

/**
 * Alarm management
 */
void callAlarm() {
    printf("ALARM STATUS \n");
    printf("----------------------------------------------------------- \n");
    printf("|The current value is :  %s                              | \n", "OFF");
    printf("----------------------------------------------------------- \n");
    // Todo recover the alarm status from broker
    printf("--------------------------------------- \n");
    printf("Enter to return in the main menu... \n");
    printf("--------------------------------------- \n");
    system("pause");
}

int main() {
    int launch = 0;

    printf("Welcome to the Control Server \n \n");
    while(launch == 0){
        int response = 0;
        response = callMenu();
        switch(response){
            case 1:
                callLight();
                break;
            case 2:
                callWasher();
                break;
            case 3:
                callGazSensor();
                clear();
                break;
            case 4:
                callAlarm();
                clear();
                break;
            case 5:
                printf("---------------------- \n");
                printf("End of program \n");
                printf("---------------------- \n");
                for(int i = 0 ; i < 4; i++){//Sleep 500ms*4
                    printf(".");
#ifdef _WIN32 //For windows
                    Sleep(500);
#else //For linux
                    usleep(500000);
#endif
                }
                launch = 1;
                break;
            default:
                printf("---------------------------------------- \n");
                printf("Pliz enter a number between 1 and 5 \n");
                printf("---------------------------------------- \n \n");
                break;
        }
    }
    return 0;
}




