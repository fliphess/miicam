# Test Framework

In the test directory you can find 2 testsuites that are used to determine if the current version is master is ready for release.

There are 2 testsuites:

- An HTTP testsuite that needs to run off-camera
- A bats unittest suite for testing the inner workings of the camera. (work in progress)


## HTTP Testsuite

The HTTP testsuite is a collection of tests to verify the validity of all http endpoints of the web interface.
It tests most of the scenario's that can go wrong and is supposed to verify nothing broke while devving on the web interface.

It takes at least 20 minutes to complete, as the camera can be freaking slow, especially when the rtsp is running and streaming to multiple clients.

To run the http testsuite, folow the instructions below :)

!!! note "Using the http testuite"
    The http testsuite is not made to run on the camera itself.
    It needs python and a bit more memory than present on the camera.


### Prerequisites

Before you start, make sure:

- You have `docker,python3, pip3 and virtualenvwrapper` installed.
- You have a running camera with a functional web interface using the default `admin` password in `config.cfg` (`HTTP_API_PASS="drgsrethsfdghs"`).
- Set the hostname or ip address of your camera in `CAMERA_HOSTNAME` in your environment:
  ```
  export CAMERA_HOSTNAME='camera.local'
  ```

### Setup

First, create a virtualenv:

```
cd MiiCamTest
mkvirtualenv -a "$( pwd )" -p "$( which python3 )" miitest
```

Next, we install the requirements:

```
pip3 install -r requirements.txt
```

That's it, you're ready to start the http testsuite :)


### Run the http testsuite

To start the http testsuite, use `nosetest` to run all the unittests:

```
nosetests -v tests/http
```


This will one by one run all the defined http tests:

```
(miitest) $ nosetests tests/http -v
test_that_api_camera_reset_state_returns_200 (test_api_camera_endpoints.TestCameraAPI) ... ok
test_that_api_camera_save_state_returns_200 (test_api_camera_endpoints.TestCameraAPI) ... ok
test_that_api_camera_state_returns_200 (test_api_camera_endpoints.TestCameraAPI) ... ok
test_that_auto_exposure_can_be_turned_on_and_off (test_api_camera_endpoints.TestCameraAPI) ... ok
...
```

### Using docker

If you don't have a computer with python installed, you can use the docker image (if you have docker installed)

To do this, use the manage script to build the container and run the suite:

```
./manage.sh --build && ./manage.sh --run-http
```

## Using the internal testsuite

The internal testsuite is based on [Bats, a bash unittesting framework](https://github.com/bats-core/bats-core/). To update to the latest version, copy the contents of `libexec/bats-core/` to the `bin/` directory of this repository.

!!! note "Using the internal testuite"
    The internal testsuite is meant to be used on the camera itself.
    To use it, clone this repo on the sdcard of the camera or use `scp` or `rsync` to copy the files from your computer to the chuangmi camera.


To run the testsuite, you need to copy it to the camera first.





