# 28_07_2021_main - By: ArinAdmin - Tue Jul 27 2021

import sensor, image, time, math, pyb
from pyb import I2C, Servo, Pin, Timer, millis

enable_lens_corr = True # turn on for straighter lines...

true = True
TRUE = True
t = True
T = True
false = False
FALSE = False
f = False
F = False


# USB
usb = pyb.USB_VCP()

Debug = usb.isconnected()
red_ang_green_cubes = t
black_wallse = t
detect_turns = t

# Leds setup
r_led = pyb.LED(1)
g_led = pyb.LED(2)
b_led = pyb.LED(3)


# Red LED = 1, Green LED = 2, Blue LED = 3, IR LEDs = 4.
# Leds's states:
#   blue                :)
#   cian(light blue)    :|
#   red/yellow/green    :(

# Pins setup
button = Pin('P0', Pin.IN)
timer = Timer(2, freq=1000)         # freq in Hz = 1 * s^(-1) => freq = 1000 ~ 1 per ms
IN3 = timer.channel(3, Timer.PWM, pin=Pin('P4'))
IN4 = Pin('P5', Pin.OUT_PP)#timer.channel(4, Timer.PWM, pin=Pin('P5'))
myservo = Servo(2)


# I2C setup
Arduino=I2C(4,I2C.MASTER, baudrate=100000)   # I2C(2) is : (SCL, SDA) = (P4, P5) = (PB10, PB11)

# Sensor setup
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.set_auto_gain(False)
sensor.set_auto_exposure(False, 20000)
sensor.set_auto_whitebal(False)
sensor.skip_frames(time = 2000)
clock = time.clock()

# Color tuples
red = (255, 0, 0)
green = (0, 255, 0)
blue = (0, 0, 255)
wight = (255, 255, 255)
black =(0, 0, 0)
yellow = (255, 255, 0)
pink = (255, 30, 255)
grey = (100, 100, 100)
dark_pink = (153, 0, 76)
light_grey = (180, 180, 180)
purple = (120, 0, 153)


# Thresholds
thresholds = [#(80, 95, -90, 0, 70, 97),    # Gren
              (40, 95, -90, -20, 50, 97),   # Green cube
              (30, 60,  60, 89, 20, 70)]    # Red cube
              #(48, 58, 75, 85, 62, 73)]    # Red
black_wall = [(0, 5, -2, 2, -2, 2)]         # Black wall
turning_lines = [(95, 98, -22, -17, 67, 90)] # Yellow line

# Some vars from old arduino code
right_angle = 0 # ??????????????????????????????????????????????????????????????????????????????????
straeght_angle = 0 # ???????????????????????????????????????????????????????????????????????????????
left_angle = 0 # ???????????????????????????????????????????????????????????????????????????????????
cube_bypassing = f
negative_value = f
button_state = f
STOP = t
turns = 0
robot_speed = 0 # ??????????????????????????????????????????????????????????????????????????????????
robot_stop = 0 # ???????????????????????????????????????????????????????????????????????????????????
finish_time = 0
last_click_time = 0
stop_time = 0
previous_turning_time = 0
WK = 1.6
CK = 1.35

#map() from Arduino
def map( x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

def drive(angle):
    if STOP:
        myservo.pulse_width(straeght_angle)
        IN3.pulse_width(robot_stop)
        IN4.value(0)
    else:
        myservo.pulse_width(robot_speed)
        IN3.pulse_width(straeght_angle)
        IN4.value(1)



while(True):
    lines = list()
    clock.tick()
    img = sensor.snapshot()
    #img =  img.lens_corr(2.8) # for 2.8mm lens...
    img = img.gaussian(3)
    img = img.gamma_corr(gamma = 0.4, contrast = 18.0, brightness = -0.4)
    nearest_cube = 0
    nearest_cube_area = 0
    cube_error = 0
    if red_ang_green_cubes:
        i = 0
        b_led.off()
        for blob in img.find_blobs(thresholds, pixels_threshold=10, area_threshold=10, roi = (0, 20, 160, 85)):
            #   Not straight line          not lines corner        not to far
            if (blob.elongation() > 0.9)or(blob.density() < 0.4)or(blob.area() < 80)  \
            or((blob.h()/ blob.w()) < 0.6):
            #or(blob.rotation() < 0.7)or(blob.rotation() > 2.75:
                if Debug: img.draw_rectangle(blob.rect(), color = yellow)
                if Debug and t:
                    print("X")
                    print("Линия на " + str(int(blob.elongation()*100)) + "%")
                    print("Заполнение " + str(int(blob.density()*100)) + "%")
                    print("Пиксели " + str(int(blob.area() * blob.density())))
                    print(blob.h()/ blob.w())
                    #print("Rotate " + str((blob.rotation())))
                #b_led.on()
            else:
                if Debug and t:
                    print("V")
                    print("Линия на " + str(int(blob.elongation()*100)) + "%")
                    print("Заполнение " + str(int(blob.density()*100)) + "%")
                    print("Пиксели " + str(int(blob.area() * blob.density())))
                    print(blob.h()/ blob.w())
                    #print("Rotate " + str((blob.rotation())))
                b_led.off()
                if i == 0:
                    nearest_cube = blob
                    nearest_cube_area = nearest_cube.area()# * nearest_cube.density()
                else:
                    new_nearest_cube_area = blob.area()# * blob.density()
                    if new_nearest_cube_area > nearest_cube_area:
                        nearest_cube = blob
                        nearest_cube_area = new_nearest_cube_area
                i+=1
        if nearest_cube !=0:
            if nearest_cube.code() == 1: # Green cube
                cube_error = ((-160 + nearest_cube.x()) * (1 + (nearest_cube.y() + nearest_cube.h())/105) + 40) / 230 * 160#* (1 + blob.area() * blob.density()/2060) / 1396 * 160
            if nearest_cube.code() == 2: # Red cube
                cube_error = ((nearest_cube.x()+ nearest_cube.w()) * (1 + (nearest_cube.y() + nearest_cube.h())/105) - 40) / 230 * 160 #* (1 + blob.area() * blob.density()/2060) / 1396 * 160

    left_blob = 0
    left_black_area = 0
    right_blob = 0
    right_black_area = 0
    walls_error = 0
    if black_wallse:
        i = 0
        for blob in img.find_blobs(black_wall, pixels_threshold=10, area_threshold=10, roi = (0, 30, 80, 77)):
            if i == 0:
                left_blob = blob
                left_black_area = left_blob.area() * left_blob.density()
            else:
                new_black_area = blob.area() * blob.density()
                if new_black_area > left_black_area:
                    left_blob = blob
                    left_black_area = new_black_area
            i+=1

        j = 0
        for blob in img.find_blobs(black_wall, pixels_threshold=10, area_threshold=10, roi = (80, 30, 80, 77)):
            if j == 0:
                right_blob = blob
                right_black_area = right_blob.area() * right_blob.density()
            else:
                new_black_area = blob.area() * blob.density()
                if new_black_area > right_black_area:
                    right_blob = blob
                    right_black_area = new_black_area
            j+=1
        walls_error = int(left_black_area - right_black_area)

    turning = False
    k = 0
    if detect_turns:
        for blob in img.find_blobs(turning_lines, pixels_threshold=10, area_threshold=10, roi = (0, 55, 160, 52)):
            if blob.elongation() > 0.6:
                turning = True
                if Debug: img.draw_rectangle(blob.rect(), color = purple)

            k += 1
    if Debug:
        if left_blob != 0:
            img.draw_rectangle(left_blob.rect(), color = grey)
            img.draw_string(left_blob.x(), left_blob.y(), "left", color = grey, scale = 3, mono_space = False,
                            char_rotation = 0, char_hmirror = False, char_vflip = False,
                            string_rotation = 0, string_hmirror = False, string_vflip = False)
            print("left: " + str(left_black_area))
        if right_blob != 0:
            img.draw_rectangle(right_blob.rect(), color = grey)
            img.draw_string(right_blob.x(), right_blob.y(), "right", color = grey, scale = 3, mono_space = False,
                            char_rotation = 0, char_hmirror = False, char_vflip = False,
                            string_rotation = 0, string_hmirror = False, string_vflip = False)
            print("right: " + str(right_black_area))
        if nearest_cube != 0:
            img.draw_rectangle(nearest_cube.rect(), color = grey)
            color_name = "none"
            if nearest_cube.code() == 1:
                str_color_name = "green"
            if nearest_cube.code() == 2:
                str_color_name = "red"
            img.draw_string(nearest_cube.x(), nearest_cube.y()-7, str_color_name, color = grey, scale = 4, mono_space = False,
                            char_rotation = 0, char_hmirror = False, char_vflip = False,
                            string_rotation = 0, string_hmirror = False, string_vflip = False)
            print("cube error: " + str(cube_error))
            print("FPS %f" % clock.fps())



    # start/stop after clicking the red button
    button_state = button.value()
    if (millis() > finish_time):
        STOP = True
    elif button_state and (millis() > last_click_time):
        STOP = not STOP
        if STOP:
            stop_time = millis()
        else:
            finish_time += millis() - stop_time
            stop_time = 0
        last_click_time = millis() + 1000


    # counting 3 rounds
    if turns == 12: finish_time = millis() + 3000


    if nearest_cube == 0: #drive_error = "w" + str(int(walls_error))
        pass
        sqrt_value = int(math.sqrt(abs(walls_error)))
        if sqrt_value > 65: sqrt_value = 65
        if negative_value:
            sqrt_value = -sqrt_value
            negative_value = False
        if (not cube_bypassing)or(cube_bypassing_time > millis()):
            new_value = map(sqrt_value * WK, -70, 70, left_angle, right_angle)
            cube_bypassing = False



    else: #drive_error = "c" + str(int(cube_error))
        pass

        cube_bypassing = True
        cube_bypassing_time = millis() + 4000

        value = int(cube_error * CK)
        if value > 160: value = 160
        if negative_value:
            value = -value
            negative_value = false

        new_value = map(value, -160, 160, left_angle, right_angle)
        if new_value > left_angle: new_value = left_angle
        if new_value < right_angle: new_value = right_angle

    drive(new_value)

    if turning and (previous_turning_time < millis()):
        previous_turning_time = millis() + 2000
        turns += 1

