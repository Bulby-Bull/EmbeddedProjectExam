from __future__ import print_function

from os import system, name
from time import sleep


# https://www.geeksforgeeks.org/clear-screen-python/
# clear CLI to remove all precedent commands
def clear():
    # for windows
    if name == 'nt':
        _ = system('cls')

    # for mac and linux(here, os.name is 'posix')
    else:
        _ = system('clear')


# Convert StringToInt to manage the string input
def convertStringToInt(result):
    return int(result)


# Main menu of the application
def callMenu():
    print("This is the menu, choose a device to manage (1-5)")
    print("**********************************************************")
    print("1. The connected light")
    print("2. The connected washer")
    print("3. Check the gaz sensor state")
    print("4. Check the alarm state")
    print("5. Stop the program")
    print("**********************************************************")
    response = input()
    clear()
    if response == '':
        return -1
    return int(response)


# Call to send the command to the broker for the light
def sendCommandToLight(result):
    if result == 1:
        print("----------------------------")
        print("The light is switching ON")
        print("----------------------------")
        print("")
        # Todo send packet in UDP to the server
    elif result == 2:
        print("----------------------------")
        print("The light is switching OFF")
        print("----------------------------")
        print("")
        # Todo send packet in UDP to the server


# Light management
def callLight():
    reset = 0
    while reset == 0:
        print("CONNECTED LIGHT")
        print("Choose a action (1-3) : ")
        print("###################################")
        print("1. ON")
        print("2. OFF")
        print("3. Return in menu")
        print("###################################")
        result = input()
        clear()
        if result == '':
            return -1
        result = convertStringToInt(result)
        if result == 1 or result == 2:
            sendCommandToLight(result)
            reset = 1
        elif result == 3:
            reset = 1
        else:
            print("----------------------------")
            print("Enter a correct number")
            print("----------------------------")
            print("")


# Call to send the command to the broker for the washer
def sendCommandToWasher(result):
    if result == 1:
        print("----------------------------")
        print("A new cycle is sending...")
        print("----------------------------")
        print("")
        # Todo send packet in UDP to the server
    elif result == 2:
        print("----------------------------------")
        print("The current cycle is stopping...")
        print("----------------------------------")
        print("")
        # Todo send packet in UDP to the server


# Washer management
def callWasher():
    reset = 0
    while reset == 0:
        print("CONNECTED WASHER")
        print("-----------------------------------------------------------")
        print("|The current state is : " + "                                  |")
        print("-----------------------------------------------------------")
        # Todo Recover information from broker on the state
        print("Choose a action (1-3) : ")
        print("###################################")
        print("1. Run a new cycle")
        print("2. Stop the current cycle")
        print("3. Return in menu")
        print("###################################")
        result = input()
        clear()
        if result == '':
            return -1
        result = convertStringToInt(result)
        if result == 1 or result == 2:
            sendCommandToWasher(result)
            reset = 1
        elif result == 3:
            reset = 1
        else:
            print("----------------------------")
            print("Enter a correct number")
            print("----------------------------")
            print("")


# Gaz Sensor management
def callGazSensor():
    print("GAZ SENSOR STATUS")
    print("The current value is represented between 200 to 10000ppm")
    print("The threshold to not exceed is 400ppm: ")
    print("-----------------------------------------------------------")
    print("|The current value is : " + "                                  |")
    print("-----------------------------------------------------------")
    # Todo the value is > than 400, go activate the alarm
    print("---------------------------------------")
    print("Enter to return in the main menu...")
    print("---------------------------------------")
    print("")
    result = input()
    clear()
    if result == '':
        return -1


# Alarm management
def callAlarm():
    print("ALARM STATUS")
    print("-------------------------------------")
    # Todo recover the alarm status from broker
    print("|The current status is :            |")
    print("-------------------------------------")
    print("Enter to return in the main menu...")
    result = input()
    if result == '':
        return -1
    clear()


# Main
if __name__ == '__main__':
    launch = 0

    print("Welcome to the Control Server \n")
    while launch == 0:
        response = callMenu()
        print(response)
        if response == 1:
            callLight()
        elif response == 2:
            callWasher()
        elif response == 3:
            callGazSensor()
        elif response == 4:
            callAlarm()
        elif response == 5:
            print("----------------------")
            print("End of program")
            print("----------------------")
            for i in range(0, 4):
                print(".", end='')
                sleep(0.5)
            exit()
        if response > 5 or response < 1:
            print("----------------------------------------")
            print("Pliz enter a number between 1 and 5")
            print("----------------------------------------")
            print("")