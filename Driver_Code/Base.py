from flask import Flask, request, jsonify
import cv2
import numpy as np
import pytesseract
import mysql.connector

pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

app = Flask(__name__)

def connect_db():
    return mysql.connector.connect(
        host="localhost",
        user="root",
        password="921729#Spks",
        database="Authorised_Vehicles"
    )

def extract_license_plate_number(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    plate_text = pytesseract.image_to_string(gray, config='--psm 8')
    plate_text = plate_text.strip()
    return plate_text

def validate_plate(plate_number):
    connection = connect_db()
    cursor = connection.cursor()
    cursor.execute("SELECT vehicle_number FROM vehicleList WHERE vehicle_number = %s", (plate_number,))
    result = cursor.fetchone()
    connection.close()
    return result is not None

@app.route('/upload', methods=['POST'])
def upload_image():
    if 'image/jpeg' in request.content_type:
        nparr = np.frombuffer(request.data, np.uint8)
        img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        plate_number = extract_license_plate_number(img)
        if validate_plate(plate_number):
            return jsonify({"status": "authorized", "plate": plate_number})
        else:
            return jsonify({"status": "unauthorized", "plate": plate_number})
    return "Invalid content type", 400

if __name__ == '__main__':
    app.run(host='192.168.56.1', port=5000)  # Ensure this IP is correctly configured on your server