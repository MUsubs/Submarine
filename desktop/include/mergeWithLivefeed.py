import cv2
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from tensorflow.keras.models import load_model
import time


def equalize_histogram(image):
    return cv2.equalizeHist(image)

# For color images
def equalize_histogram_color(image):
    img_yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV)
    img_yuv[:, :, 0] = cv2.equalizeHist(img_yuv[:, :, 0])
    return cv2.cvtColor(img_yuv, cv2.COLOR_YUV2BGR)

def apply_clahe(image):
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
    return clahe.apply(image)

# For color images
def apply_clahe_color(image):
    img_yuv = cv2.cvtColor(image, cv2.COLOR_BGR2YUV)
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
    img_yuv[:, :, 0] = clahe.apply(img_yuv[:, :, 0])
    return cv2.cvtColor(img_yuv, cv2.COLOR_YUV2BGR)

def adjust_gamma(image, gamma=1.0):
    inv_gamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** inv_gamma) * 255 for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)

def denoise_image(image):
    return cv2.fastNlMeansDenoisingColored(image, None, 10, 10, 7, 21)

def preprocess_image(image):


    # Apply CLAHE
    image = apply_clahe_color(image)

    # Apply gamma correction
    image = adjust_gamma(image, gamma=1.2)
    
    image = denoise_image(image)



    return image

class Tracking:
    def __init__(self, model_path, scaler=64, scaler_height=48):
        self.scaler = scaler
        self.scaler_height = scaler_height
        self.model = load_model(model_path)
        
    
    def predict_bounding_box(self, img):
        if img is None or img.size == 0:
            raise ValueError("Invalid image provided")
        
        img_resized = cv2.resize(img, (self.scaler, self.scaler_height))
        img_resized = preprocess_image(img_resized)

        img_normalized = img_resized / 255.0
        img_expanded = np.expand_dims(img_normalized, axis=0)
        predicted_box = self.model.predict(img_expanded)[0]
        
        print("unscaled predicted bbox", predicted_box)

        # Rescale box to original dimensions
        x, y, w, h = self.rescale_bbox(predicted_box[0], predicted_box[1], predicted_box[2], predicted_box[3])

        return [x, y, w, h], img_resized
    
    def rescale_bbox(self, x, y, w, h):
        scale_x = self.original_dims[0] / self.scaler
        scale_y = self.original_dims[1] / self.scaler_height
        new_x = x * scale_x
        new_y = y * scale_y
        new_width = w * scale_x
        new_height = h * scale_y
        print(f'x : {new_x},y:  {new_y}, w: {new_width}, h : {new_height} ')
        return [new_x, new_y, new_width, new_height]
    
    def rescale_bbox2(self, x, y, w, h):
        scale_x = self.scaler / self.original_dims[0]
        scale_y = self.scaler_height / self.original_dims[1]
        new_x = x * scale_x
        new_y = y * scale_y
        new_width = w * scale_x
        new_height = h * scale_y
        return [new_x, new_y, new_width, new_height]


def live_feed(tracking):
    cap = cv2.VideoCapture(0)



    while True:
        time.sleep(0.1)
        ret, frame = cap.read()
        frame = cv2.resize(frame, (640,480))
        cv2.imwrite(f"livefeed.jpg", frame)
        # frame = 'frame78(BA_2).jpg'
        time.sleep(0.1)

        frame = cv2.imread('livefeed.jpg', 3)
        print("frameshape:", frame.shape)

        # frame = cv2.resize(frame, (640,480))
        tracking.original_dims = frame.shape
        if not ret:
            print("Failed to capture video")
            break
        
        box, img = tracking.predict_bounding_box(frame)
        print("volgens leo is box: ", box)
        x, y, w, h = box
        cv2.rectangle(frame, (int(x), int(y)), (int(x + w), int(y + h)), (0, 0, 255), 2)
        # cv2.rectangle(img, (int(x), int(y)), (int(x + w), int(y + h)), (0, 0, 255), 2)

        print(frame.shape)
        # print(img.shape)
        # img = cv2.resize(img, (600,800))
        # cv2.imshow('Tracking', img)
        cv2.imshow('Tracking', frame)


        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    model_path = 'modelH5.keras'
    tracking = Tracking(model_path)
    live_feed(tracking)