import os
import json
import cv2
import numpy as np
from sklearn.model_selection import train_test_split
from tensorflow.keras import layers, models
import tensorflow as tf
import matplotlib.pyplot as plt

class Tracking:
    def __init__(self, image_dir, json_path, scaler=64):
        self.image_dir = image_dir
        self.json_path = json_path
        self.scaler = scaler
        self.scaler_height = int(scaler * (3/4))
        self.coordinates_data = self.load_json()
        self
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
                        scaled_box = self.scale_bbox(x, y, w, h, (640, 480))
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
            # Dense layers for localization
            layers.Dense(128),
            layers.Dense(4)
        ])

        # filter_size = 3
        # pool_size = 2

        # self.model = models.Sequential([
        #     layers.Conv2D(16, (filter_size, filter_size), activation='relu', input_shape=(self.scaler_height, self.scaler, 3)),
        #     layers.BatchNormalization(),
        #     layers.MaxPooling2D((pool_size, pool_size)),
        #     layers.Conv2D(64, (filter_size, filter_size), activation='relu'),
        #     layers.BatchNormalization(),
        #     layers.MaxPooling2D((pool_size, pool_size)),
        #     layers.Flatten(),
        #     layers.Dense(128, activation='relu'),
        #     layers.Dropout(0.5),
        #     layers.Dense(128, activation='relu'),
        #     layers.Dense(4)
        # ])

        self.model.compile(optimizer='adam',
                           loss='mean_squared_error',
                           metrics=['accuracy'])

    def train_model(self, images_train, images_val, boxes_train, boxes_val):
        self.model.fit(images_train, boxes_train, epochs=epochs, 
                       validation_data=(images_val, boxes_val))
        self.model.save("modelH1.h5")

    def predict_bounding_box(self, img):
        if img is None or img.size == 0:
            raise ValueError("Invalid image provided")
            
        img_resized = cv2.resize(img, (self.scaler, self.scaler_height))
        img_normalized = img_resized / 255.0
        img_expanded = np.expand_dims(img_normalized, axis=0)

        predicted_box = self.model.predict(img_expanded)
        x, y, w, h = predicted_box[0]
        rescaled_predict_box = self.rescale_bbox(x, y, w, h, original_dims=(640, 480))
        print(f"Rescaled predicted box (normalized): {rescaled_predict_box}")
        print(f"Predicted box: {predicted_box[0]}")
        
        return predicted_box[0]

    def calculate_center(self, box):
        """ Calculate the center (x_center, y_center) of a bounding box given as [x, y, w, h]. """
        x, y, w, h = box
        x_center = x + w / 2
        y_center = y + h / 2
        return np.array([x_center, y_center])

    def evaluate_model(self, images_val, boxes_val):
        """ Evaluate the model by predicting the boxes and comparing with true boxes. """
        predicted_boxes = self.model.predict(images_val)
        # predicted_boxes1 = self.predict_bounding_box(images_val)
        centers_pred = np.array([self.calculate_center(box) for box in predicted_boxes])
        centers_true = np.array([self.calculate_center(box) for box in boxes_val])

        print("predicted_boxes: ", predicted_boxes)
        # print("predicted_boxes1: ", predicted_boxes1)

        differences = np.linalg.norm(centers_true - centers_pred, axis=1)
        average_difference = np.mean(differences)
        
        print(f"Average difference in distance between real center and predicted center: {average_difference}")
        return average_difference
    
if __name__ == "__main__":
    json_path = 'data2/val/combined.json'
    scaler = 64
    epochs = 300
    image_dir = 'data2/BA'
    tracking = Tracking(image_dir, json_path, scaler)
    
    # Preprocess data
    images_train, images_val, boxes_train, boxes_val = tracking.preprocess_data()
    
    # Build and train the model
    tracking.build_model()
    tracking.train_model(images_train, images_val, boxes_train, boxes_val)

    # Load a single image to test prediction
    test_image_path = os.path.join(image_dir, 'frame283(BA_4).jpg')  # Replace 'example.jpg' with an actual image file
    test_image = cv2.imread(test_image_path)
    if test_image is not None:
        predicted_box = tracking.predict_bounding_box(test_image)
        print("Predicted bounding box:", predicted_box)

    # Evaluate the model
    # tracking.evaluate_model(images_val, boxes_val)