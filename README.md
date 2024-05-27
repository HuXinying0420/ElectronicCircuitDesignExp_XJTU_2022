# ElectronicCircuitDesignExp_XJTU_2022

his repository contains the contents and reports of my experiments from the "Electronic Circuit Design Experiment" course at Xi'an Jiaotong University (XJTU) in 2022. This course is a core practical course for students majoring in Electrical Engineering and Automation, which aims to cultivate students' abilities in electronic circuit design, embedded system development, and practical problem-solving.

The experiments cover a wide range of topics, including microcontroller programming, digital logic design, analog circuit design, control system implementation, and even simple game development. Through these hands-on experiments, students can apply the theoretical knowledge learned in the classroom to practical engineering problems, improving their comprehensive abilities in electronic circuit design and embedded system development.

The two tasks presented in this repository are representative examples of the experiments conducted in this course. 

## Task 1: Game Design Experiment based on C8051F020 Microcontroller

This experiment implemented a small game called "Catching the Cherry Blossoms" based on the C8051F020 microcontroller. The main functionalities are:

* **LCD Display Area**:
  * Displays the game's main menu, including the game name and simple rules
  * Displays the position relationship between the character and the cherry blossoms during the game

* **Keyboard Control Area**:
  * The middle key is used to start the game
  * The left and right keys are used to control the character's left and right movement

* **LED Indicator Area**:
  * The first group of LEDs displays the 10-second countdown
  * The second group of LEDs displays the current score
  * The third group of LEDs displays the current total number of rounds

The game process is as follows:
1. Press the middle key to start the game, and the timer starts a 10-second countdown
2. The character and cherry blossoms will appear on the LCD screen, and the player can use the left and right keys to control the character's movement
3. If the character aligns with the cherry blossom, one point is scored, and the score is displayed on the second group of LEDs
4. When a cherry blossom falls, it is considered the end of a round, and the total number of rounds is displayed on the third group of LEDs
5. After the 10-second countdown ends, the game automatically ends and returns to the main menu

## Task 2: Vertical Take-off and Landing Control System Design Experiment based on C8051F020 Microcontroller

This experiment implemented a simulated helicopter vertical take-off and landing control system based on the C8051F020 microcontroller. The main functionalities are:

* **LCD Display Area**:
  * Displays personal information and system menus
  * Can be used as an oscilloscope to display the voltage change curve

* **Keyboard Control Area**:
  * The middle key is used to enter the control submenu
  * The left key is used to select the filter
  * The right key is used to modify the PID parameters

* **LED Indicator Area**:
  * Displays the voltage set value
  * Displays the measured voltage value
  * Displays the control voltage value

The system functions are as follows:
1. After system initialization, the main menu is displayed on the LCD
2. Press any key to enter the system submenu
3. In the control submenu, you can select the filter and modify the PID parameters
4. Switch to the oscilloscope mode to observe the voltage output waveform, and use the left and middle keys to adjust the set value


## Hardware and Software Platform

Hardware Platform:
- Based on the C8051F020 smart controller
- Uses the C8051F020 microcontroller as the core component
- Includes additional modules such as:
  - Keypad
  - Seven-segment displays
  - LCD display
  - AD/DA input/output signal conditioning circuits

Software Platform:
- The design program is written in C language
- Uses the Keil C51 programming software
- The program is divided into multiple modules, including:
  - Main program
  - LED display
  - LCD series programs
  - ...


In summary, these two experiments are both based on the C8051F020 microcontroller platform, involving embedded system design, game development, and control system design, which is a good practical project. 

## Disclaimer
This repository is intended for personal academic use and reference. It is recommended to understand and work through the materials independently before referring to the provided solutions.

Feel free to explore the content and reach out if you have any questions!

---

This article was originally published on GitHub by HuXinying0420 in the repository ElectronicCircuitDesignExp_XJTU_2022 at the following address: [https://github.com/username/repository-name/path/to/file](https://github.com/HuXinying0420/ElectronicCircuitDesignExp_XJTU_2022)

Please note the following when reprinting this content:

1. Source: This article was originally published on GitHub by HuXinying0420.
2. Original URL: https://github.com/HuXinying0420/ElectronicCircuitDesignExp_XJTU_2022
