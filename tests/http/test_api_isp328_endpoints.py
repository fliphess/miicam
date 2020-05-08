from tests.testsuite import HTTPTestCase


class TestISP328API(HTTPTestCase):

    def test_that_api_camera_isp328_returns_200(self):
        response = self.client.get('/api/camera/isp328')
        response_data = response.json()

        self.assertEqual(response.status_code, 200)
        self.assertIn("keys", response_data)

    def test_that_brightness_can_be_changed(self):
        # Set
        response1 = self.client.get('/api/camera/isp328/set/brightness/100')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])

        self.assertEqual(response1_data["key"], "brightness")
        self.assertEqual(response1_data["value"], "100")

        # Get State
        response2 = self.client.get('/api/camera/isp328/get/brightness')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertTrue(response2_data["success"])

        self.assertEqual(response2_data["key"], "brightness")
        self.assertEqual(response2_data["value"], "100")

        # Set to 128
        self.client.get('/api/camera/isp328/set/brightness/128')

    def test_that_contrast_can_be_changed(self):
        # Set
        response1 = self.client.get('/api/camera/isp328/set/contrast/100')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])

        self.assertEqual(response1_data["key"], "contrast")
        self.assertEqual(response1_data["value"], "100")

        # Get State
        response2 = self.client.get('/api/camera/isp328/get/contrast')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertTrue(response2_data["success"])

        self.assertEqual(response2_data["key"], "contrast")
        self.assertEqual(response2_data["value"], "100")

        # Set to 128
        self.client.get('/api/camera/isp328/set/contrast/128')

    def test_that_hue_can_be_changed(self):
        # Set
        response1 = self.client.get('/api/camera/isp328/set/hue/100')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])

        self.assertEqual(response1_data["key"], "hue")
        self.assertEqual(response1_data["value"], "100")

        # Get State
        response2 = self.client.get('/api/camera/isp328/get/hue')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertTrue(response2_data["success"])

        self.assertEqual(response2_data["key"], "hue")
        self.assertEqual(response2_data["value"], "100")

        # Set to 128
        self.client.get('/api/camera/isp328/set/hue/128')

    def test_that_saturation_can_be_changed(self):
        # Set
        response1 = self.client.get('/api/camera/isp328/set/saturation/100')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])

        self.assertEqual(response1_data["key"], "saturation")
        self.assertEqual(response1_data["value"], "100")

        # Get State
        response2 = self.client.get('/api/camera/isp328/get/saturation')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertTrue(response2_data["success"])

        self.assertEqual(response2_data["key"], "saturation")
        self.assertEqual(response2_data["value"], "100")

        # Set to 128
        self.client.get('/api/camera/isp328/set/saturation/128')

    def test_that_denoise_can_be_changed(self):
        # Set
        response1 = self.client.get('/api/camera/isp328/set/denoise/100')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])

        self.assertEqual(response1_data["key"], "denoise")
        self.assertEqual(response1_data["value"], "100")

        # Get State
        response2 = self.client.get('/api/camera/isp328/get/denoise')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertTrue(response2_data["success"])

        self.assertEqual(response2_data["key"], "denoise")
        self.assertEqual(response2_data["value"], "100")

        # Set to 128
        self.client.get('/api/camera/isp328/set/denoise/128')

    def test_that_sharpness_can_be_changed(self):
        # Set
        response1 = self.client.get('/api/camera/isp328/set/sharpness/100')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])

        self.assertEqual(response1_data["key"], "sharpness")
        self.assertEqual(response1_data["value"], "100")

        # Get State
        response2 = self.client.get('/api/camera/isp328/get/sharpness')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertTrue(response2_data["success"])

        self.assertEqual(response2_data["key"], "sharpness")
        self.assertEqual(response2_data["value"], "100")

        # Set to 128
        self.client.get('/api/camera/isp328/set/sharpness/128')
