import os
import shutil
from sklearn.model_selection import train_test_split

# Directories voor je ruwe dataset en gesplitste datasets
raw_data_dir = 'dataset/raw'
train_dir = 'dataset/train'
val_dir = 'dataset/val'

# Zorg ervoor dat de directories bestaan
os.makedirs(os.path.join(train_dir, 'submarine'), exist_ok=True)
os.makedirs(os.path.join(train_dir, 'background'), exist_ok=True)
os.makedirs(os.path.join(val_dir, 'submarine'), exist_ok=True)
os.makedirs(os.path.join(val_dir, 'background'), exist_ok=True)

# Lijsten voor afbeeldingen van submarine en background
submarine_images = [f for f in os.listdir(os.path.join(raw_data_dir, 'submarine')) if os.path.isfile(os.path.join(raw_data_dir, 'submarine', f))]
background_images = [f for f in os.listdir(os.path.join(raw_data_dir, 'background')) if os.path.isfile(os.path.join(raw_data_dir, 'background', f))]

# Controleer of er voldoende afbeeldingen zijn
if len(submarine_images) == 0 or len(background_images) == 0:
    raise ValueError("Geen afbeeldingen gevonden in de ruwe data directories.")

# Splitsen van de data (bijvoorbeeld 80% training en 20% validatie)
sub_train, sub_val = train_test_split(submarine_images, test_size=0.2, random_state=42)
bg_train, bg_val = train_test_split(background_images, test_size=0.2, random_state=42)

# Functie om bestanden te verplaatsen
def move_files(file_list, source_dir, dest_dir):
    for file in file_list:
        shutil.copy(os.path.join(source_dir, file), os.path.join(dest_dir, file))

# Verplaatsen van submarine afbeeldingen
move_files(sub_train, os.path.join(raw_data_dir, 'submarine'), os.path.join(train_dir, 'submarine'))
move_files(sub_val, os.path.join(raw_data_dir, 'submarine'), os.path.join(val_dir, 'submarine'))

# Verplaatsen van background afbeeldingen
move_files(bg_train, os.path.join(raw_data_dir, 'background'), os.path.join(train_dir, 'background'))
move_files(bg_val, os.path.join(raw_data_dir, 'background'), os.path.join(val_dir, 'background'))

print("Data succesvol gesplitst en verplaatst.")
