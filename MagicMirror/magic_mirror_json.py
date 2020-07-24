from datetime import datetime
from flask import Flask, request
from flask_json import FlaskJSON, JsonError, json_response, as_json
from flask import jsonify

app = Flask(__name__)
FlaskJSON(app)

soc_data_file = '/home/pawel/Documents/IoT_home/data/soc_data.csv'

def get_data(soc_data_file):
    with open(soc_data_file) as csv_file:
        lst = csv_file.readlines()[-1]
        row_lst = lst.split(",")
        soc = row_lst[1]
        power = row_lst[5]
        energy_day = row_lst[10]
        energy_all = row_lst[11]
        voltage_b = row_lst[6]
    return soc, power, energy_day, energy_all, voltage_b

@app.route('/get_value')
@as_json
def get_value():
    soc, power, energy_day, energy_all, voltage_b = get_data(soc_data_file)
    return {"soc": soc, "power": power, "energy_day": energy_day, "voltage_b": voltage_b}


if __name__ == '__main__':
    app.run(host= '0.0.0.0')
