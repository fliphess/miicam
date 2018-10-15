<?php

// * Main index page
$app->get('/', function ($request, $response, $args) {
    $this->logger->addInfo('Serving route for / to: ' . $_SERVER['REMOTE_ADDR']);
    return $this->view->render($response, 'index.twig', [
        'title' => 'Main Page',
    ]);
})->setName('/');


// * About page
$app->get('/about', function ($request, $response, $args) {
    $this->logger->addInfo('Serving route for /about to: ' . $_SERVER['REMOTE_ADDR']);
    return $this->view->render($response, 'about.twig', [
        'title' => 'About Page'
    ]);
})->setName('/about');


// * Info page
$app->get('/info', function ($request, $response, $args) {
    $this->logger->addInfo('Serving route for /info to: ' . $_SERVER['REMOTE_ADDR']);
    return phpinfo();
})->setName('/info');


// * Settings page
$app->get('/settings', function ($request, $response, $args) {
    $this->logger->addInfo('Serving route for /settings to: ' . $_SERVER['REMOTE_ADDR']);
    return $this->view->render($response, 'settings.twig', [
        'title' => 'Settings Page'
    ]);
})->setName('/settings');


