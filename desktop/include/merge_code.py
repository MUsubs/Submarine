import os
import json
import numpy as np
import cv2
import sqlite3
from tensorflow import keras
from tensorflow.keras.models import load_model


class Tracking:
    def __init__(self, com_port_cam, image_dir, json_path, scaler=128):
        """
        Initialize the Tracking class with paths to the image directory and JSON file.
        """
        self.com_port_cam = com_port_cam
        self.image_dir = image_dir
        self.json_path = json_path
        self.scaler = scaler
        self.scaler_height = int(scaler * (3 / 4))
        self.coordinates_data = self.load_json()
        self.model = None

    def load_json(self):
        with open(self.json_path) as f:
            return json.load(f)

    def get_frames(self):
        cap = cv2.VideoCapture(self.com_port_cam)

        while cap.isOpened():
            ret1, frame = cap.read()

            frame = cv2.resize(frame, (540, 380), fx = 0, fy = 0, interpolation = cv2.INTER_CUBIC)
            sets = self.predict_bounding_box(frame)

            x,z = self.convert_data_to_x_z(sets[0],sets[1], frame)
            coords = [x,z]
            print(x,z)
            
            db_path = r"R2D2\Autonome-Navigatie\desktop\include\flaskr.sqlite"
            
            if not os.path.isfile(db_path):
                print(f"Database file '{db_path}' not found.")
            else:
                try:
                    conn = sqlite3.connect(db_path)
                    cursor = conn.cursor()
                    cursor.execute("""
                    INSERT INTO current_locations (x, y, z)
                    VALUES (?, ?, ?)
                    """, (coords[0], 0, coords[1]))
                    conn.commit()
                    print("Data successfully inserted into the 'current_locations' table.")
                except sqlite3.Error as e:
                    print(f"An error occurred: {e}")
                finally:
                    if conn:
                        conn.close()
        cap.release()
        return frame
    
    def predict_bounding_box(self, frame):

        # self.model = keras.models.load_model("R2D2\Autonome-Navigatie\Model\modelE.keras")
        self.model = load_model("modelR2.h5")
        if frame is None or frame.size == 0:
            raise ValueError("Invalid image provided")
            
        frame_resized = cv2.resize(frame, (self.scaler, self.scaler_height))
        frame_normalized = frame_resized / 255.0
        frame_expanded = np.expand_dims(frame_normalized, axis=0)

        predicted_box = self.model.predict(frame_expanded)
        print(f"Predicted box 1 (normalized): {predicted_box[0]}")
        return predicted_box[0]

    def convert_data_to_x_z(self,mpx,mpz,frame):
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
        # points = [(yellow_x,yellow_y)]
        # colour = [(255,0,0)]
        # self.draw_points_on_image(frame, points, colour)
        print("mpx:", mpx)
        print("mpz:", mpz)
        
        print("Red box:", red_x, red_y)
        print("yellow box", yellow_x, yellow_y)
        bx = (mpx - red_x) / (yellow_x - red_x)
        bz = (mpz - red_y) / (yellow_y - red_y)
        print("bx:", bx)
        print("bz:", bz)

        rounded_x = round(bx,3)
        rounded_z = round(bz,3)
        
        return rounded_x,rounded_z
    
    def draw_points_on_image(self, frame, points, colors):
        for point, color in zip(points, colors):
            cv2.circle(frame, point, 1, color, -1)  # 10 is the radius, -1 fills the circle
        cv2.imshow('Frame with Points', frame)
        cv2.waitKey(0)  # Wait for a key press
        cv2.destroyAllWindows()

    def main():
        com_port_cam = 0
        image_dir = "R2D2/Datasets_making/Frames"
        json_path = "R2D2/Datasets_making/combined.json"
        object = Tracking(com_port_cam, image_dir, json_path)
        object.get_frames()

if __name__ == "__main__":
    Tracking.main()   



