from tests.testsuite import HTTPTestCase


class TestHTMLPages(HTTPTestCase):

    def test_that_main_page_responds_with_content_and_status_200(self):
        response = self.client.get('/')
        assert response.status_code == 200
        assert '<title>Main Page</title>' in str(response.content)

    def test_that_index_page_responds_with_content_and_status_200(self):
        response = self.client.get('/index.html')
        assert response.status_code == 200
        assert '<title>Main Page</title>' in str(response.content)

    def test_that_apidocs_page_responds_with_content_and_status_200(self):
        response = self.client.get("/apidocs.html")
        assert response.status_code == 200
        assert '<title>API Docs</title>' in str(response.content)

    def test_that_config_responds_with_content_and_status_200(self):
        response = self.client.get("/config.html")
        assert response.status_code == 200
        assert '<title>Config Editor</title>' in str(response.content)

    def test_that_logviewer_responds_with_content_and_status_200(self):
        response = self.client.get("/logviewer.html")
        assert response.status_code == 200
        assert '<title>Logviewer</title>' in str(response.content)

    def test_that_services_responds_with_content_and_status_200(self):
        response = self.client.get("/services.html")
        assert response.status_code == 200
        assert '<title>Services</title>' in str(response.content)

    def test_that_settings_responds_with_content_and_status_200(self):
        response = self.client.get("/settings.html")
        assert response.status_code == 200
        assert '<title>Settings</title>' in str(response.content)

    def test_that_snapshot_responds_with_content_and_status_200(self):
        response = self.client.get("/snapshot.html")
        assert response.status_code == 200
        assert '<title>Snapshot</title>' in str(response.content)

    def test_that_system_page_responds_with_content_and_status_200(self):
        response = self.client.get("/system.html")
        assert response.status_code == 200
        assert '<title>System</title>' in str(response.content)
