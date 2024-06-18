import os

# Directories voor je ruwe dataset
raw_data_dir = 'dataset/raw'
submarine_dir = os.path.join(raw_data_dir, 'submarine')
background_dir = os.path.join(raw_data_dir, 'background')

# Lijsten voor afbeeldingen van submarine en background
submarine_images = [f for f in os.listdir(submarine_dir) if os.path.isfile(os.path.join(submarine_dir, f))]
background_images = [f for f in os.listdir(background_dir) if os.path.isfile(os.path.join(background_dir, f))]

print(f"Aantal submarine afbeeldingen: {len(submarine_images)}")
print(f"Aantal background afbeeldingen: {len(background_images)}")

# Controleer of de lijsten niet leeg zijn
if not submarine_images:
    print("Geen afbeeldingen gevonden in 'submarine' directory.")
if not background_images:
    print("Geen afbeeldingen gevonden in 'background' directory.")
    
