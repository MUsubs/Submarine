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

# Paden naar de data
image_dir = 'Autonome-Navigatie\dataset_zwembad'  # Pad naar de map met jpg-afbeeldingen
json_path = 'Autonome-Navigatie\combined.json'  # Pad naar het JSON-bestand met annotaties

# JSON-gegevens laden
with open(json_path) as f:
    coordinates_data = json.load(f)

# Functie om afbeeldingen met edge detection en bijbehorende bounding boxes te laden
def load_data_with_edges(image_dir, coordinates_data):
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

# Gegevens laden en preprocessen
images, boxes = load_data_with_edges(image_dir, coordinates_data)
images = images / 255.0  # Normaliseer de afbeeldingen

# Gegevens splitsen in trainings- en validatiesets
images_train, images_val, boxes_train, boxes_val = train_test_split(images, boxes, test_size=0.2, random_state=42)

# Data-augmentatie
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

# Callback voor aanpassing van de leersnelheid
reduce_lr = ReduceLROnPlateau(monitor='val_loss', factor=0.2, patience=5, min_lr=0.00001)

# Train het model met de geaugmenteerde data
model.fit(train_generator, epochs=10, validation_data=val_generator, callbacks=[reduce_lr])

# Sla het model op in het aanbevolen formaat
model.save('submarine_detection_model_vgg16_edges.keras')

# Functie om IoU (Intersection over Union) te berekenen
def calculate_iou(box1, box2):
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

# Functie om het model te evalueren op de validatieset
def evaluate_model(model, images_val, boxes_val):
    ious = []
    for i in range(len(images_val)):
        img = images_val[i]
        true_box = boxes_val[i]
        
        # Voorspel de bounding box
        img_input = np.expand_dims(img, axis=0)
        pred_box = model.predict(img_input)[0]
        
        # Bereken IoU
        iou = calculate_iou(true_box, pred_box)
        ious.append(iou)
    
    mean_iou = np.mean(ious)
    return mean_iou

# Bereken de gemiddelde IoU op de validatieset
mean_iou = evaluate_model(model, images_val, boxes_val)
print(f"Mean IoU on validation set: {mean_iou:.4f}")

# Functie om de bounding box op een nieuwe afbeelding te voorspellen
def predict_bounding_box(image_path, model):
    img = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        raise FileNotFoundError(f"Could not read image {image_path}")
    edges = cv2.Canny(img, threshold1=100, threshold2=200)
    edges_resized = cv2.resize(edges, (224, 224))
    
    # Convert to 3-channel image
    edges_3ch = cv2.cvtColor(edges_resized, cv2.COLOR_GRAY2RGB)
    
    edges_normalized = edges_3ch / 255.0
    edges_input = np.expand_dims(edges_normalized, axis=0)
    
    pred_box = model.predict(edges_input)[0]
    return pred_box

# Functie om de voorspelde bounding box te visualiseren
def visualize_prediction(image_path, predicted_box):
    img = cv2.imread(image_path)
    if img is None:
        raise FileNotFoundError(f"Could not read image {image_path}")
    
    # Haal de coördinaten en afmetingen uit de voorspelde box
    x, y, w, h = predicted_box
    
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

# Voorspel en visualiseer de bounding box voor de nieuwe afbeelding
image_path = r'Autonome-Navigatie\frame15(zwembad1).jpg'

if not os.path.isfile(image_path):
    print(f"Error: Test image {image_path} does not exist.")
else:
    try:
        predicted_box = predict_bounding_box(image_path, model)
        print(f"Predicted bounding box: {predicted_box}")
        visualize_prediction(image_path, predicted_box)
    except FileNotFoundError as e:
        print(e)
