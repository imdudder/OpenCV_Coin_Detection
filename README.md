# OpenCV Coin Detection

This program utilizes the OpenCV library to analyze an input image and identify U.S. coins within the image. Once the program has recognized an object as a coin, it will determine if it is a penny, nickel, dime, or quarter, and identify if it is heads-up or tails-up. Based on these identifications, it will print an output image with all the coins boxed-in and labeled with their type, orientation, and a measure of confidence in their identification. Additionally, the output image will include the total value of the collection.

![coins2_output](https://user-images.githubusercontent.com/79345007/130873942-eda48f90-626e-4f67-b0e2-0391f2debdb8.jpg)

Project objectives:
   1) Recognize elliptical objects in an image.
   2) Identify which elliptical objects are coins, and which are not.
   3) Determine if the coin is heads-up, or tails-up.
   4) Identify which type of coin was found (i.e. penny, nickel, dime, or quarter).
   5) Report on the total value of the coins in the image.

This program will read in all ".jpg" images which are contained in the "Test Images" directory local to the program. Any images you do not wish to run should be put into the "Additional Images" folder in the "Test Images" directory. After the input images are processed, the corresponding output images will be saved to the "Output Images" directory
local to the program and displayed to the screen.

Note that this program is only capable of recognizing pennies, nickels, dimes, and quarters of standard front and back (eg. eagle back quarters and Lincoln memorial backed pennies).


# Visual Studio Configuration

The following steps tell you how to set up the project to run in Visual Studio after installing OpenCV on your computer.

Step One: Switch to x64 debug mode (this can be done with the dropdown menu right next to the 'Local Windows Debugger'
button).

Step Two: Set up the OpenCV Property Sheet
   1) Navigate to View > Property Manager
   2) Select "Add Existing Property Sheet"
   3) Navigate files to find "OpenCV_Debug.props". Select it and click 'Open'

Step Three: Switch Language Standard to C++ 17 to allow use of filesystem
   1) Navigate to Project > Properties
   2) Under "Configuration Properties", navigate to "General".
   3) Switch the C++ Language Standard to ISO C++17 Standard (std:c++17) using the dropdown menu.
   4) Click "Apply", then click "OK" to close the menu.

