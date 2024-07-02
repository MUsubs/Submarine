import os
import numpy as np
import cv2
import sqlite3
from tensorflow import keras
from tensorflow.keras.models import load_model

## @class Tracking merge_code.py "desktop/include/merge_code.py"
#  @brief A class for tracking objects using a camera and a trained model.
#  @details This class provides methods to capture frames from a camera, predict bounding boxes for objects, and calculate coordinates.
class Tracking:

    ## @brief Initialize the Tracking class with paths to the image directory and JSON file.
    #  @param com_port_cam The camera port.
    #  @param scaler The scaling factor for resizing images.
    def __init__(self, com_port_cam, scaler=64):
        """
        Initialize the Tracking class with paths to the image directory and JSON file.
        """
        self.com_port_cam = com_port_cam
        self.scaler = scaler
        self.scaler_height = int(scaler * (3 / 4))
        self.model = None

    ## @brief Capture frames from the camera, predict bounding boxes, and calculate coordinates.
    #  @return The last frame captured from the camera.
    def get_frames(self):
        cap = cv2.VideoCapture(self.com_port_cam)

        while cap.isOpened():
            ret1, frame = cap.read()

            frame = cv2.resize(frame, (640, 480), fx=0, fy=0, interpolation=cv2.INTER_CUBIC)
            sets = self.predict_bounding_box(frame)

            x, z = self.convert_data_to_x_z(sets[0], sets[1], sets[2], sets[3], frame)
            coords = [x,z]

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
                    """, (coords[0], 0.0, coords[1]))
                    conn.commit()
                    print("Data successfully inserted into the 'current_locations' table.")
                except sqlite3.Error as e:
                    print(f"An error occurred: {e}")
                finally:
                    if conn:
                        conn.close()

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        cap.release()
        cv2.destroyAllWindows()
        return frame

    def rescale_bbox(self, x, z, w, h, original_dims):
        scale_x = original_dims[0] / self.scaler
        scale_z = original_dims[1] / self.scaler_height
        new_x = x * scale_x
        new_z = z * scale_z
        new_width = w * scale_x
        new_height = h * scale_z
        return [new_x, new_z, new_width, new_height]

    ## @brief Predict bounding boxes for objects in the given frame using a trained model.
    #  @param frame The frame to predict bounding boxes on.
    #  @return The predicted bounding box.
    def predict_bounding_box(self, frame):
        self.model = load_model("R2D2\Autonome-Navigatie\modelH1.h5")
        if frame is None or frame.size == 0:
            raise ValueError("Invalid image provided")
            
        frame_resized = cv2.resize(frame, (self.scaler, self.scaler_height))
        frame_normalized = frame_resized / 255.0
        frame_expanded = np.expand_dims(frame_normalized, axis=0)

        predicted_box = self.model.predict(frame_expanded)
        x, y, w, h = predicted_box[0]
        rescale_predicted_box = self.rescale_bbox(x, y, w, h, (640, 480))
        return rescale_predicted_box

    ## @brief Convert bounding box data to x and z coordinates.
    #  @param mpx The x-coordinate of the bounding box.
    #  @param mpz The z-coordinate of the bounding box.
    #  @param frame The frame containing the bounding box.
    #  @return The converted x and z coordinates.
    def convert_data_to_x_z(self, x, y, w, h, frame):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        lower_red = np.array([0, 70, 50])
        upper_red = np.array([10, 255, 255])
        lower_yellow = np.array([20, 100, 100])
        upper_yellow = np.array([30, 255, 255])
        mask_red = cv2.inRange(hsv, lower_red, upper_red)
        mask_yellow = cv2.inRange(hsv, lower_yellow, upper_yellow)
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

        cx = x + w / 2
        cy = y + h / 2
        bx = (cx - red_x) / (yellow_x - red_x)
        bz = (cy - red_y) / (yellow_y - red_y)
        rounded_x = round(bx, 3)
        rounded_z = round(bz, 3)
        
        return rounded_x, rounded_z

    def main():
        com_port_cam = 0
        object = Tracking(com_port_cam)
        object.get_frames()

if __name__ == "__main__":
    Tracking.main()
