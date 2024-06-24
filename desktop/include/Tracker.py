import os
import json
import numpy as np
import cv2
import tensorflow as tf
from tensorflow.keras.applications import VGG16
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Flatten, Dense, Dropout
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.callbacks import ReduceLROnPlateau
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt

class Tracking:
    def __init__(self, image_dir, json_path):
        """
        Initialize the Tracking class with paths to the image directory and JSON file.
        """
        self.image_dir = image_dir
        self.json_path = json_path
        self.coordinates_data = self.load_json()
        self.model = None

    def load_json(self):
        """
        Load JSON data containing bounding box annotations.
        """
        with open(self.json_path) as f:
            return json.load(f)

    def load_data(self):
        """
        Load images and their corresponding bounding boxes from the directory and JSON data.
        boxes = []
        """
        images = []
        for filename in os.listdir(self.image_dir):
            if filename.endswith('.jpg'):
                img_path = os.path.join(self.image_dir, filename)
                img = cv2.imread(img_path)
                if img is None:
                    print(f"Warning: Could not read image {img_path}")
                    continue
                img = cv2.resize(img, (224, 224))
                images.append(img)

                frame_name = filename.replace('.jpg', '.json')
                if frame_name in self.coordinates_data:
                    box = self.coordinates_data[frame_name]
                    try:
                        x = float(box['x'])
                        y = float(box['y'])
                        w = float(box['w'])
                        h = float(box['h'])
                        boxes.append([x, y, w, h])
                    except KeyError:
                        boxes.append([0.0, 0.0, 0.0, 0.0])
        return np.array(images), np.array(boxes)

    def preprocess_data(self):
        """
        Preprocess the loaded data by normalizing images and splitting them into training and validation sets.
        """
        images, boxes = self.load_data()
        images = images / 255.0
        return train_test_split(images, boxes, test_size=0.2, random_state=42)

    def build_model(self):
        """
        Build the model using the VGG16 base model and custom dense layers.
        """
        base_model = VGG16(weights='imagenet', include_top=False, input_shape=(224, 224, 3))
        x = Flatten()(base_model.output)
        x = Dense(1024, activation='relu')(x)
        x = Dropout(0.5)(x)
        x = Dense(512, activation='relu')(x)
        x = Dropout(0.5)(x)
        output = Dense(4)(x)

        model = Model(inputs=base_model.input, outputs=output)
        for layer in base_model.layers:
            layer.trainable = False

        model.compile(optimizer=Adam(learning_rate=0.0001), loss='mean_squared_error')
        self.model = model

    def train_model(self, images_train, images_val, boxes_train, boxes_val):
        """
        Train the model using data augmentation and a reduce learning rate callback.
        """
        datagen = ImageDataGenerator(
            rotation_range=20,
            width_shift_range=0.2,
            height_shift_range=0.2,
            shear_range=0.2,
            zoom_range=0.2,
            horizontal_flip=True,
            fill_mode='nearest'
        )

        train_generator = datagen.flow(images_train, boxes_train, batch_size=32)
        val_generator = datagen.flow(images_val, boxes_val, batch_size=32)
        reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.2, patience=5, min_lr=0.00001)

        self.model.fit(train_generator, epochs=10, validation_data=val_generator, callbacks=[reduce_lr])
        self.model.save('submarine_detection_model_vgg16.keras')

    @staticmethod
    def calculate_iou(box1, box2):
        """
        Calculate Intersection over Union (IoU) between two bounding boxes.
        """
        x1_min, y1_min, w1, h1 = box1
        x1_max = x1_min + w1
        y1_max = y1_min + h1

        x2_min, y2_min, w2, h2 = box2
        x2_max = x2_min + w2
        y2_max = y2_min + h2

        inter_min_x = max(x1_min, x2_min)
        inter_min_y = max(y1_min, y2_min)
        inter_max_x = min(x1_max, x2_max)
        inter_max_y = min(y1_max, y2_max)

        inter_area = max(0, inter_max_x - inter_min_x) * max(0, inter_max_y - inter_min_y)
        box1_area = w1 * h1
        box2_area = w2 * h2
        union_area = box1_area + box2_area - inter_area

        return inter_area / union_area

    def evaluate_model(self, images_val, boxes_val):
        """
        Evaluate the model on the validation set and calculate the mean IoU.
        """
        ious = []
        for i in range(len(images_val)):
            img = images_val[i]
            true_box = boxes_val[i]

            img_input = np.expand_dims(img, axis=0)
            pred_box = self.model.predict(img_input)[0]

            iou = self.calculate_iou(true_box, pred_box)
            ious.append(iou)

        return np.mean(ious)

    def predict_bounding_box(self, image_path):
        """
        Predict the bounding box for a given image.
        """
        img = cv2.imread(image_path)
        if img is None:
            raise FileNotFoundError(f"Could not read image {image_path}")
        img_resized = cv2.resize(img, (224, 224))
        img_normalized = img_resized / 255.0
        img_input = np.expand_dims(img_normalized, axis=0)

        return self.model.predict(img_input)[0]

    def visualize_prediction(self, image_path, predicted_box):
        """
        Visualize the predicted bounding box on the image.
        """
        img = cv2.imread(image_path)
        if img is None:
            raise FileNotFoundError(f"Could not read image {image_path}")

        x, y, w, h = predicted_box
        x, y, w, h = int(x), int(y), int(w), int(h)
        cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

        plt.imshow(img_rgb)
        plt.title("Predicted Bounding Box")
        plt.axis("off")
        plt.show()

# Example usage
if __name__ == "__main__":
    image_dir = 'dataset'
    json_path = 'combined.json'
    tracking = Tracking(image_dir, json_path)
    images_train, images_val, boxes_train, boxes_val = tracking.preprocess_data()
    tracking.build_model()
    tracking.train_model(images_train, images_val, boxes_train, boxes_val)
    mean_iou = tracking.evaluate_model(images_val, boxes_val)
    print(f"Mean IoU on validation set: {mean_iou:.4f}")

    image_path = 'frame544.jpg'
    if not os.path.isfile(image_path):
        print(f"Error: Test image {image_path} does not exist.")
    else:
        try:
            predicted_box = tracking.predict_bounding_box(image_path)
            print(f"Predicted bounding box: {predicted_box}")
            tracking.visualize_prediction(image_path, predicted_box)
        except FileNotFoundError as e:
            print(e)
