from flask import Flask, render_template, request, url_for
import os

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0
printer_photos = os.path.join('static')
full_filename = "/aa/2357/xy.jpg"
app.config['upload_folder'] = printer_photos
@app.route("/")
def hello():
    full_filename = os.path.join(app.config['upload_folder'], 'xy.jpg')
    return render_template("index.html", user_image = full_filename)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5001)
