from tests.testsuite import HTTPTestCase


class TestSnapshotAPI(HTTPTestCase):

    def test_that_a_snapshot_can_be_requested(self):
        response = self.client.get('/api/snapshot/create')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]

        assert "/tmp/sd/RECORDED_IMAGES" in response_data["filename"]
        assert "/snapshots/" in response_data["url"]
        assert response_data["output"] == "Snapshot created"

    def test_that_last_url_endpoint_returns_a_url(self):
        response = self.client.get('/api/snapshot/get_url')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert '/snapshots/' in response_data["url"]

    def test_that_last_path_endpoint_returns_a_path(self):
        response = self.client.get('/api/snapshot/get_last')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert "/tmp/sd/RECORDED_IMAGES" in response_data["filename"]

    def test_that_api_snapshot_image_returns_an_image(self):
        response = self.client.get('/api/snapshot/image')

        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.headers['Content-Type'], 'image/jpeg')
