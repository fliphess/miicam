import argparse
import os

from miicam_updater import run_command_over_ssh, rsync_files


def parse_arguments():
    parser = argparse.ArgumentParser(description="MiiCam Updater")

    parser.add_argument(
        '--host',
        required=True,
        type=str,
        dest="hostname",
        help="The hostname of the camera")

    parser.add_argument(
        '--user',
        type=str,
        default='root',
        dest="username",
        help="The username for logging in")

    parser.add_argument(
        '--port',
        default=22,
        type=int,
        dest="port",
        help="The SSH port of the camera")

    parser.add_argument(
        '--source',
        required=True,
        type=str,
        dest="source",
        help="The directory containing the sdcard directory")

    parser.add_argument(
        '-v',
        '--verbose',
        action="store_true",
        default=False,
        dest="verbose",
        help="Be more verbose")

    args = parser.parse_args()

    source = os.path.join(args.source, 'sdcard')
    if not os.path.isdir(source):
        parser.error(f'Directory {source} not found!')
    args.source = source

    return args


def main():
    arguments = parse_arguments()

    print("*** Removing generated config in new files")
    config_file = os.path.join(arguments.source, 'config.cfg')
    if os.path.isfile(config_file):
        os.unlink(config_file)

    print("*** Testing SSH connection")
    run_command_over_ssh(
        host=arguments.hostname,
        user=arguments.username,
        port=arguments.port,
        command="exit 0")

    print("*** Running pre-update script")
    run_command_over_ssh(
        host=arguments.hostname,
        user=arguments.username,
        port=arguments.port,
        command="test -f /tmp/sd/firmware/scripts/update/pre-update "
                "&& bash -c /tmp/sd/firmware/scripts/update/pre-update")

    print(f"*** Uploading files from {arguments.source}")
    rsync_files(
        source=f'{arguments.source}/*',
        dest='/tmp/sd',
        host=arguments.hostname,
        port=arguments.port,
        user=arguments.username,
        verbose=arguments.verbose)

    print("*** Running post-update script and reboot")
    run_command_over_ssh(
        host=arguments.hostname,
        user=arguments.username,
        port=arguments.port,
        command="test -f /tmp/sd/firmware/scripts/update/post-update "
                "&& bash -c /tmp/sd/firmware/scripts/update/post-update")
