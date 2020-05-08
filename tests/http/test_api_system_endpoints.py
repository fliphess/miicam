from tests.testsuite import HTTPTestCase


class TestSystemAPI(HTTPTestCase):

    def test_that_api_ping_returns_200(self):

        response = self.client.get('/api/ping')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert response_data["data"] == "pong"

    def test_that_api_system_proclist_returns_200(self):
        response = self.client.get('/api/system/proclist')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_system_dmesg_returns_200(self):
        response = self.client.get('/api/system/dmesg')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_diskusage_returns_200(self):
        response = self.client.get('/api/system/diskusage')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_uptime_returns_200(self):
        response = self.client.get('/api/system/uptime')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_meminfo_returns_200(self):
        response = self.client.get('/api/system/meminfo')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_memfree_returns_200(self):
        response = self.client.get('/api/system/memfree')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_cpuinfo_returns_200(self):
        response = self.client.get('/api/system/cpuinfo')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_iwconfig_returns_200(self):
        response = self.client.get('/api/system/iwconfig')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]

    def test_that_api_ifconfig_returns_200(self):
        response = self.client.get('/api/system/ifconfig')
        response_data = response.json()

        assert response.status_code == 200
        assert "output" in response_data
        assert response_data["success"]
