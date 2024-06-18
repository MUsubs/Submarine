import cv2
import os

# Video bestand pad
video_path = 'Vooraanzicht-VID02.mp4'
output_dir = 'R2D2\dataset'
os.makedirs(output_dir, exist_ok=True)

# Video capture object
cap = cv2.VideoCapture(video_path)

frame_count = 0
success = True

while success:
    success, frame = cap.read()
    if success:
        # Opslaan van elk frame
        frame_filename = os.path.join(output_dir, f'frame_{frame_count:04d}.jpg')
        cv2.imwrite(frame_filename, frame)
        frame_count += 1

cap.release()
print(f"Extracted {frame_count} frames from the video.")
