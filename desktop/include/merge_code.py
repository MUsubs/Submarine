import os
import json
import numpy as np
import cv2
import tensorflow as tf
import scipy
from skimage import transform
from tensorflow.keras.applications import VGG16
from tensorflow.keras.models import Model, load_model
from tensorflow.keras.layers import Flatten, Dense, Dropout
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.callbacks import ReduceLROnPlateau
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
from tensorflow.keras import layers, models

class Tracking:
    def __init__(self, com_port_1, com_port_2, image_dir, json_path, scaler=256):
        """
        Initialize the Tracking class with paths to the image directory and JSON file.
        """
        #get_frames
        self.com_port_1 = com_port_1
        self.com_port_2 = com_port_2

        self.image_dir = image_dir
        self.json_path = json_path
        self.scaler = scaler
        self.coordinates_data = self.load_json()

    def load_json(self):
        with open(self.json_path) as f:
            return json.load(f)

    def get_frames(self):
        cap1 = cv2.VideoCapture(self.com_port_1)
        cap2 = cv2.VideoCapture(self.com_port_2)
        while cap1.isOpened() and cap2.isOpened():
            ret1, frame1 = cap1.read()
            ret2, frame2 = cap2.read()
            frame1 = cv2.resize(frame1, (540, 380), fx = 0, fy = 0, interpolation = cv2.INTER_CUBIC)
            frame2 = cv2.resize(frame2, (540, 380), fx = 0, fy = 0, interpolation = cv2.INTER_CUBIC)
            sets1, sets2 = self.predict_bounding_box(frame1, frame2)

            x,y = self.convert_data_to_x_y(sets1[0],sets1[1],sets1[2],sets1[3], frame1)
            z = self.convert_data_to_z(sets2[0], sets2[1], sets2[2], sets2[3], frame2)
            coords = [x,y,z]
            with open("cords.txt", "w") as bestand:
                bestand.write(f"{coords[0]}, {coords[1]}, {coords[2]} \n")
            # print("X:", self.x, "Y:", y, "Z:", z)
            cap1.release() 
            cap2.release() 
        return frame1, frame2  
    
    # Functie om waarden uit een bestand te lezen
    def lees_waarden(self):
        cords = []

        with open("cords.txt", 'r') as bestand:
            for regel in bestand:
                cords.append([float(waarde.strip()) for waarde in regel.split(',')])
        return cords


    def predict_bounding_box(self, frame1, frame2):
        self.model = load_model("model.h5")
        if frame1 is None or frame1.size == 0:
            raise ValueError("Invalid image 1 provided")
        if frame2 is None or frame2.size == 0:
            raise ValueError("Invalid image 2 provided")
            
        frame1_resized = cv2.resize(frame1, (self.scaler, self.scaler))
        frame1_normalized = frame1_resized / 255.0
        frame1_expanded = np.expand_dims(frame1_normalized, axis=0)

        frame2_resized = cv2.resize(frame2, (self.scaler, self.scaler))
        frame2_normalized = frame2_resized / 255.0
        frame2_expanded = np.expand_dims(frame2_normalized, axis=0)

        predicted_box_1 = self.model.predict(frame1_expanded)
        predicted_box_2 = self.model.predict(frame2_expanded)
        print(f"Predicted box 1 (normalized): {predicted_box_1[0]}")
        print(f"Predicted box 2 (normalized): {predicted_box_2[0]}")
        return predicted_box_1[0], predicted_box_2[0]
    
    def load_model(self):
        #####Save model
        #model.save("model.h5")
        #print("Saved model to disk")
        #####
        self.model = load_model("path to model")


    def convert_data_to_x_y(self,x,y,w,h,frame):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        #Red range
        lower_red = np.array([0, 70, 50])
        upper_red = np.array([10, 255, 255])
        #Yellow range
        lower_yellow = np.array([20, 100, 100])
        upper_yellow = np.array([30, 255, 255])
        #Create masks
        mask_red = cv2.inRange(hsv, lower_red, upper_red)
        mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
        #Calculate red coordinats
        contours_red, _ = cv2.findContours(mask_red, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        if contours_red:
            largest_contour = max(contours_red, key=cv2.contourArea)
            M = cv2.moments(largest_contour)
            if M["m00"] != 0:
                red_x = int(M["m10"] / M["m00"])
                red_y = int(M["m01"] / M["m00"])
            else:
                red_x, red_y = 0,0
        else:
            red_x, red_y = 0,0

        #Calculate yellow coordinats
        contours_yellow, _ = cv2.findContours(mask_yellow, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        if contours_yellow:
            largest_contour = max(contours_yellow, key=cv2.contourArea)
            M = cv2.moments(largest_contour)
            if M["m00"] != 0:
                yellow_x = int(M["m10"] / M["m00"])
                yellow_y = int(M["m01"] / M["m00"])
            else:
                yellow_x, yellow_y = 0,0
        else:
            yellow_x, yellow_y = 0,0     

        bx = int (x + (w/2))
        by = int (y + (h/2))
        bx = (bx - red_x)/(yellow_x-red_x)
        by = (by - red_y)/(yellow_y-red_y)
        rounded_x = round(bx,3)
        rounded_y = round(by,3)
        return rounded_x,rounded_y

    def convert_data_to_z(self,x,y,w,h,frame):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        #Red range
        lower_red = np.array([0, 70, 50])
        upper_red = np.array([10, 255, 255])
        #Yellow range
        lower_yellow = np.array([20, 100, 100])
        upper_yellow = np.array([30, 255, 255])
        #Create masks
        mask_red = cv2.inRange(hsv, lower_red, upper_red)
        mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
        #Calculate red coordinats
        contours_red, _ = cv2.findContours(mask_red, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        if contours_red:
            largest_contour = max(contours_red, key=cv2.contourArea)
            M = cv2.moments(largest_contour)
            if M["m00"] != 0:
                red_x = int(M["m10"] / M["m00"])
                red_y = int(M["m01"] / M["m00"])
            else:
                red_x, red_y = 0,0
        else:
            red_x, red_y = 0,0

        #Calculate yellow coordinats
        contours_yellow, _ = cv2.findContours(mask_yellow, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        if contours_yellow:
            largest_contour = max(contours_yellow, key=cv2.contourArea)
            M = cv2.moments(largest_contour)
            if M["m00"] != 0:
                yellow_x = int(M["m10"] / M["m00"])
                yellow_y = int(M["m01"] / M["m00"])
            else:
                yellow_x, yellow_y = 0,0
        else:
            yellow_x, yellow_y = 0,0     

        bx = int (x + (w/2))
        bx = (bx - red_x)/(yellow_x-red_x)
        rounded_x = round(bx,3)
        return rounded_x

    def read_current_location(self,x1,y1,w1,h1,frame1,x2,y2,w2,h2,frame2):
        x,y = self.convert_data_to_x_y(x1,y1,w1,h1,frame1)
        z = self.convert_data_to_z(x2,y2,w2,h2,frame2)
        return x,y,z
    
    def main():
        com_port_cam_1 = 0
        com_port_cam_2 = 1
        image_dir = "R2D2/Datasets_making/Frames"
        json_path = "R2D2/Datasets_making/combined.json"
        scaler = 256
        object = Tracking(com_port_cam_1, com_port_cam_2, image_dir, json_path)
        object.get_frames()

        # Gebruik de functie
        print(object.lees_waarden())

        # sets1, sets2 = object.predict_bounding_box(frame1, frame2)
        # print(sets1)
        # print(sets2)

        # frame_zijkant = cv2.imread("R2D2\Coordinaat-bepaling\images\Geel_2.jpg")
        # frame_voorkant = cv2.imread("R2D2\Coordinaat-bepaling\images\Geel_3.jpg")
        # x1,y1,w1,h1 = 240,146,112,47
        # x2,y2,w2,h2 = 320,170,90,40
        # print(read_current_location(x1,y1,w1,h1,frame_voorkant,x2,y2,w2,h2,frame_zijkant))


if __name__ == "__main__":
    Tracking.main()   



