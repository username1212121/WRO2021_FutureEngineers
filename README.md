# IORO2021 FutureEngineers
<p>Innopolis Open Robotics Olympiad 2021 Future engineers WRO’s Self-driving vehicle challenge.</p>
<p>We created this robot to participate in World Robot Olympiad 2021 Future Engineers WRO’s Self-driving vehicle challenge.
Our robot is a four-wheeled vehicle with differential rotation of the rear wheelset, it also has Camera and Ultrasonic sensors.</p>
<p>Rulles of World Robot Olympiad 2021 Future Engineers WRO’s Self-driving vehicle challenge can be found here https://wro-association.org/fileadmin/files/challenges/2021-power-bots-energy/WRO-2021-Future-Engineers-01-Game-Rules.pdf</p>

<h3>Major Components and Parts</h3>
	<ul><li>Arduino Uno Board</li>
	<li>Ultrasonic Range Finder HC-SR04</li>
	<li>Applied Robotics TrackingCam</li> 
	<li>Encoders</li></ul>
	
<h3>Arduino Uno Board</h3>
	<p>Arduino/Genuino Uno is a microcontroller board based on the ATmega328P.
	We use it to program our robot.</br>
	About Arduino Uno Board https://www.arduino.cc/en/Main/ArduinoBoardUno/</br>
	To program the Arduino Uno Board, we use the Arduino IDE version 1.8.15, you can get it here https://www.arduino.cc/en/software</p>
  
![Image of ArduinoUnoBoard](https://github.com/Glaurung2000/images/blob/master/1390133.jpg)



<h3>Ultrasonic Range Finder HC-SR04</h3>
	<p>We use three Ultrasonic Range Finders HC-SR04 to determine the distance to the field borders in front of the robot and to the left and to the right of it.
	To process the data come from HC-SR04 to Arduino Uno Board we use NewPing library.</br>
  	You can read about it here https://www.arduino.cc/reference/en/libraries/newping/</br>
	About Ultrasonic sensor http://wiki.amperka.ru/%D0%BF%D1%80%D0%BE%D0%B4%D1%83%D0%BA%D1%82%D1%8B:hc-sr04-ultrasonic-sensor-distance-module</p>
  
![Image of SonarHCSR04](https://github.com/Glaurung2000/images/blob/master/ultrasonic-hc-sr04.1.jpg)
	
<h3>Applied Robotics TrackingCam</h3>
	<p>TrackingCam is a computer vision module which is capable of recognizing both types of objects monochrome and objects consisting of several colors. 
	To configure the camera to recognize certain objects, you need to install a special application.</br>
	You can get it here https://yadi.sk/d/fqNfFFCnKqz7UA</br>
	We use TrackingCam to detect traffic signs and determine their color.
	To transfer data from the TrackingCam to Arduino Uno Board, we use the I2C interface, for this we use TrackingCamI2C library.</br>
  	You can get it here https://yadi.sk/d/SqLnRoai1HeVGg</br>
	About TrackingCam https://docviewer.yandex.ru/view/502104293/?*=H8tzg5x50SJp33D6wKdMkln2EE17InVybCI6InlhLWRpc2stcHVibGljOi8vb0hYbS9FRHdXeklnWmhwSUpsSG0xNWpMT24rMGtVRzdjTkJHcWp4TTBxY3hPUFRYUGJnWWpvenN3V0NlWFVRcnEvSjZicG1SeU9Kb25UM1ZvWG5EYWc9PSIsInRpdGxlIjoi0KPRh9C10LHQvdC%2B0LUg0L%2FQvtGB0L7QsdC40LUgwqvQotC10YXQvdC40YfQtdGB0LrQvtC1INC30YDQtdC90LjQtSDRgNC%2B0LHQvtGC0L7QsiDRgSDQuNGB0L%2FQvtC70YzQt9C%2B0LLQsNC90LjQtdC8IFRyYWNraW5nQ2FtwrtfcHVibGljLnBkZiIsIm5vaWZyYW1lIjpmYWxzZSwidWlkIjoiNTAyMTA0MjkzIiwidHMiOjE2MjQ0NDI1ODExOTcsInl1IjoiNTM5MzE0OTMyMTU5MzA5MzU3OCJ9</p>

![Image of Sonar HC-SR04](https://github.com/Glaurung2000/images/blob/master/MC2018_TrackingCam_LEGO.png)	

<h3>Encoders</h3>
	<p>We use Encoders to calculate the distance the robot has passed, for this we use PinChangeInterrupt library.</br>
  	You can read about this library here https://www.arduino.cc/reference/en/libraries/pinchangeinterrupt/</p>

<h3>People who worked on this project</h3>
	<ul><li>Konstantin Orlik - engineer</li>
	<li>Korobkina Arina - programmer</li></ul>
