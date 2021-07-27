# OpenCV Coin Detection

--------------------------------------------------------------------------------------------------------------------
// This program seeks to find and identify US coins within a given image. The coins must be pennies, nickels, dimes, or
// quarters and they must be standard front and back (eg. eagle back quarters and lincoln memorial backed pennies).
//
// Project objectives:
//      1. Recognize elliptical objects in an image.
//      2. Identify which elliptical objects are coins, and which are not.
//      3. Determine if the coin is heads-up, or tails-up.
//      4. Identify which type of coin was found (i.e. penny, nickel, dime, or quarter).
//      5. Report on the total value of the coins in the image.
//
// This program will read in all ".jpg" images which are contained in the "Test Images" directory local to the program.
// Any images you do not wish to run should be put into the "Additional Images" folder in the "Test Images" directory. 
// After the input images are processed, the corresponding output images will be saved to the "Output Images" directory
// local to the program and displayed to the screen.
//--------------------------------------------------------------------------------------------------------------------


# Visual Studio Configuration:

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

