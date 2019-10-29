#!/usr/bin/env python3
import argparse
import json
import os
import subprocess

from multiprocessing.pool import Pool


class ParallelDownloader:
    def __init__(self, procs, func):
        self.function = func
        self.pool = Pool(procs)

    def call(self, *args, **kwargs):
        self.pool.apply_async(self.function, args, kwargs)

    def wait(self):
        self.pool.close()
        self.pool.join()


def download(task):
    destination, uri = task
    command_line = f"wget -q -t 5 -T 10 -c -O {destination} {uri}"

    child = subprocess.Popen(command_line.split(), stdout=subprocess.PIPE)
    output = child.communicate()[0]

    return dict(name=name, exit=child.returncode, output=output)


def format_downloads(data, destdir):
    for package, package_data in data.items():
        version = package_data.get('version')
        archive = package_data.get('archive').format(version=version)

        uri = package_data.get('uri').format(version=version, archive=archive)
        destination = os.path.join(destdir, archive)

        if os.path.isfile(destination):
            print(f"    --> File exists: {destination}")
            continue

        yield destination, uri


def parse_arguments():
    parser = argparse.ArgumentParser(description="Download sources")
    parser.add_argument(
        '-d',
        '--dest',
        dest="destination",
        type=str,
        default="./src",
        help="The destination to download the sources to")

    parser.add_argument(
        '-f',
        '--file',
        dest="filename",
        type=str,
        default="sources.json",
        help="The json file with source definitions")

    parser.add_argument(
        "-p",
        "--procs",
        dest="procs",
        type=int,
        default=15,
        help="The amount of processes to use running downloads in parallel")

    args =  parser.parse_args()

    if not os.path.isfile(args.filename):
        parser.error("Json file {} not found!".format(args.filename))

    if not os.path.isdir(args.destination):
        os.mkdir(args.destination)

    return args


def main():
    arguments = parse_arguments()

    with open(arguments.filename) as fh:
        sources_dict = json.loads(fh.read())

    runner = ParallelDownloader(procs=arguments.procs, func=download)

    for task in format_downloads(sources_dict, arguments.destination):
        print("  --> Starting {}".format(task))
        runner.call(task)

    print("Waiting for all downloads to be completed")
    runner.wait()


if __name__ == "__main__":
    main()
