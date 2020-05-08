import os
import sys

import requests

from miicam_updater import download_file, write_md5sum


def get_latest_nightly(data):
    highest_version = 0, None

    for item in data:
        version = int(item['version'].replace(".", ""))
        if version > highest_version[0]:
            highest_version = version, item

    return highest_version[1]


def download_nightly(destination):
    nightly_data = requests.get(url='https://api.anaconda.org/package/MiiCam/MiiCam-nightly/files').json()
    latest = get_latest_nightly(nightly_data)

    write_md5sum(latest=latest, destination=destination)

    download_url = f"https:{latest['download_url']}"
    filename = os.path.join(destination, 'MiiCam.tgz')

    print(f"*** Downloading {filename}")
    download_file(filename=filename, uri=download_url)

    if not os.path.isfile(filename):
        print(f"*** Failed to download {filename}")
        sys.exit(1)
