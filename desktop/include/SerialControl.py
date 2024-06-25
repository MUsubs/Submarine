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


    def send_serial(self, string, com_port, send_and_read=False):
        self.ser.port = f"COM{com_port}"
        self.ser.baudrate = 9600
        try:
            self.ser.open()
        except Exception as e:
            print(f"COM{com_port} not available. Try again or use another port.")
            return -1
        
        charList = []
        string = string.upper()
        print(f"Sending message : '{string}'")
        self.ser.write(string.encode('utf-8'))
        print("Message sent!")
        return_code = 0
        if send_and_read:
            # Wait for and read response from serial port
            time.sleep(0.5)
            while self.ser.in_waiting <= 0:
                pass

            if self.ser.in_waiting > 0:
                try:
                    response = self.ser.readline(self.ser.in_waiting)
                    print("Response from serial port (raw bytes):", response)
                    decoded_response = response.decode('utf-8', errors='ignore')
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
                return decoded_response, return_code
        # Close serial port after sending string
        self.ser.close()
        return return_code, return_code

    def read_serial(self, com_port):
        self.ser.port = f"COM{com_port}"
        self.ser.baudrate = 9600
        if(self.ser.isOpen() == False):
            self.ser.open()
        else:
            print(f"COM{com_port} not available. Try again or use another port.")
            return -1
        while self.ser.in_waiting <= 0:
                pass
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
            return decoded_response, return_code
        # Close serial port after sending string
        return return_code, return_code
    
    def parse_response(self, response):
        print(response)
        try:
            if response == "INST,STOP":
                return "INST_STOP", None, 0
            elif response == "INST,ACK\n":
                return "INST_ACK", None, 0
            elif isinstance(response, tuple):
               if response[0].startswith("INST,ACK,SENS,TEMP"):
                    try:
                        temp = float(response[0].split(',')[4])
                        return "TEMP", temp, 0
                    except (IndexError, ValueError) as e:
                        print(f"Error parsing SENS,TEMP: {e}")
                        return "ERROR", None, -1
               elif response[0].startswith("SENS,TEMP"):
                    try:
                        temp = float(response[0].split(',')[2])
                        return "TEMP", temp, 0
                    except (IndexError, ValueError) as e:
                        print(f"Error parsing SENS,TEMP: {e}")
                        return "ERROR", None, -1
            else:
                print(f"Unsupported command: {response}")
                return "ERROR", None, -1
        except (IndexError, ValueError) as e:
            print(f"Error parsing response: {response}. exception: {e}")
# Serial_test = SerialControl()
# while True:
#     Serial_test.send_serial("INST,NEW_POS,X=1.0_,Y=2.0_,Z=3.0_", 8)
#     time.sleep(3)