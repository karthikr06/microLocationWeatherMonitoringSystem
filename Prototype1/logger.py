import serial
import gspread
from oauth2client.service_account import ServiceAccountCredentials
import time

SERIAL_PORT = 'COM5'  

BAUD_RATE = 115200

#Sheet name is given here:
SHEET_NAME = 'Micro Location Google Sheets' 

def setup_google_sheets():
    scope = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']
    creds = ServiceAccountCredentials.from_json_keyfile_name('credentials.json', scope)
    client = gspread.authorize(creds)
    sheet = client.open(SHEET_NAME)
    return sheet

def main():
    sheet = setup_google_sheets()
    print(f"Opened sheet: '{SHEET_NAME}'")

    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} bps...")
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        print("Please check the port name and ensure the device is connected.")
        return

    #Waiting to initialise
    time.sleep(2)

    while True: #Loop to keep watching
        try:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()
                
                #If it is a logging statement
                if line.startswith("LOG,"):
                    print(f"Received: {line}")
                    
                    # Split the data
                    parts = line.split(',')
                    
                    if len(parts) == 6:
                        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
                        node_id = parts[1]
                        temp = parts[2]
                        hum = parts[3]
                        aqi = parts[4]
                        rain = parts[5]
                        
                        # Selecting worksheet
                        try:
                            worksheet_name = f"Sheet{node_id}"
                            worksheet = sheet.worksheet(worksheet_name)
                            
                            # Appending the row
                            row_to_add = [timestamp, temp, hum, aqi, rain]
                            worksheet.append_row(row_to_add)
                            print(f"Uploaded to '{worksheet_name}'")
                          
                        except Exception:
                            print(f"An error occurred while uploading")
                    else:
                        print(f"Warning: Received malformed data line: {line}")

        except serial.SerialException:
            print("Serial port disconnected. Please reconnect the device.")
            break
        except KeyboardInterrupt:
            print("\nExiting program.")
            break
        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            time.sleep(5) #wait here

    ser.close()

if __name__ == '__main__':
    main()
