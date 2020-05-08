from tests.testsuite import HTTPTestCase


class TestRecordAPI(HTTPTestCase):

    def test_that_a_recording_can_be_requested(self):
        response = self.client.get('/api/record/create')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]

        assert "/tmp/sd/RECORDED_VIDEOS" in response_data["filename"]
        assert "/videos/" in response_data["url"]
        assert response_data["output"] == "VideoRecording created"

    def test_that_last_url_endpoint_returns_a_url(self):
        response = self.client.get('/api/record/get_url')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert '/videos/' in response_data["url"]

    def test_that_last_path_endpoint_returns_a_path(self):
        response = self.client.get('/api/record/get_last')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert "/tmp/sd/RECORDED_VIDEOS" in response_data["filename"]
