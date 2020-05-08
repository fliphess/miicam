import requests

from tests.testsuite import HTTPTestCase


class TestServicesAPI(HTTPTestCase):

    def test_that_the_state_of_all_services_can_be_requested(self):
        response = self.client.get('/api/services/state')
        response_data = response.json()

        assert response.status_code == 200

        service = response_data[0]
        assert "name" in service
        assert "running" in service
        assert "managed" in service

    def test_that_a_running_service_state_can_be_requested(self):
        response = self.client.get("/api/services/lighttpd/running")
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["service"] == "lighttpd"
        assert response_data["state"] == "on"

    def test_that_a_not_running_service_state_returns_false(self):
        response = self.client.get("/api/services/somethingthatdoesnotexist/running")
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["service"] == "somethingthatdoesnotexist"
        assert response_data["state"] == "off"

    def test_that_a_running_service_can_be_restarted(self):
        response = self.client.get("/api/services/crond/restart")
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert response_data["service"] == "crond"
        assert response_data["output"] == "*** Stopping crond Cron Daemon ... *** Starting crond Cron Daemon..."

    def test_that_a_running_service_can_be_status_checked(self):
        response = self.client.get("/api/services/crond/status")
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert response_data["service"] == "crond"
        assert 'Cron Daemon is running with PID' in response_data["output"]

    def test_that_a_not_running_service_cannot_be_started(self):
        with self.assertRaises(requests.exceptions.HTTPError):
            response = self.client.get("/api/services/crond/start")
            response_data = response.json()

            assert response.status_code == 500
            assert response_data["error"]
            assert response_data["message"] == 'Error: service crond is already running!'

    def test_that_a_not_running_service_cannot_be_restarted(self):
        with self.assertRaises(requests.exceptions.HTTPError):
            response = self.client.get("/api/services/somethingthatdoesnotexist/restart")
            response_data = response.json()

            assert response.status_code == 500
            assert response_data["success"]
            assert response_data["message"] == 'Error: service somethingthatdoesnotexist not running!'

    def test_that_a_not_running_service_cannot_be_stopped(self):
        with self.assertRaises(requests.exceptions.HTTPError):
            response = self.client.get("/api/services/somethingthatdoesnotexist/stop")
            response_data = response.json()

            assert response.status_code == 500
            assert response_data["success"]
            assert response_data["message"] == 'Error: service somethingthatdoesnotexist not running!'

    def test_that_a_running_service_can_be_stopped_and_started(self):
        response1 = self.client.get("/api/services/crond/stop")
        response1_data = response1.json()

        assert response1.status_code == 200
        assert response1_data["success"]
        assert response1_data["service"] == "crond"
        assert response1_data["output"] == "*** Stopping crond Cron Daemon ..."

        response2 = self.client.get("/api/services/crond/start")
        response2_data = response2.json()

        assert response2.status_code == 200
        assert response2_data["success"]
        assert response2_data["service"] == "crond"
        assert response2_data["output"] == "*** Starting crond Cron Daemon..."
