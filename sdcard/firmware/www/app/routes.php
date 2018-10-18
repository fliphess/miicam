<?php

// * routes
require "routes/main.php";
require "routes/logview.php";
require "routes/api.php";

// * Last route (404)
$app->map(['GET', 'POST', 'PUT', 'DELETE', 'PATCH'], '/{routes:.+}', function($req, $res) {
    $handler = $this->notFoundHandler; // handle using the default Slim page not found handler
    return $handler($req, $res);
});
