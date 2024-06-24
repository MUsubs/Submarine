import os
import json
import cv2
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from tensorflow.keras import layers, models
import random

class Tracking:
    def __init__(self, image_dir, json_path, scaler=256):
        self.image_dir = image_dir
        self.json_path = json_path
        self.scaler = scaler
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
                else:
                    print(f"No coordinates found for frame {frame_name}")
        return np.array(images), np.array(boxes)

    def scale_bbox(self, x, y, w, h):
        scale_x = self.scaler / 64
        scale_y = self.scaler / 64
        new_x = x * scale_x
        new_y = y * scale_y
        new_width = w * scale_x 
        new_height = h * scale_y
        return [new_x, new_y, new_width, new_height]

    def draw_bounding_box(self, img, box, color):
        if img is None or img.size == 0:
            raise ValueError("Invalid image provided")

        # Rescale coordinates back to original image size
        x, y, w, h = box
        x = int(x)
        y = int(y)
        w = int(w)
        h = int(h)

        # Draw bounding box directly on the image
        cv2.rectangle(img, (x, y), (x + w, y + h), color, 2)

        return img

    def preprocess_data(self):
        images, boxes = self.load_data()
        images = images / 255.0
        return train_test_split(images, boxes, test_size=0.2, random_state=42)

    def build_model(self):
        self.model = models.Sequential([
            layers.Conv2D(32, (3, 3), activation='relu', input_shape=(self.scaler, self.scaler, 3)),
            layers.MaxPooling2D((2, 2)),
            
            layers.Conv2D(64, (3, 3), activation='relu'),
            layers.MaxPooling2D((2, 2)),
            layers.Dropout(0.25),

            layers.Conv2D(128, (3, 3), activation='relu'),
            layers.MaxPooling2D((2, 2)),
            layers.Dropout(0.25),

            layers.Conv2D(256, (3, 3), activation='relu'),
            layers.MaxPooling2D((2, 2)),
            layers.Dropout(0.25),

            # Flatten the output for Dense layers
            layers.Flatten(),

            # Dense layers for localization
            layers.Dense(512, activation='relu'),
            layers.Dropout(0.5),
            
            layers.Dense(256, activation='relu'),
            layers.Dropout(0.5),

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
        return predicted_box[0]

    def get_actual_bounding_box(self, filename):
        frame_name = os.path.basename(filename).replace('.jpg', '.json')
        if frame_name in self.coordinates_data:
            box = self.coordinates_data[frame_name]
            x = float(box['x'])
            y = float(box['y'])
            w = float(box['w'])
            h = float(box['h'])
            return self.scale_bbox(x, y, w, h)
        else:
            return [0.0, 0.0, 0.0, 0.0]

    def print_frame_coordinates_from_json(self, filename):
        frame_name = os.path.basename(filename).replace('.jpg', '.json')
        if frame_name in self.coordinates_data:
            box = self.coordinates_data[frame_name]
            x = float(box['x'])
            y = float(box['y'])
            w = float(box['w'])
            h = float(box['h'])
            return [x, y, w, h]
        else:
            return [0.0, 0.0, 0.0, 0.0]

if __name__ == "__main__":
    image_dir = "data/traindata"
    json_path = "data/validatiedata/combined.json"
    scaler = 64
    
    tracking = Tracking(image_dir, json_path, scaler)
    images_train, images_val, boxes_train, boxes_val = tracking.preprocess_data()
    tracking.build_model()
    tracking.train_model(images_train, images_val, boxes_train, boxes_val)
    
    # Choose 8 random image paths
    random_image_paths = random.sample(os.listdir(image_dir), 8)
    random_image_paths = [os.path.join(image_dir, img_path) for img_path in random_image_paths if img_path.endswith('.jpg')]
    
    for img_path in random_image_paths:
        example_img = cv2.imread(img_path)
        if example_img is not None:
            actual_box = tracking.print_frame_coordinates_from_json(img_path)
            predicted_box = tracking.predict_bounding_box(example_img)
            
            # Draw actual bounding box (green)
            img_with_actual_box = tracking.draw_bounding_box(example_img.copy(), actual_box, (0, 255, 0))
            
            # Draw predicted bounding box (blue)
            img_with_predicted_box = tracking.draw_bounding_box(img_with_actual_box, predicted_box, (0, 0, 255))
            print("Predicted box: ", predicted_box)
            print("Actual box: ", actual_box)
            # Display the image with both boxes
            plt.imshow(cv2.cvtColor(img_with_predicted_box, cv2.COLOR_BGR2RGB))
            plt.axis('off')
            plt.show()
        else:
            print(f"Could not read example image at {img_path}")
