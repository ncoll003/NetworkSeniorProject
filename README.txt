Tyler Beveridge
Nicholas Collier
Christopher Jaress

CS179I Project README
---------------------

How to Run
----------
Door: ./door <server IP> <port>
Control: ./control <port>
Android Application: Run Executable

The Control program will start running on the specified port.  Then it will wait for doors to connect before accepting connections to phones.  The door is ran by specifiying the control's IP and port number.  The door will then connect
to the control center and will wait further instructions from the control.  The android application will then launch and connect to the control as a phone process.  Then the phone and control will exchange data.  If everything is
authorized the control will send the okay down to the door and the sequence of lights will be output.  A final transaction with the phone and control will be sent with the sequence of lights.  If the two match the door unlocks, if not
it will stay unlocked.  If the door gets unlocked there will be a 10 second time limit to open the door before it locks again.



Door.cpp
--------------

The door application connects to the control center before any phone connects.  Then the control relays information down to the door and when the door is prompted by the control center it outputs a sequence of green and red lights.
From here the door will then receive the inputted sequence from the user and if it matches it will unlock, if not it will stay locked.


Control.cpp
-----------

This is the main hub of the project. It is the controller for the doors. It takes input from the android application, checks to make sures its correct, and tells the doors what to output. It is written in C++. It tells the doors to either blink lights or open/close. It also parses information frm the android application. It has a the decryption key for an RSA to decode message sent by the android application.


Android Application 
-------------------

Description:

A very simple android application that takes 3 different inputs. Each input has a button press to send the information to the controller. First you must enter your ID to verify that your information is in the database and you can access doors. Second you must enter which door you want to enter. If you are allowed access to that door, the lights on the correct door will blink and you put the sequence into the last box. It sends the first message encrypted. Once this message is verified, it sends the next few messages to finish access with the control.cpp. Red = 0 Green = 1
