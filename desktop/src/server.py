from flask import Flask, request, jsonify, render_template, redirect, url_for
import serial
import time

ser = serial.Serial()
app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/coordinates', methods=['GET', 'POST'])
def coordinates():
    if request.method == 'POST':
        try:
            num_values = int(request.form['num_values'])
            return redirect(url_for('input_coordinates', num_values=num_values))
        except ValueError:
            return jsonify(success=False, message="Invalid input for number of values"), 400
    return render_template('coordinates.html')

@app.route('/input_coordinates/<int:num_values>', methods=['GET', 'POST'])
def input_coordinates(num_values):
    if request.method == 'POST':
        coordinates = []
        for i in range(num_values):
            try:
                x = float(request.form[f'x_{i}'])
                y = float(request.form[f'y_{i}'])
                z = float(request.form[f'z_{i}'])
                coordinates.append((x, y, z))
            except ValueError:
                return jsonify(success=False, message="Invalid input for coordinates"), 400

        for coord in coordinates:
            send_serial(f'X{coord[0]} Y{coord[1]} Z{coord[2]}', 8)
        return redirect(url_for('index'))
    
    return render_template('input_coordinates.html', num_values=num_values)

def send_serial(string, com_port):
    ser.port = f"COM{com_port}"
    ser.baudrate = 9600
    ser.open()
    
    charList = []
    string = string.upper()
    for character in string:
        charList.append(character)
    for item in charList:
        ser.write(item.encode())
        time.sleep(0.1)
    print("Message sent!")

    # Wait for and read response from serial port
    time.sleep(0.5)  # Wait a bit to ensure the response is available
    return_code = 0
    if ser.in_waiting > 0:
        try:
            response = ser.read(ser.in_waiting)
            print("Response from serial port (raw bytes):", response)
            decoded_response = response.decode('utf-8', errors='ignore')  # Decode while ignoring errors
            print("Response from serial port (decoded):", decoded_response)
            return_code = 0
        except Exception as e:
            print(f"Error reading response: {e}")
            return_code = -1
    else:
        print("No response from the serial port.")
        return_code = -1
    
    # Close serial port after sending string
    ser.close()
    return return_code

if __name__ == '__main__':
    app.run(debug=True, use_reloader=False)
