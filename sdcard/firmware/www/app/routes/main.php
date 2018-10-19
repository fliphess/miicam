<?php

use Slim\Http\Request;
use Slim\Http\Response;


// * Main index page
$app->get('/', function ($request, $response, $args) {
    return $this->view->render($response, 'pages/index.twig', [
        'title' => 'Main Page',
    ]);
})->setName('/');


// * About page
$app->get('/overview', function ($request, $response, $args) {
    return $this->view->render($response, 'pages/overview.twig', [
        'title'    => 'Overview Page',
        'hostname' => getenv('HOSTNAME'),
    ]);
})->setName('/about');


// * Info page
$app->get('/info', function ($request, $response, $args) {
    return phpinfo();
})->setName('/info');


// * Settings page
$app->get('/settings', function ($request, $response, $args) {
    return $this->view->render($response, 'pages/settings.twig', [
        'title' => 'Settings Page'
    ]);
})->setName('/settings');


// * API Docs
$app->get('/apidocs', function ($request, $response, $args) {
     return $this->view->render($response, 'pages/apidocs.twig', [
         'title' => 'API Docs Page'
     ]);
})->setName('/apidocs');

