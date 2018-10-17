<?php

// * Main index page
$app->get('/', function ($request, $response, $args) {
    return $this->view->render($response, 'index.twig', [
        'title' => 'Main Page',
    ]);
})->setName('/');


// * About page
$app->get('/about', function ($request, $response, $args) {
    return $this->view->render($response, 'about.twig', [
        'title'    => 'About Page',
        'hostname' => getenv('HOSTNAME'),
    ]);
})->setName('/about');


// * Info page
$app->get('/info', function ($request, $response, $args) {
    return phpinfo();
})->setName('/info');


// * Settings page
$app->get('/settings', function ($request, $response, $args) {
    return $this->view->render($response, 'settings.twig', [
        'title' => 'Settings Page'
    ]);
})->setName('/settings');


// * API Docs
$app->get('/apidocs', function ($request, $response, $args) {
     return $this->view->render($response, 'apidocs.twig', [
         'title' => 'API Docs Page'
     ]);
})->setName('/apidocs');
