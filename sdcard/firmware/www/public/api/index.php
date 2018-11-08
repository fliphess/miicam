<?php

require '../../vendor/autoload.php';
require '../../libs/autoload.php';

if (PHP_SAPI == 'cli-server') {
    $url  = parse_url($_SERVER['REQUEST_URI']);
    $file = __DIR__ . $url['path'];
    if (is_file($file)) {
        return false;
    }
}

// * Setup dotenv
$dotenv = new Dotenv\Dotenv('/tmp/sd', 'config.cfg');
$dotenv->load();

// Setup slim
$settings = require('../../app/settings.php');
$app = new \Slim\App($settings);

// * Set up dependencies
require('../../app/dependencies.php');

// * Register middleware
require('../../app/middleware.php');

// * Register routes
require('../../app/routes.php');

// * Run SLIM framework
$app->run();
