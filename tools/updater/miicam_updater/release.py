import os
import sys

import requests

from miicam_updater import download_file


def release_uris(destination):
    release_data = requests.get(url='https://api.github.com/repos/miicam/miicam/releases/latest').json()

    if "assets" not in release_data or not release_data.get('assets'):
        print("*** No assets found for release! Please wait till the latest release build completes.", file=sys.stderr)
        sys.exit(1)

    for asset in release_data.get('assets'):
        if "tgz" in asset.get("name"):
            filename = os.path.join(destination, asset.get('name'))
            uri = asset.get('browser_download_url')
            yield filename, uri


def download_release(destination):
    for filename, uri in release_uris(destination):
        print(f"*** Downloading {filename}")

        download_file(filename=filename, uri=uri)

        if not os.path.isfile(filename):
            print(f"*** Failed to download {filename}")
            sys.exit(1)
