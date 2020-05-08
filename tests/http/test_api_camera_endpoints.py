from tests.testsuite import HTTPTestCase


class TestCameraAPI(HTTPTestCase):

    ELEMENTS = [
        'ir_cut',
        'night_mode',
        'flip_mode',
        'mirror_mode',
        'brightness',
        'contrast',
        'hue',
        'saturation',
        'denoise',
        'sharpness',
        'auto_exposure',
        'auto_white_balance',
        'auto_sharpen',
        'black_level_correction',
        'contrast_enhancement',
        'gamma_curve_correction',
        'noise_reduction',
    ]

    def test_that_api_camera_state_returns_200(self):
        response = self.client.get('/api/camera/state')
        response_data = response.json()

        self.assertEqual(response.status_code, 200)

        for element in self.ELEMENTS + ['ir_led', 'blue_led', 'yellow_led']:
            self.assertIn(element, response_data, msg=f"{element} not found in response data")

    def test_that_api_camera_save_state_returns_200(self):
        response = self.client.get('/api/camera/save_state')
        response_data = response.json()

        self.assertEqual(response.status_code, 200)
        self.assertTrue(response_data["success"])
        self.assertIn("Saving settings", response_data["message"])

    def test_that_api_camera_reset_state_returns_200(self):
        response = self.client.get('/api/camera/reset')
        response_data = response.json()

        self.assertEqual(response.status_code, 200)
        self.assertTrue(response_data["success"])
        self.assertEqual(response_data["message"], "Camera settings have been reset")

    def test_that_ir_cut_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/ir_cut/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "ir_cut")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/ir_cut')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "ir_cut")
        self.assertEqual(response2_data["state"], "off")

        # Turn on
        response3 = self.client.get('/api/camera/ir_cut/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "ir_cut")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/ir_cut')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "ir_cut")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/camera/ir_cut/set/off')

    def test_that_night_mode_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/night_mode/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "night_mode")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/night_mode')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "night_mode")
        self.assertEqual(response2_data["state"], "off")

        # Turn on
        response3 = self.client.get('/api/camera/night_mode/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "night_mode")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/night_mode')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "night_mode")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/camera/night_mode/set/off')

    def test_that_flip_mode_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/flip_mode/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "flip_mode")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/flip_mode')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "flip_mode")
        self.assertEqual(response2_data["state"], "off")

        # Turn on
        response3 = self.client.get('/api/camera/flip_mode/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "flip_mode")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/flip_mode')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "flip_mode")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/camera/flip_mode/set/off')

    def test_that_mirror_mode_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/mirror_mode/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "mirror_mode")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/mirror_mode')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "mirror_mode")
        self.assertEqual(response2_data["state"], "off")

        # Turn on
        response3 = self.client.get('/api/camera/mirror_mode/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "mirror_mode")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/mirror_mode')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "mirror_mode")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/camera/mirror_mode/set/off')

    def test_that_auto_exposure_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/auto_exposure/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "auto_exposure")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/auto_exposure')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "auto_exposure")
        self.assertEqual(response2_data["state"], "disable")

        # Turn on
        response3 = self.client.get('/api/camera/auto_exposure/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "auto_exposure")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/auto_exposure')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "auto_exposure")
        self.assertEqual(response4_data["state"], "enable")

        # Turn off
        self.client.get('/api/camera/auto_exposure/set/off')

    def test_that_auto_white_balance_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/auto_white_balance/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "auto_white_balance")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/auto_white_balance')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "auto_white_balance")
        self.assertEqual(response2_data["state"], "disable")

        # Turn on
        response3 = self.client.get('/api/camera/auto_white_balance/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "auto_white_balance")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/auto_white_balance')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "auto_white_balance")
        self.assertEqual(response4_data["state"], "enable")

        # Turn off
        self.client.get('/api/camera/auto_white_balance/set/off')

    def test_that_auto_sharpen_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/auto_sharpen/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "auto_sharpen")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/auto_sharpen')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "auto_sharpen")
        self.assertEqual(response2_data["state"], "disable")

        # Turn on
        response3 = self.client.get('/api/camera/auto_sharpen/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "auto_sharpen")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/auto_sharpen')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "auto_sharpen")
        self.assertEqual(response4_data["state"], "enable")

        # Turn off
        self.client.get('/api/camera/auto_sharpen/set/off')

    def test_that_black_level_correction_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/black_level_correction/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "black_level_correction")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/black_level_correction')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "black_level_correction")
        self.assertEqual(response2_data["state"], "disable")

        # Turn on
        response3 = self.client.get('/api/camera/black_level_correction/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "black_level_correction")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/black_level_correction')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "black_level_correction")
        self.assertEqual(response4_data["state"], "enable")

        # Turn off
        self.client.get('/api/camera/black_level_correction/set/off')

    def test_that_contrast_enhancement_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/contrast_enhancement/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "contrast_enhancement")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/contrast_enhancement')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "contrast_enhancement")
        self.assertEqual(response2_data["state"], "disable")

        # Turn on
        response3 = self.client.get('/api/camera/contrast_enhancement/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "contrast_enhancement")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/contrast_enhancement')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "contrast_enhancement")
        self.assertEqual(response4_data["state"], "enable")

        # Turn off
        self.client.get('/api/camera/contrast_enhancement/set/off')

    def test_that_gamma_curve_correction_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/gamma_curve_correction/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "gamma_curve_correction")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/gamma_curve_correction')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "gamma_curve_correction")
        self.assertEqual(response2_data["state"], "disable")

        # Turn on
        response3 = self.client.get('/api/camera/gamma_curve_correction/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "gamma_curve_correction")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/gamma_curve_correction')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "gamma_curve_correction")
        self.assertEqual(response4_data["state"], "enable")

        # Turn off
        self.client.get('/api/camera/gamma_curve_correction/set/off')

    def test_that_noise_reduction_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/camera/noise_reduction/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["key"], "noise_reduction")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/camera/noise_reduction')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["key"], "noise_reduction")
        self.assertEqual(response2_data["state"], "disable")

        # Turn on
        response3 = self.client.get('/api/camera/noise_reduction/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["key"], "noise_reduction")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/camera/noise_reduction')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["key"], "noise_reduction")
        self.assertEqual(response4_data["state"], "enable")

        # Turn off
        self.client.get('/api/camera/noise_reduction/set/off')


