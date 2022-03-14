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
        return speed or "-1"

@app.route('/upload')
def upload():
    with open(os.path.join(root_dir, "speed"), "r") as f:
        speed = f.readline() #  to discard. Its download
        speed = f.readline()
        return speed or "-1"

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8889, threaded=True)

