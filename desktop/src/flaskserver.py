from flask import Flask, request, jsonify, render_template, redirect, url_for
import serial
import time
import sys
import os
import json
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'include'))
from SerialControl import SerialControl
from db import get_db, close_db, init_db, init_db_command, init_app
COM_PORT = 8
# PLACEHOLDER FUNCTION
def read_current_location():
    return 0.1, 0.2, 0.3

def create_app(clear_database=True):
    app = Flask(__name__)
    database_path = os.path.normpath(os.path.join(os.path.dirname(os.path.dirname(app.instance_path)), r'include\flaskr.sqlite'))
    print(database_path)
    app.config.from_mapping(
        SECRET_KEY='dev',
        DATABASE=database_path,
    )
    server = Server(app, clear_database)
    return app

class Server:
    def __init__(self, app, clear_database=False):
        self.ser = serial.Serial()
        self.app = app
        self.configure_routes()
        self.server_serial = SerialControl()
        if clear_database:
            init_app(self.app, True)
        # Set these values to None as soon as real data should be used instead of testing data
        self.current_target_x = 0.1
        self.current_target_y = 0.2
        self.current_target_z = 0.3


    def configure_routes(self):
        @self.app.route('/')
        def index():
            return render_template('index.html')

        @self.app.route('/coordinates', methods=['GET', 'POST'])
        def coordinates():
            if request.method == 'POST':
                try:
                    num_values = int(request.form['num_values'])
                    return redirect(url_for('input_coordinates', num_values=num_values))
                except ValueError:
                    return jsonify(success=False, message="Invalid input for number of values"), 400
            return render_template('coordinates.html')

        @self.app.route('/input_coordinates/<int:num_values>', methods=['GET', 'POST'])
        def input_coordinates(num_values):
            if request.method == 'POST':
                db = get_db()
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
                    try:
                         db.execute(
                    "INSERT INTO target_destinations (x, y, z) VALUES (?, ?, ?)",
                    (coord[0], coord[1], coord[2]),
                )   
                         db.commit()  
                         print(f"Values X={coord[0]},Y={coord[1]},Z={coord[2]} inserted into database")
                    except db.IntegrityError:
                        error = f"Coordinates {coord[0], coord[1], coord[2]} error"
                    x, y, z = read_current_location()
                    # Comment this back in to set the target coordinates to the given coordinates
                    # self.current_target_x = coord[0]
                    # self.current_target_y = coord[1]
                    # self.current_target_z = coord[2]
                    self.server_serial.send_serial(f"UPDATE,CURR,X={x},Y={y},Z={z}", COM_PORT)
                    # Remove this sleep when parsing with \n is fixed to improve speed
                    time.sleep(1)
                    server_response, serial_return = self.server_serial.send_serial(f'INST,NEW_POS,X={coord[0]},Y={coord[1]},Z={coord[2]}', COM_PORT, True)
                    response_command, _, _ = self.server_serial.parse_response(server_response)
                    print(f"Response command: {response_command}")

                return redirect(url_for('send_current_location'))
            
            return render_template('input_coordinates.html', num_values=num_values)

        @self.app.route('/data')
        def data():
            sensor_data_path = f'{os.getcwd()}\desktop\include\sensordata.json'
            if not os.path.exists(sensor_data_path):
                print(f"File not found: {sensor_data_path}, Current CWD: {os.getcwd()}")
                return jsonify(success=False, message="Sensor data file not found"), 404
            
            try:
                with open(sensor_data_path, 'r') as f:
                    sensor_data = json.load(f)
            except FileNotFoundError:
                return jsonify(success=False, message="Sensor data file not found"), 404
            return render_template('data.html', sensor_data=sensor_data)
        
        @self.app.route('/send_current_location')
        def send_current_location():
            print("Sending current location")
            x, y, z = read_current_location()
            self.server_serial.send_serial(f"UPDATE,CURR,X{x},Y{y},Z{z}", COM_PORT)
            serial_response = 0
            if (self.current_target_x == x and self.current_target_y == y and self.current_target_z == z):
                # Remove this sleep when parsing with \n is fixed to improve speed
                time.sleep(1)
                serial_response = self.server_serial.send_serial(f"INST,ARRIVED", COM_PORT, True)    
            print(f"Serial Response before if statement {serial_response}")
            if serial_response[0].startswith("INST,ACK,SENS,TEMP"):
                db = get_db()
                #serial_response = self.server_serial.read_serial(COM_PORT)
                print(f"Serial Response after ACK: {serial_response}")
                response_command, temperature_value, error_code = self.server_serial.parse_response(serial_response)
                if temperature_value != None:
                    try:
                        print(f"Writing temperature value {temperature_value} to database.")
                        db.execute(
                    "INSERT INTO temperature (temperature_value) VALUES (?)",
                    (temperature_value,),
                    )   
                        db.commit()  
                        print("Values inserted into database")
                    except db.IntegrityError:
                            error = f"Temperature {temperature_value} error"
                else:
                    print(f"Serial response not a temperature value. Response command:{response_command}, Temperature value:{temperature_value}, Error code:{error_code}")
            return render_template('send_current_location.html', x=x, y=y, z=z)

        @self.app.route('/temperatures')
        def temperatures():
            db = get_db()
            temperatures = db.execute(
                'SELECT temperature_value FROM temperature'
            ).fetchall()
            return render_template('temperatures.html', temperatures=temperatures)

    
    def run(self, debug=True, use_reloader=False):
        self.app.run(debug=debug, use_reloader=use_reloader)

