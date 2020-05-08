import os
import re
import unittest
from urllib.parse import urlsplit, urlunsplit

import requests
from requests.auth import HTTPBasicAuth


class Client(object):
    def __init__(self, base_url=None, headers=None):
        self.base_url = base_url

        self.session = requests.Session()
        self.session.auth = HTTPBasicAuth("admin", "drgsrethsfdghs")

        self.headers = dict()

        if headers:
            self.headers.update(headers)

    def request(self, method, path, *args, **kwargs):
        url = self.format_absolute_url(path)

        request_method = getattr(self.session, method)

        response = request_method(
            url,
            headers=self.headers,
            allow_redirects=False,
            *args,
            **kwargs)

        response.raise_for_status()

        return response

    def format_absolute_url(self, path):
        def join_and_clean_paths(path1, path2):
            return re.sub(r'/+', '/', path1 + '/' + path2)

        urlparts = list(urlsplit(self.base_url))
        urlparts[2] = join_and_clean_paths(urlparts[2], path)
        return urlunsplit(urlparts)

    def get(self, path, *args, **kwargs):
        return self.request("get", path, *args, **kwargs)

    def post(self, path, *args, **kwargs):
        return self.request("post", path, *args, **kwargs)


class HTTPTestCase(unittest.TestCase):
    _multiprocess_can_split_ = True

    def setUp(self):
        super().setUp()
        self.base_url = "http://{}".format(os.environ.get('CAMERA_HOSTNAME'))
        self.client = Client(base_url=self.base_url)
