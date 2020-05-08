import time

from tests.testsuite import HTTPTestCase


class TestnvramAPI(HTTPTestCase):
    def test_that_the_state_of_all_nvram_keys_can_be_requested(self):
        response = self.client.get('/api/nvram/state')
        response_data = response.json()

        assert response.status_code == 200
        for key in ['light', 'miio_ssid', 'band_nearby', 'model']:
            assert key in response_data

    def test_that_the_state_of_a_single_key_can_be_requested(self):
        response1 = self.client.get('/api/nvram/get/model')
        response1_data = response1.json()

        assert response1.status_code == 200
        assert response1_data["key"] == "model"
        assert 'chuangmi' in response1_data["value"]
        assert response1_data["success"]

        response2 = self.client.get('/api/nvram/get/doesnotexist')
        response2_data = response2.json()

        assert response2.status_code == 200
        assert not response2_data["success"]
        assert not response2_data["exists"]

    def test_a_cycle_list_create_overwrite_delete(self):
        key = "testing_{time}".format(time=int(time.time()))

        response1 = self.client.get(f'/api/nvram/set/{key}/testing')
        response1_data = response1.json()

        assert response1.status_code == 200
        assert response1_data["success"]
        assert response1_data["key"] == key
        assert response1_data["value"] == "testing"

        response2 = self.client.get(f'/api/nvram/set/{key}/testing')
        response2_data = response2.json()

        assert response2.status_code == 200
        assert response2_data["exists"]
        assert not response2_data["success"]

        response3 = self.client.get(f'/api/nvram/overwrite/{key}/somethingelse')
        response3_data = response3.json()

        assert response3.status_code == 200
        assert response3_data["success"]
        assert response3_data["key"] == key
        assert response3_data["value"] == "somethingelse"

        response4 = self.client.get(f'/api/nvram/unset/{key}')
        response4_data = response4.json()

        assert response4.status_code == 200
        assert response4_data["success"]

        response5 = self.client.get(f'/api/nvram/unset/{key}')
        response5_data = response5.json()

        assert response5.status_code == 200
        assert not response5_data["success"]
