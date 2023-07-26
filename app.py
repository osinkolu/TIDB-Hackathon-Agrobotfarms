from flask import Flask,request
from datetime import datetime 
import pymysql

"""raw_data =     {
        "device_name":"Alpha_x1",
        "temperature":32.47,
        "humidity":59.26,
        "pressure":2867.0,
        "altitude":1539.0,
        "soil_moisture":2564.0,
        "analog_rain_sensor":1234.0,
        "rain":1
    }"""


# Get current date and time
now = datetime.now()

# Format date as 'YYYY-MM-DD'
formatted_date = now.strftime('%Y-%m-%d')

# Format time as 'HH:MM:SS'
formatted_time = now.strftime('%H:%M:%S')



app = Flask(__name__)
@app.route('/')
def index():
    return("Welcome, please smile more")



@app.route("/save", methods=["GET", "POST"])
def save():
    raw_data = request.get_json(force=True)
    device_name = raw_data["device_name"]
    iot_date = formatted_date
    iot_time = formatted_time
    humidity = raw_data["humidity"]
    temperature = raw_data["temperature"]
    pressure = raw_data["pressure"]
    altitude = raw_data["altitude"]
    soil_moisture = raw_data["soil_moisture"]
    analog_rain_sensor = raw_data["analog_rain_sensor"]
    rain = [True if raw_data["rain"]==0 else False][0]
    connection = pymysql.connect(
            host= "gateway01.eu-central-1.prod.aws.tidbcloud.com",
            port=  4000,
            user=  "j95PwvJZywqTsY4.root",
            password= "TNcrrPs9AZqq2PAr",
            database= "agrobot",
            ssl={
            "ca": "cacert.pem"
            },

            autocommit=True
        )
    with connection:
        with connection.cursor() as cursor:
            cursor.execute("INSERT INTO agrobot.iotdata (device_name, iot_date, iot_time, humidity, temperature, pressure, altitude, soil_moisture, analog_rain_sensor, rain) VALUES {};".format((device_name, iot_date, iot_time, humidity, temperature, pressure, altitude, soil_moisture, analog_rain_sensor, rain)))
    return("200")

if __name__ =="__main__":
    app.run(host='0.0.0.0', port=8080)
