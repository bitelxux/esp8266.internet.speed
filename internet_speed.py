#!/usr/bin/python3

import os
from datetime import date, datetime
from flask import Flask, Markup, request, render_template

MAX_UPLOAD = 350
MAX_DOWNLOAD = 60

root_dir = os.getcwd()
app = Flask(__name__)

@app.route('/')
def main():
     return "I'm alive!"

@app.route('/download')
def download():
    with open(os.path.join(root_dir, "speed"), "r") as f:
        speed = f.readline()
        if not speed:
            return "-1";
        #angle = 180 - (int(float(speed))*180)/MAX_SPEED
        angle = 180 - int(float(speed)*180/MAX_UPLOAD)
        return str(int(angle))

@app.route('/upload')
def upload():
    with open(os.path.join(root_dir, "speed"), "r") as f:
        speed = f.readline() #  to discard. Its download
        speed = f.readline()
        if not speed:
            return "-1";
        #angle = 180 - (int(float(speed))*180)/MAX_SPEED
        angle = 180 - int(float(speed)*180/MAX_DOWNLOAD)
        return str(int(angle))

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8889, threaded=True)

