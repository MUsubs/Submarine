import os
import json
import numpy as np
import cv2
import tensorflow as tf
from tensorflow.keras.applications import VGG16
from tensorflow.keras.models import Model, Sequential
from tensorflow.keras.layers import Input, Dense, Flatten, Dropout, Conv2D, MaxPooling2D
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.callbacks import ReduceLROnPlateau
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt

class Tracking():
    def __init__(self, image_dir, json_path):
        """
        Initialiseer een nieuwe rechthoek met breedte en hoogte.
        """
        self.json_path = json_path
        self.image_dir = image_dir
        self.coordinates_data = self.load_json_data(json_path)
        self.images, self.boxes = self.load_data_with_edges(image_dir, self.coordinates_data)
        self.images = self.images / 255.0
        self.images_train, self.images_val, self.boxes_train, self.boxes_val = train_test_split(self.images, self.boxes, test_size=0.2, random_state=42)  
        self.model = self.build_model()

    def load_json_data(self, json_path):
        with open(json_path) as f:
            return json.load(f)

    def load_data_with_edges(self, image_dir, coordinates_data):
        images = []
        boxes = []
        for filename in os.listdir(image_dir):
            if filename.endswith('.jpg'):
                # Afbeelding laden
                img_path = os.path.join(image_dir, filename)
                img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)  # Grayscale afbeelding laden
                if img is None:
                    print(f"Warning: Could not read image {img_path}")
                    continue
                # Edge detection toepassen
                edges = cv2.Canny(img, threshold1=100, threshold2=200)
                edges = cv2.resize(edges, (224, 224))  # Formaat aanpassen voor modelinvoer
                
                # Convert to 3-channel image
                edges_3ch = cv2.cvtColor(edges, cv2.COLOR_GRAY2RGB)
                
                images.append(edges_3ch)
                
                # Bijbehorende bounding box extraheren
                frame_name = filename.replace('.jpg', '.json')
                if frame_name in coordinates_data:
                    box = coordinates_data[frame_name]
                    if 'x' in box and 'y' in box and 'w' in box and 'h' in box:
                        boxes.append([int(box['x']), int(box['y']), int(box['w']), int(box['h'])])
                    else:
                        # Voeg een dummy box toe of handel dit anders af
                        boxes.append([0, 0, 0, 0])

        return np.array(images), np.array(boxes)

    def data_augmentatie(self):
        datagen = ImageDataGenerator(
            rotation_range=20,
            width_shift_range=0.2,
            height_shift_range=0.2,
            shear_range=0.2,
            zoom_range=0.2,
            horizontal_flip=True,
            fill_mode='nearest'
        )
        train_generator = datagen.flow(self.images_train, self.boxes_train, batch_size=32)
        val_generator = datagen.flow(self.images_val, self.boxes_val, batch_size=32)
        return train_generator, val_generator

    def build_model(self):
        # Laad VGG16 model zonder de top layers
        base_model = VGG16(weights='imagenet', include_top=False, input_shape=(224, 224, 3))  # Drie kanalen voor RGB afbeeldingen
        # Voeg aangepaste lagen toe
        x = Flatten()(base_model.output)
        x = Dense(1024, activation='relu')(x)
        x = Dropout(0.5)(x)
        x = Dense(512, activation='relu')(x)
        x = Dropout(0.5)(x)
        output = Dense(4)(x)  # Vier uitgangen voor de bounding box coördinaten

        # Bouw het volledige model
        model = Model(inputs=base_model.input, outputs=output)

        # Bevries de lagen van het basis model
        for layer in base_model.layers:
            layer.trainable = False

        # Compileer het model
        model.compile(optimizer=Adam(learning_rate=0.0001), loss='mean_squared_error')

        return model

    def train_model(self):
        # Callback voor aanpassing van de leersnelheid
        reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.2, patience=5, min_lr=0.00001)

        # Genereer geaugmenteerde data
        self.train_generator, self.val_generator = self.data_augmentatie()

        # Train het model met de geaugmenteerde data
        self.model.fit(self.train_generator, epochs=10, validation_data=self.val_generator, callbacks=[reduce_lr])

        # Sla het model op in het aanbevolen formaat
        self.model.save('submarine_detection_model_vgg16_edges.keras')


    def evaluate_model(self):
        ious = []
        for i in range(len(self.images_val)):
            img = self.images_val[i]
            true_box = self.boxes_val[i]
            
            # Voorspel de bounding box
            img_input = np.expand_dims(img, axis=0)
            pred_box = self.model.predict(img_input)[0]
            
            # Bereken IoU
            iou = self.calculate_iou(true_box, pred_box)
            ious.append(iou)
        
        mean_iou = np.mean(ious)
        return mean_iou

    def predict_bounding_box(self, image_path):
        img = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
        if img is None:
            raise FileNotFoundError(f"Could not read image {image_path}")
        edges = cv2.Canny(img, threshold1=100, threshold2=200)
        edges_resized = cv2.resize(edges, (224, 224))
        
        # Convert to 3-channel image
        edges_3ch = cv2.cvtColor(edges_resized, cv2.COLOR_GRAY2RGB)
        
        edges_normalized = edges_3ch / 255.0
        edges_input = np.expand_dims(edges_normalized, axis=0)
        
        pred_box = self.model.predict(edges_input)[0]
        return pred_box

    def visualize_prediction(self, image_path):
        img = cv2.imread(image_path)
        if img is None:
            raise FileNotFoundError(f"Could not read image {image_path}")
        
        # Haal de coördinaten en afmetingen uit de voorspelde box
        x, y, w, h = self.predicted_box
        
        # Converteer naar gehele waarden voor OpenCV
        x, y, w, h = int(x), int(y), int(w), int(h)
        
        # Teken de voorspelde bounding box op de afbeelding
        cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
        
        # Converteer BGR-afbeelding naar RGB voor weergave met matplotlib
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        
        # Toon de afbeelding
        plt.imshow(img_rgb)
        plt.title("Predicted Bounding Box")
        plt.axis("off")
        plt.show()

    def calculate_iou(self, box1, box2):
        # Haal de coördinaten uit de boxen
        x1_min, y1_min, w1, h1 = box1
        x1_max = x1_min + w1
        y1_max = y1_min + h1
        
        x2_min, y2_min, w2, h2 = box2
        x2_max = x2_min + w2
        y2_max = y2_min + h2
        
        # Bereken de coördinaten van de intersectie
        inter_min_x = max(x1_min, x2_min)
        inter_min_y = max(y1_min, y2_min)
        inter_max_x = min(x1_max, x2_max)
        inter_max_y = min(y1_max, y2_max)
        
        # Bereken het oppervlak van de intersectie
        inter_area = max(0, inter_max_x - inter_min_x) * max(0, inter_max_y - inter_min_y)
        
        # Bereken het oppervlak van beide bounding boxes
        box1_area = w1 * h1
        box2_area = w2 * h2
        
        # Bereken het oppervlak van de unie
        union_area = box1_area + box2_area - inter_area
        
        # Bereken IoU
        iou = inter_area / union_area
        return iou

# Gebruik van de Tracking-klasse
image_dir = 'R2D2/Autonome-Navigatie/Dataset'  # Verander naar het juiste pad
json_path = 'R2D2/Autonome-Navigatie/Dataset/lijst.json'  # Verander naar het juiste pad

tracking = Tracking(image_dir, json_path)
tracking.train_model()
mean_iou = tracking.evaluate_model()
print(f"Mean IoU on validation set: {mean_iou:.4f}")

image_path = 'Autonome-Navigatie/frame15(zwembad1).jpg'  # Verander naar het juiste pad
predicted_box = tracking.predict_bounding_box(image_path)
print(f"Predicted bounding box: {predicted_box}")
tracking.visualize_prediction(image_path, predicted_box)