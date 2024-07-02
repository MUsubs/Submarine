import numpy as np
import cv2
import sqlite3

class Tracking:
    def __init__(self, com_port_cam, scaler=64):
        self.com_port_cam = com_port_cam
        self.scaler = scaler
        self.scaler_height = int(scaler * (3 / 4))
        self.model = None
        self.db_path = r"R2D2\Autonome-Navigatie\desktop\include\flaskr.sqlite"
 
    def send_to_db(self, x, z):
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        cursor.execute("""
        INSERT INTO current_locations (x, y, z)
        VALUES (?, ?, ?)
        """, (x, 0, z))
        conn.commit()
        conn.close()

    def get_cursor_position(self):
        cursor_position = [0, 0]

        def update_cursor_position(event, x, y, flags, param):
            """
            Update the cursor position on mouse events.
            """
            if event == cv2.EVENT_MOUSEMOVE:
                cursor_position[0], cursor_position[1] = x, y

        cap = cv2.VideoCapture(0)

        if not cap.isOpened():
            print("Error: Could not access the camera.")
            return None, None

        cv2.namedWindow('Camera Feed')
        cv2.setMouseCallback('Camera Feed', update_cursor_position)

        while True:
            ret, frame = cap.read()
            if not ret:
                print("Error: Could not read frame from the camera.")
                break

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
                    red_x, red_y = 0, 0
            else:
                red_x, red_y = 0, 0

            contours_yellow, _ = cv2.findContours(mask_yellow, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            if contours_yellow:
                largest_contour = max(contours_yellow, key=cv2.contourArea)
                M = cv2.moments(largest_contour)
                if M["m00"] != 0:
                    yellow_x = int(M["m10"] / M["m00"])
                    yellow_y = int(M["m01"] / M["m00"])
                else:
                    yellow_x, yellow_y = 0, 0
            else:
                yellow_x, yellow_y = 0, 0   

            if yellow_x - red_x != 0 and yellow_y - red_y != 0:
                rounded_x = round((cursor_position[0] - red_x) / (yellow_x - red_x), 3)
                rounded_z = round((cursor_position[1] - red_y) / (yellow_y - red_y), 3)
            else:
                rounded_x, rounded_z = 0, 0

            print(rounded_x, rounded_z)

            self.send_to_db(rounded_x, rounded_z)
            
            cv2.imshow('Camera Feed', frame)
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break

        cap.release()
        cv2.destroyAllWindows()
        return cursor_position, frame

    @staticmethod
    def main():
        com_port_cam = 0
        tracker = Tracking(com_port_cam)
        tracker.get_cursor_position()

if __name__ == "__main__":
    Tracking.main()
