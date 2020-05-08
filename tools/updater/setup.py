#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import datetime
import io
import os
import subprocess
import sys
from shutil import rmtree

from setuptools import setup, Command

NAME = 'MiiCam Updater'
DESCRIPTION = 'A downloader and remote update script for the MiiCam IP Camera.'
URL = 'https://github.com/MiiCam/MiiCamUpdater'
EMAIL = 'miicam@fliphess.com'
AUTHOR = 'Flip Hess'
REQUIRES_PYTHON = '>=3.6'
VERSION = datetime.datetime.now().strftime('%Y%m%d%H%M%S')
REQUIRED = [
    "pytest==4.3.0",
    "requests>=2.21.0",
    "requests-toolbelt==0.9.1",
    "twine==1.13.0",
]

here = os.path.abspath(os.path.dirname(__file__))
try:
    with io.open(os.path.join(here, 'tools/updater/README.md'), encoding='utf-8') as f:
        long_description = '\n' + f.read()
except FileNotFoundError:
    long_description = DESCRIPTION


setup(
    name='miicam_updater',
    version=VERSION,
    description=DESCRIPTION,
    long_description=long_description,
    long_description_content_type='text/markdown',
    author=AUTHOR,
    author_email=EMAIL,
    python_requires=REQUIRES_PYTHON,
    url=URL,
    packages=[
        'miicam_updater'],
    entry_points=dict(console_scripts=[
        'miicam-downloader=miicam_updater.downloader:main',
        'miicam-updater=miicam_updater.updater:main']),
    install_requires=REQUIRED,
    include_package_data=True,
    license='MIT',
    classifiers=[
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: Implementation :: CPython',
        'Programming Language :: Python :: Implementation :: PyPy'
    ],
)
