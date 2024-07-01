import cv2
import numpy as np
from tensorflow.keras.models import load_model

class LiveFeedTracker:
    def __init__(self, model_path, scaler=64, scaler_height=48):
        # Load the pre-trained model
        self.model = load_model(model_path)
        self.scaler = scaler
        self.scaler_height = scaler_height

    def predict_bounding_box(self, img):
        # Preprocess the image
        img_resized = cv2.resize(img, (self.scaler, self.scaler_height))
        img_normalized = img_resized / 255.0
        img_expanded = np.expand_dims(img_normalized, axis=0)

        # Predict the bounding box using the loaded model
        predicted_box = self.model.predict(img_expanded)
        normalized_coordinates = predicted_box[0]
        x, y, w, h = normalized_coordinates
        x = x * 10
        y = y * 10
        w = w * 10
        h = h * 10
        print(f"Predicted box (normalized): {normalized_coordinates}")

        # Scale coordinates back to original frame size
        x, y, w, h = normalized_coordinates
        x, w = x * 640, w * 640  # Scaling factor for width
        y, h = y * 480, h * 480  # Scaling factor for height
        real_coordinates = (int(x), int(y), int(w), int(h))
        print(f"Predicted box (real coordinates): {real_coordinates}")
        return real_coordinates

    def run(self):
        # Start capturing video from the default camera
        cap = cv2.VideoCapture(0)

        if not cap.isOpened():
            print("Error: Cannot open camera")
            return

        try:
            while True:
                # Capture frame-by-frame
                ret, frame = cap.read()
                if not ret:
                    print("Can't receive frame (stream end?). Exiting ...")
                    break

                # Predict the bounding box for the current frame
                predicted_box = self.predict_bounding_box(frame)
                
                # Display the resulting frame with bounding box
                x, y, w, h = predicted_box
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                cv2.imshow('Frame', frame)

                if cv2.waitKey(1) == ord('q'):
                    break
        finally:
            # When everything done, release the capture
            cap.release()
            cv2.destroyAllWindows()

if __name__ == "__main__":
    tracker = LiveFeedTracker("modelH1.h5")
    tracker.run()
