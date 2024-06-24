import os
import json
import cv2
import numpy as np
from sklearn.model_selection import train_test_split
from tensorflow.keras import layers, models
import tensorflow as tf
import matplotlib.pyplot as plt

class Tracking:
    def __init__(self, image_dir, json_path, scaler=256):
        self.image_dir = image_dir
        self.json_path = json_path
        self.scaler = scaler
        self.coordinates_data = self.load_json()
        self.model = None

    def load_json(self):
        with open(self.json_path) as f:
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
                img = cv2.resize(img, (self.scaler, self.scaler))
                images.append(img)

                frame_name = filename.replace('.jpg', '.json')
                if frame_name in self.coordinates_data:
                    box = self.coordinates_data[frame_name]
                    try:
                        x = float(box['x'])
                        y = float(box['y'])
                        w = float(box['w'])
                        h = float(box['h'])
                        scaled_box = self.scale_bbox(x, y, w, h)
                        boxes.append(scaled_box)
                    except KeyError:
                        boxes.append([0.0, 0.0, 0.0, 0.0])
        return np.array(images), np.array(boxes)

    def scale_bbox(self, x, y, w, h):
        scale_x = self.scaler / 640
        scale_y = self.scaler / 480
        new_x = x * scale_x
        new_y = y * scale_y
        new_width = w * scale_x
        new_height = h * scale_y
        return [new_x, new_y, new_width, new_height]

    def preprocess_data(self):
        images, boxes = self.load_data()
        images = images / 255.0
        return train_test_split(images, boxes, test_size=0.2, random_state=42)

    def build_model(self):
        num_filters = 32
        filter_size = 3
        pool_size = 2

        self.model = models.Sequential([
            layers.Conv2D(num_filters, (filter_size, filter_size), activation='relu', input_shape=(self.scaler, self.scaler, 3)),
            layers.MaxPooling2D((pool_size, pool_size)),
            layers.Conv2D(num_filters, (filter_size, filter_size), activation='relu'),
            layers.MaxPooling2D((pool_size, pool_size)),
            layers.Conv2D(num_filters, (filter_size, filter_size), activation='relu'),
            layers.Flatten(),
            layers.Dense(128, activation='relu'),
            layers.Dense(4)
        ])

        self.model.compile(optimizer='adam',
                           loss='mean_squared_error',
                           metrics=['accuracy'])

    def train_model(self, images_train, images_val, boxes_train, boxes_val):
        self.model.fit(images_train, boxes_train, epochs=5, 
                       validation_data=(images_val, boxes_val))

    def predict_bounding_box(self, img):
        if img is None or img.size == 0:
            raise ValueError("Invalid image provided")
            
        img_resized = cv2.resize(img, (self.scaler, self.scaler))
        img_normalized = img_resized / 255.0
        img_expanded = np.expand_dims(img_normalized, axis=0)

        predicted_box = self.model.predict(img_expanded)
        print(f"Predicted box (normalized): {predicted_box[0]}")
        return predicted_box[0]
        
    def visualize_bounding_box(self, img, box):
        if img is None or img.size == 0:
            raise ValueError("Invalid image provided")
            
        img_resized = cv2.resize(img, (self.scaler, self.scaler))
        x, y, w, h = box
        x = int(x * img_resized.shape[1])
        y = int(y * img_resized.shape[0])
        w = int(w * img_resized.shape[1])
        h = int(h * img_resized.shape[0])

        img_with_box = cv2.rectangle(img_resized.copy(), (x, y), (x + w, y + h), (255, 0, 0), 2)

        plt.imshow(cv2.cvtColor(img_with_box, cv2.COLOR_BGR2RGB))
        plt.show()

    def print_frame_coordinates_from_json(self, filename):
        frame_name = filename.replace('.jpg', '.json')
        print(f"Coordinates for frame {frame_name}:")
        if frame_name in self.coordinates_data:
            box = self.coordinates_data[frame_name]
            x = box.get('x', 'N/A')
            y = box.get('y', 'N/A')
            w = box.get('w', 'N/A')
            h = box.get('h', 'N/A')
            print(f"x={x}, y={y}, w={w}, h={h}")
        else:
            print(f"No coordinates found for frame {frame_name}")

if __name__ == "__main__":
    image_dir = 'data/traindata'
    json_path = 'data/validatiedata/combined.json'
    scaler = 256

    tracking = Tracking(image_dir, json_path, scaler)
    images_train, images_val, boxes_train, boxes_val = tracking.preprocess_data()
    tracking.build_model()
    tracking.train_model(images_train, images_val, boxes_train, boxes_val)
    
    example_img_path = os.path.join(image_dir, 'frame_61NOZC.jpg')
    example_img = cv2.imread(example_img_path)
    if example_img is not None:
        predicted_box = tracking.predict_bounding_box(example_img)
        tracking.visualize_bounding_box(example_img, predicted_box)
        tracking.print_frame_coordinates_from_json(example_img_path)
    else:
        print(f"Could not read example image at {example_img_path}")
    
