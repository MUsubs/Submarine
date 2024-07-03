import os
import json
import cv2
import numpy as np
from sklearn.model_selection import train_test_split
from tensorflow.keras import layers, models
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from tensorflow.keras.models import load_model
import keras


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
    def __init__(self, image_dir, json_path, scaler):
        self.image_dir = image_dir
        self.json_path = json_path
        self.scaler = scaler
        self.scaler_height = int(scaler * (3/4))
        self.coordinates_data = self.load_json()
        self.model = None
        
    def load_json(self):
        with open(self.json_path, 'r') as f:
            return json.load(f)

    def load_data(self):
        boxes = []
        images = []
        for filename in os.listdir(self.image_dir):
            if filename.endswith('.jpg'):
                img_path = os.path.join(self.image_dir, filename)
                img = cv2.imread(img_path)
                if img is None:
                    print(f"Warning: Could not read image {img_path}")
                    continue
                img = cv2.resize(img, (self.scaler, self.scaler_height))
                images.append(img)
                
                frame_name = filename.replace('.jpg', '.json')
                if frame_name in self.coordinates_data:
                    box = self.coordinates_data[frame_name]
                    try:
                        x = float(box['x'])
                        y = float(box['y'])
                        w = float(box['w'])
                        h = float(box['h'])
                        scaled_box = self.scale_bbox(x, y, w, h, (480,640))
                        boxes.append(scaled_box)
                    except KeyError:
                        boxes.append([0.0, 0.0, 0.0, 0.0])
        return np.array(images), np.array(boxes)

    def scale_bbox(self, x, y, w, h, original_dims):
        scale_x = self.scaler / original_dims[0]
        scale_y = self.scaler_height / original_dims[1]
        new_x = x * scale_x
        new_y = y * scale_y
        new_width = w * scale_x
        new_height = h * scale_y
        return [new_x, new_y, new_width, new_height]
    
    def rescale_bbox(self, x, y, w, h, original_dims):
        scale_x = original_dims[0] / self.scaler
        scale_y = original_dims[1] / self.scaler_height
        new_x = x * scale_x
        new_y = y * scale_y
        new_width = w * scale_x
        new_height = h * scale_y
        return [new_x, new_y, new_width, new_height]

    def preprocess_data(self):
        images, boxes = self.load_data()
        images = np.asarray([preprocess_image(i) for i in images])
        images = images / 255.0
        return train_test_split(images, boxes, test_size=0.2, random_state=42)

    def build_model(self):
        self.model = models.Sequential([
            layers.Conv2D(16, (3, 3), activation='relu', input_shape=(self.scaler_height, self.scaler, 3)),
            layers.MaxPooling2D((2, 2)),
            layers.Conv2D(32, (3, 3), activation='relu'),
            layers.MaxPooling2D((2, 2)),
            layers.Dropout(0.5),
            layers.Flatten(),
            layers.Dense(128, 'relu'),
            layers.Dense(4)
        ])

        self.model.compile(optimizer='adam',
                           loss='mean_squared_error',
                           metrics=['accuracy'])

    def train_model(self, images_train, images_val, boxes_train, boxes_val):
        self.model.fit(images_train, boxes_train, epochs=epochs, 
                       validation_data=(images_val, boxes_val))
        self.model.save("modelH5.keras")
        self.model = None
        self.model = load_model('modelH5.keras')

    def predict_bounding_box(self, img):
        if img is None or img.size == 0:
            raise ValueError("Invalid image provided")
            
        img_resized = cv2.resize(img, (self.scaler, self.scaler_height))
        img_resized = preprocess_image(img_resized)
        img_normalized = img_resized / 255.0
        img_expanded = np.expand_dims(img_normalized, axis=0)

        predicted_box = self.model.predict(img_expanded)
        x, y, w, h = predicted_box[0]
        rescaled_predict_box = self.rescale_bbox(x, y, w, h, original_dims=(480,640))
        print(f"Rescaled predicted box (normalized): {rescaled_predict_box}")
        print(f"Unscaled predicted box (normalized): {predicted_box}")
        
        return predicted_box[0]
    
    def visualize_bounding_box(self, img, box):
        fig, ax = plt.subplots(1)
        ax.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
        # Draw rectangle (predicted box)
        rect = patches.Rectangle((box[0], box[1]), box[2], box[3], linewidth=1, edgecolor='r', facecolor='none')
        ax.add_patch(rect)
        plt.show()
    
if __name__ == "__main__":
    json_path = 'data2/val/combined.json'
    scaler = 64
    epochs = 64
    image_dir = 'data2/BA'
    tracking = Tracking(image_dir, json_path, scaler)
    
    # Preprocess data
    images_train, images_val, boxes_train, boxes_val = tracking.preprocess_data()
    
    # Build and train the model
    tracking.build_model()
    tracking.train_model(images_train, images_val, boxes_train, boxes_val)

    # Load a single image to test prediction
    test_image_path = os.path.join(image_dir, 'frame78(BA_2).jpg')  # Modify the filename to an actual image file
    test_image = cv2.imread(test_image_path)
    
    if test_image is not None:
        predicted_box = tracking.predict_bounding_box(test_image)
        
        original_dims = (480,640)  # Change this to match your specific image dimensions if different
        rescaled_predict_box = tracking.rescale_bbox(predicted_box[0], predicted_box[1],
                                                     predicted_box[2], predicted_box[3], original_dims)
        
        # Display the original image with the predicted bounding box overlaid
        tracking.visualize_bounding_box(test_image, rescaled_predict_box)
        print("Predicted bounding box:", predicted_box)
    else:
        print("Failed to load the image for prediction.")