import cv2

def update_cursor_position(event, x, y, flags, param):
    """
    Update the cursor position on mouse events and print it to the terminal.
    """
    if event == cv2.EVENT_MOUSEMOVE:
        print(f'Cursor Position: ({x}, {y})')

def main():
    # Initialize video capture from the camera
    # Adjust the index if necessary. 0 usually refers to the default camera.
    cap = cv2.VideoCapture(0)

    if not cap.isOpened():
        print("Error: Could not access the camera.")
        return

    cv2.namedWindow('Camera Feed')
    cv2.setMouseCallback('Camera Feed', update_cursor_position)

    while True:
        # Capture frame-by-frame
        ret, frame = cap.read()

        if not ret:
            print("Error: Could not read frame from the camera.")
            break

        # Display the frame in the window just to capture mouse events
        cv2.imshow('Camera Feed', frame)

        # Break loop on pressing 'q'
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release the camera and close all OpenCV windows
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
