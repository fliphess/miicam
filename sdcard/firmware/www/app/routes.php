<?php

use Slim\Http\Request;
use Slim\Http\Response;

// * Generic routes (/, /about, /settings, /info)
require "routes/main.php";

// * API routes
require "routes/api.php";

// * Last route (404)
$app->map(['GET', 'POST', 'PUT', 'DELETE', 'PATCH'], '/{routes:.+}', function($req, $res) {
    $handler = $this->notFoundHandler; // handle using the default Slim page not found handler
    return $handler($req, $res);
});
