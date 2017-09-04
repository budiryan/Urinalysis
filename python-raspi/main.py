import cv2
import numpy as np
#from lib.image_proc import (convert_array_to_matrices,
#                            interpolate,
#                            naive_distance,
#                            rgb_to_lab,
#                            get_glucose_reference_colors,
#                            get_glucose_amount)
from lib.stepper_motor import stepper_motor
from lib.pump import pump
import RPi.GPIO as GPIO
import time
from picamera.array import PiRGBArray
from picamera import PiCamera
import pigpio


if __name__ == '__main__':		
	# Motor test
    motor = stepper_motor(dir_pin=19, step_pin=26)
    motor.rotate(direction='CW', degrees=360)
    # Pump test
    #piezo_pump = pump(dir_pin=20, step_pin=21)
    #piezo_pump.start_pump(direction='left', duty_cycle=0, freq=0)
    
    #pi = pigpio.pi()
    #pi.write(20, 0)
    #pi.set_mode(20, pigpio.OUTPUT)
    #pi.set_mode(21, pigpio.OUTPUT)
    #pi.set_PWM_dutycycle(21, 128)
    #pi.set_PWM_frequency(21, 1000)
    
    
    '''
    # Generate reference data: glucose level ascending order
    glucose_colors = get_glucose_reference_colors()
    glucose_amount_per_stage = get_glucose_amount()
    
    # Generate dummy testing data, replace this later with camera result
    test_data = np.array([
        (3, 4, 255),
        (40, 201, 134),
        (37, 64, 63)
    ])

    # Assuming size of reference data is: 30x30
    size = 30
    test_data_matrices = convert_array_to_matrices(test_data, size)
    test_data_matrices = rgb_to_lab(test_data_matrices)
    glucose_reference_matrices = rgb_to_lab(
        convert_array_to_matrices(glucose_colors, size))
    
    # cap = cv2.VideoCapture(0)
    camera = PiCamera()
    camera.resolution = (640, 480)
    camera.framerate = 32
    rawCapture = PiRGBArray(camera, size=(640, 480))
    time.sleep(0.1)
   
	
    for image in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
        frame = image.array
        height = frame.shape[0]
        width = frame.shape[1]
        cv2.rectangle(frame, (width / 2, height / 2 - size / 2),
                      (width / 2 + size, height / 2 + size / 2), (255, 0, 0), 2)

        glucose_matrix_display = frame[height / 2 - size /
                                       2: height / 2 + size / 2, width / 2:width / 2 + size]

        glucose_matrix = cv2.cvtColor(
            glucose_matrix_display, cv2.COLOR_BGR2RGB)
        cv2.imshow('Kingsmen', frame)
        cv2.imshow('filtered', glucose_matrix_display)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            # test interpolation
            # glucose_matrix = white_patch(rgb_to_lab(glucose_matrix))
            glucose_matrix = glucose_matrix[np.newaxis, :]
            print naive_distance(glucose_matrix, glucose_reference_matrices, glucose_amount_per_stage)
            break
        rawCapture.truncate(0)
    cv2.destroyAllWindows()
	'''
