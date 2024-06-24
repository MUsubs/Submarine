import os
import json
import cv2
import numpy as np
import torch
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt
import subprocess

class Tracker:
    def __init__(self, data_dir, validation_file, yolov5_dir, img_size=(640, 640)):
        self.data_dir = data_dir
        self.validation_file = validation_file
        self.img_size = img_size
        self.yolov5_dir = yolov5_dir
        self.device = 'cuda' if torch.cuda.is_available() else 'cpu'
        self.X_train, self.X_val, self.y_train, self.y_val = None, None, None, None

    def load_validation_data(self):
        with open(self.validation_file, 'r') as f:
            validation_data = json.load(f)
        return validation_data

    def load_data(self):
        images = []
        labels = []
        validation_data = self.load_validation_data()
        
        for filename in os.listdir(self.data_dir):
            if filename.endswith('.jpg') or filename.endswith('.png'):
                filepath = os.path.join(self.data_dir, filename)
                image = cv2.imread(filepath, cv2.IMREAD_GRAYSCALE)  # Load image in grayscale
                image = cv2.resize(image, self.img_size)  # Resize for consistency
                images.append(image)
                
                # Get the corresponding bounding box
                frame_key = filename.split('.')[0] + '.json'
                if frame_key in validation_data:
                    bbox = validation_data[frame_key]
                    x = float(bbox['x']) / self.img_size[0]
                    y = float(bbox['y']) / self.img_size[1]
                    w = float(bbox['w']) / self.img_size[0]
                    h = float(bbox['h']) / self.img_size[1]
                    labels.append([0, x, y, w, h])  # YOLO format: [class, x_center, y_center, width, height]
        
        images = np.array(images) / 255.0  # Normalize images
        images = np.expand_dims(images, axis=-1)  # Add an extra dimension for the grayscale channel
        labels = np.array(labels, dtype=float)
        
        self.X_train, self.X_val, self.y_train, self.y_val = train_test_split(images, labels, test_size=0.2, random_state=42)

    def build_model(self):
        # The model will be built and trained using YOLOv5 scripts directly
        pass

    def train_model(self, epochs=20):
        if self.X_train is None or self.y_train is None:
            raise ValueError("Data is not loaded. Call load_data() first.")
        
        # Prepare the dataset in YOLO format
        self.prepare_yolo_format()

        # Train the YOLO model using the yolov5/train.py script
        subprocess.run([
            'python', os.path.join(self.yolov5_dir, 'train.py'),
            '--img', str(self.img_size[0]),
            '--batch', '16',
            '--epochs', str(epochs),
            '--data', 'data.yaml',
            '--weights', 'yolov5s.pt'
        ])

    def evaluate_model(self):
        # Evaluate the model using the yolov5/val.py script
        subprocess.run([
            'python', os.path.join(self.yolov5_dir, 'val.py'),
            '--data', 'data.yaml',
            '--weights', os.path.join(self.yolov5_dir, 'runs/train/exp/weights/best.pt')
        ])

    def predict(self, image_path):
        # Predict using the yolov5/detect.py script
        subprocess.run([
            'python', os.path.join(self.yolov5_dir, 'detect.py'),
            '--source', image_path,
            '--weights', os.path.join(self.yolov5_dir, 'runs/train/exp/weights/best.pt'),
            '--img', str(self.img_size[0])
        ])

    def visualize_predictions(self, num_samples=5):
        if self.X_val is None or self.y_val is None:
            raise ValueError("Validation data is not loaded. Call load_data() first.")
        
        for i in range(num_samples):
            image_path = os.path.join('data/images', f'{i}.png')
            label = self.y_val[i]
            self.predict(image_path)

            image = cv2.imread(image_path)
            plt.imshow(image, cmap='gray')
            plt.title(f'True: {label}')
            plt.show()

    def prepare_yolo_format(self):
        # Convert and save data in YOLO format
        image_dir = 'data/images'
        label_dir = 'data/labels'
        os.makedirs(image_dir, exist_ok=True)
        os.makedirs(label_dir, exist_ok=True)
        
        for i, (image, label) in enumerate(zip(self.X_train, self.y_train)):
            image_path = os.path.join(image_dir, f'{i}.png')
            label_path = os.path.join(label_dir, f'{i}.txt')
            
            # Save image
            image = (image * 255).astype(np.uint8).reshape(self.img_size)
            cv2.imwrite(image_path, image)
            
            # Save label
            with open(label_path, 'w') as f:
                f.write(f"{int(label[0])} {label[1]} {label[2]} {label[3]} {label[4]}")
        
        # Create a data.yaml file for YOLO training
        with open('data.yaml', 'w') as f:
            f.write(f"""
            train: {image_dir}
            val: {label_dir}
            nc: 1
            names: ['submarine']
            """)

# Usage example
data_dir = 'dataset_comb/train'
validation_file = 'dataset_comb/val/validation_data.json'
yolov5_dir = 'yolov5'

tracker = Tracker(data_dir, validation_file, yolov5_dir)
tracker.load_data()
tracker.build_model()
tracker.train_model(epochs=10)
tracker.evaluate_model()
tracker.visualize_predictions(num_samples=5)
