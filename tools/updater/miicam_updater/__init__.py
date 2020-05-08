import os
import shutil
import subprocess
import sys
import tarfile
import requests


SSH_OPTIONS = (
    ' -x',
    '-o StrictHostKeyChecking=no',
    '-o ConnectTimeout=1',
    '-o PasswordAuthentication=no',
)

RSYNC_OPTIONS = (
    '--modify-window=2',
    '--update',
    '--times',
    '--rsync-path="/tmp/rsync"',
    '--recursive',
    '-z',
)


def unpack_tarbal(destination):
    filename = os.path.join(destination, 'MiiCam.tgz')
    dest_dir = os.path.join(destination, 'sdcard')

    if os.path.isdir(dest_dir):
        shutil.rmtree(dest_dir)

    print(f"*** Unpacking tarbal {filename}")
    os.chdir(destination)

    with tarfile.open(filename, 'r') as t:
        t.extractall()

    dest_dir = os.path.join(destination, 'sdcard')

    if not os.path.isdir(dest_dir):
        print(f"*** Extracting {filename} to {dest_dir} failed")


def download_file(filename, uri):
    with requests.get(uri, stream=True) as response:
        with open(filename, 'wb') as fh:
            for chunk in response.iter_content(chunk_size=2048):
                if chunk:
                    fh.write(chunk)


def verify_md5sum(destination):
    sum_filename = os.path.join(destination, 'MiiCam.tgz.md5sum')
    if not os.path.isfile(sum_filename):
        print(f"*** Error: md5sum file {sum_filename} not found!", sys.stderr)
        sys.exit(1)

    print("*** Matching md5sum")
    try:
        subprocess.check_call(f"cd {destination} && md5sum -c {sum_filename}", shell=True)
    except subprocess.CalledProcessError as e:
        print("*** Error: MD5 of file does not match!", file=sys.stderr)
        sys.exit(1)


def write_md5sum(latest, destination):
    latest_sum = latest['md5']

    sumfile = os.path.join(destination, 'MiiCam.tgz.md5sum')

    with open(sumfile, "w") as fh:
        fh.write(f"{latest_sum}  MiiCam.tgz")


def ssh_arguments():
    ssh_options = " ".join(SSH_OPTIONS)
    return f"ssh {ssh_options}"


def run_command_over_ssh(host, port, user='root', command='exit 0'):
    ssh_command = ssh_arguments()

    try:
        subprocess.check_call(f'{ssh_command} -l "{user}" -p {port} "{host}" "{command}"', shell=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"*** Failed to connect with SSH to {user}@{host}:{port}: {e}", file=sys.stderr)
        sys.exit(1)


def rsync_files(source, dest, host, port, user='root', verbose=False):
    ssh_command = ssh_arguments()
    rsync_arguments = " ".join(RSYNC_OPTIONS)

    rsync_cmd = f"rsync {rsync_arguments} -e '{ssh_command} -p {port}' {source} {user}@{host}:{dest}"

    if verbose:
        rsync_cmd = f"{rsync_cmd} -v"

    try:
        subprocess.check_call(rsync_cmd, shell=True)
        print(f'Rsync for {source} to {dest} at {host} [OK]')
        return True
    except subprocess.CalledProcessError as e:
        print(f'*** Rsync for {source} to {dest} at {host} [FAILED]')
        print(f'*** Error was: {e}')
        return False
