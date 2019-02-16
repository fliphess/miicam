set -e -o pipefail

##
## Install miniconda
##

echo "*** Uploading nightly build to anaconda cloud"

export VERSION="$( date +%Y.%m.%d )"
export FILENAME="MiiCam-nightly-${VERSION}.tar.gz"

cp MiiCam.tgz "${FILENAME}"
anaconda -t $CONDA_UPLOAD_TOKEN upload -u miicam -l nightly "$FILENAME" --force --package-type file --package miicam-nightly --version "${VERSION}" --summary "Miicam"
