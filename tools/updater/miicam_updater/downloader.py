import argparse
import os

from miicam_updater import verify_md5sum, unpack_tarbal
from miicam_updater.nightly import download_nightly
from miicam_updater.release import download_release


def parse_arguments():
    parser = argparse.ArgumentParser(description="MiiCam Downloader")

    parser.add_argument(
        '-r',
        '--release',
        default=False,
        action="store_true",
        dest="release",
        help="Download the latest release")

    parser.add_argument(
        '-n',
        '--nightly',
        default=False,
        action="store_true",
        dest="nightly",
        help="Download the latest nightly build")

    parser.add_argument(
        '-d',
        '--destination',
        type=str,
        default='/tmp',
        dest='destination',
        help='The destination where to store the tarbal')

    args = parser.parse_args()

    if (not args.nightly and not args.release) or (args.nightly and args.release):
        parser.error('Use either --release or --nightly')

    if not os.path.isdir(args.destination):
        parser.error(f'Directory {args.destination} not found!')

    return args


def main():
    arguments = parse_arguments()

    if arguments.nightly:
        download_nightly(destination=arguments.destination)
    elif arguments.release:
        download_release(destination=arguments.destination)

    verify_md5sum(destination=arguments.destination)
    unpack_tarbal(destination=arguments.destination)

    version_word = 'nightly' if arguments.nightly else 'release'
    print(f'*** The latest {version_word} has been downloaded to {arguments.destination}')
