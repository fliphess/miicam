from tests.testsuite import HTTPTestCase


class TestLEDSAPI(HTTPTestCase):

    def test_that_api_led_state_returns_200(self):
        response = self.client.get('/api/led/state')
        response_data = response.json()

        self.assertEqual(response.status_code, 200)
        self.assertIn("blue_led", response_data, msg="Blue led not found in response data")
        self.assertIn("yellow_led", response_data, msg="Yellow led not found in response data")
        self.assertIn("ir_led", response_data, msg="IR led not found in response data")

    def test_that_led_blue_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/led/blue_led/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["led"], "blue_led")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/led/blue_led')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["led"], "blue_led")
        self.assertEqual(response2_data["state"], "off")

        # Turn on
        response3 = self.client.get('/api/led/blue_led/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["led"], "blue_led")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/led/blue_led')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["led"], "blue_led")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/led/blue_led/set/off')

    def test_that_blue_led_can_be_set_to_blink_and_off(self):

        # Turn blink
        response3 = self.client.get('/api/led/blue_led/set/blink')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["led"], "blue_led")
        self.assertEqual(response3_data["action"], "TurnBlink")

        # Get state
        response4 = self.client.get('/api/led/blue_led')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["led"], "blue_led")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/led/blue_led/set/off')

    def test_that_led_yellow_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/led/yellow_led/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["led"], "yellow_led")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/led/yellow_led')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["led"], "yellow_led")
        self.assertEqual(response2_data["state"], "off")

        # Turn on
        response3 = self.client.get('/api/led/yellow_led/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["led"], "yellow_led")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/led/yellow_led')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["led"], "yellow_led")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/led/yellow_led/set/off')

    def test_that_yellow_led_can_be_set_to_blink_and_off(self):

        # Turn blink
        response3 = self.client.get('/api/led/yellow_led/set/blink')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["led"], "yellow_led")
        self.assertEqual(response3_data["action"], "TurnBlink")

        # Get state
        response4 = self.client.get('/api/led/yellow_led')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["led"], "yellow_led")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/led/yellow_led/set/off')

    def test_that_led_ir_can_be_turned_on_and_off(self):

        # Turn off
        response1 = self.client.get('/api/led/ir_led/set/off')
        response1_data = response1.json()

        self.assertEqual(response1.status_code, 200)
        self.assertTrue(response1_data["success"])
        self.assertEqual(response1_data["led"], "ir_led")
        self.assertEqual(response1_data["action"], "TurnOff")

        # Get state
        response2 = self.client.get('/api/led/ir_led')
        response2_data = response2.json()

        self.assertEqual(response2.status_code, 200)
        self.assertEqual(response2_data["led"], "ir_led")
        self.assertEqual(response2_data["state"], "off")

        # Turn on
        response3 = self.client.get('/api/led/ir_led/set/on')
        response3_data = response3.json()

        self.assertEqual(response3.status_code, 200)
        self.assertTrue(response3_data["success"])
        self.assertEqual(response3_data["led"], "ir_led")
        self.assertEqual(response3_data["action"], "TurnOn")

        # Get state
        response4 = self.client.get('/api/led/ir_led')
        response4_data = response4.json()

        self.assertEqual(response4.status_code, 200)
        self.assertEqual(response4_data["led"], "ir_led")
        self.assertEqual(response4_data["state"], "on")

        # Turn off
        self.client.get('/api/led/ir_led/set/off')
