from tests.testsuite import HTTPTestCase


class TestConfigAPI(HTTPTestCase):

    def test_that_a_config_can_read_and_written(self):
        # Read configuration
        response1 = self.client.get('/api/config/read')
        response1_data = response1.json()

        assert response1.status_code == 200
        assert response1_data["success"]
        assert "AUTO_NIGHT_MODE" in str(response1_data["content"])

        # Write configuration
        response2 = self.client.post('/api/config/write', data={"content": response1_data.get('content')})
        response2_data = response2.json()

        assert response2.status_code == 200
        assert response2_data["success"]
        assert "Config written successfully" in str(response2_data["message"])

    def test_that_api_config_test_returns_200(self):
        response = self.client.get('/api/config/test')
        data = response.json()

        assert response.status_code == 200
        assert data["success"]
        assert data["message"] == "Syntax OK"

    def test_that_config_backups_can_be_deleted(self):

        # delete
        response1 = self.client.get('/api/config/backup/remove')
        response1_data = response1.json()

        assert response1.status_code == 200
        assert response1_data["success"]
        assert response1_data["message"] == "Backups deleted"

        # list
        response2 = self.client.get('/api/config/backup/list')
        response2_data = response2.json()

        assert response2.status_code == 200
        assert response2_data == []

    def test_that_a_config_backup_can_be_created(self):
        # create
        response = self.client.get('/api/config/backup/create')
        response_data = response.json()

        assert response.status_code == 200
        assert response_data["success"]
        assert "New backup created" in response_data["message"]

    def test_that_a_config_backup_can_be_restored(self):

        # Create
        ret = self.client.get('/api/config/backup/create')
        assert ret.status_code == 200

        # list
        response1 = self.client.get('/api/config/backup/list')
        response1_data = response1.json()
        assert response1.status_code == 200

        # Restore
        response2 = self.client.get('/api/config/restore/{}'.format(response1_data[0]))
        response2_data = response2.json()

        assert response2.status_code == 200
        assert response2_data["success"]
        assert 'restored to /tmp/sd/config.cfg' in response2_data["message"]
