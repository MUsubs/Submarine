import serial
import time
class SerialControl:

    def __init__(self):
        """
        Initializes the MyClass with the given parameters.

        Parameters
        ----------
        param1 : type
            Description of param1
        param2 : type
            Description of param2
        """
        self.ser = serial.Serial()


    def send_serial(self, string, com_port):
        self.ser.port = f"COM{com_port}"
        self.ser.baudrate = 9600
        if(self.ser.isOpen() == False):
            self.ser.open()
        else:
            print(f"COM{com_port} not available. Try again or use another port.")
            return -1
        
        charList = []
        string = string.upper()
        for character in string:
            charList.append(character)
        for item in charList:
            self.ser.write(item.encode())
            time.sleep(0.1)
        print("Message sent!")

        # Wait for and read response from serial port
        time.sleep(0.5)  # Wait a bit to ensure the response is available
        return_code = 0
        if self.ser.in_waiting > 0:
            try:
                response = self.ser.read(self.ser.in_waiting)
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
        self.ser.close()
        return return_code

    def read_serial(self, com_port):
        self.ser.port = f"COM{com_port}"
        self.ser.baudrate = 9600
        if(self.ser.isOpen() == False):
            self.ser.open()
        else:
            print(f"COM{com_port} not available. Try again or use another port.")
            return -1
        if self.ser.in_waiting > 0:
            try:
                response = self.ser.read(self.ser.in_waiting)
                print("Response from serial port (raw bytes):", response)
                decoded_response = response.decode('utf-8')
                print("Response from serial port (decoded):", decoded_response)
                return_code = 0
            except Exception as e:
                print(f"Error reading response: {e}")
                return_code = -1
        else:
            print("No response from the serial port.")
            return_code = -1
        self.ser.close()
        if return_code == 0:
            return decoded_response
        # Close serial port after sending string
        return return_code
# Serial_test = SerialControl()
# while True:
#     Serial_test.send_serial("INST,NEW_POS,X=1.0_,Y=2.0_,Z=3.0_", 8)
#     time.sleep(3)
