# IORO2021 FutureEngineers
<p>Innopolis Open Robotics Olympiad 2021 Future engineers WRO’s Self-driving vehicle challenge.</p>
<p>We created this robot to participate in World Robot Olympiad 2021 Future Engineers WRO’s Self-driving vehicle challenge.
Our robot is a four-wheeled vehicle with differential rotation of the rear wheelset, it also has Camera.</p>

<p>Rulles of World Robot Olympiad 2021 Future Engineers WRO’s Self-driving vehicle challenge can be found here https://wro-association.org/fileadmin/files/challenges/2021-power-bots-energy/WRO-2021-Future-Engineers-01-Game-Rules.pdf</p>

<h3>Photos of the robot from different sides</h3>
<ul><li>Front view</li>
	
![Image of robot1](https://github.com/Glaurung2000/images/blob/master/1.png)

<li>Left view</li>
	
![Image of robot2](https://github.com/Glaurung2000/images/blob/master/2.png)
	
<li>Back view</li>

![Image of robot3](https://github.com/Glaurung2000/images/blob/master/3.png)

<li>Right view</li>
	
![Image of robot4](https://github.com/Glaurung2000/images/blob/master/4.png)

<li>Bottom view</li>

![Image of robot5](https://github.com/Glaurung2000/images/blob/master/5.png)

<li>View from above</li>
	
![Image of robot6](https://github.com/Glaurung2000/images/blob/master/6.png)

</ul>

<h3>Major Components and Parts</h3>
<p>To create this robot, special electronic components were needed, namely: engine, Two-channel motor driver DRV8833, servo motor and camera OpenMV Cam H7 Plus. All components are mounted on a plastic frame that has been 3D printed. All photos of the robot, a schematic diagram of electromechanical components and all other materials you can see in our repository.</p>

<h3>OpenMV Cam H7 Plus</h3>

![Image of OpenMV Cam](https://github.com/Glaurung2000/images/blob/master/pinout-openmv-cam-h7-plus-ov5640.png)

<p>The OpenMV Cam is a small, low power, microcontroller board which allows you to easily implement applications using machine vision in the real-world. You program the OpenMV Cam in high level Python scripts (courtesy of the MicroPython Operating System). This makes it easier to deal with the complex outputs of machine vision algorithms and working with high level data structures.</br>
OpenMV Cam allows us to detect field walls and traffic signs using the find_blobs() function. It finds all blobs (connected pixel regions that pass a threshold test) in the image that is satisfying the given parameters and returns a list of objects which describe each blob. After receiving information about the blobs we get the black area on the right and left sides of the screen, their difference is the deviation from the trajectory of the robot. Then we convert this to the value for the servo motor for turning the angle needed to return to the right trajectory, for this we use function map(), the function proportionally transfers the value(x) from the current range of values (in_min, in_max) to the new range (out_min, out_max) specified by the parameters. Also if function find_blobs() gives us information about green or red blobs then instead of difference between left and right black area we use size and coordinates of the to get value for the servo motor that will help robot get around the red and green traffic signs.</p>


<h3> Two-channel motor driver DRV8833</h3>

![Image of Motor drive](https://github.com/Glaurung2000/images/blob/master/DRV8833.jpg)

<p>The DRV8833 driver allows you to drive a brushed motor or a single stepper motor.</br>
We use it to control the speed and direction of the motor rotation</p>


<h3>People who worked on this project</h3>
	<ul><li>Konstantin Orlik</li>
	<li>Korobkina Arina</li></ul>
