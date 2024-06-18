from flask import Flask, request, jsonify, render_template, redirect, url_for
import serial
import time
import sys
import os
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'include'))
from SerialControl import SerialControl

class Server:
    def __init__(self):
        self.ser = serial.Serial()
        self.app = Flask(__name__)
        self.configure_routes()
        self.server_serial = SerialControl()

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
                    self.server_serial.send_serial(f'X{coord[0]} Y{coord[1]} Z{coord[2]}', 8)
                return redirect(url_for('index'))
            
            return render_template('input_coordinates.html', num_values=num_values)

    def run(self, debug=True, use_reloader=False):
        self.app.run(debug=debug, use_reloader=use_reloader)

if __name__ == '__main__':
    app = Server()
    app.run()
