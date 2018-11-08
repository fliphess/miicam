<?php

$container = $app->getContainer();

// * Setup Logging
$container['logger'] = function ($container) {
    $settings = $container->get('settings')['logger'];

    $logger = new Monolog\Logger($settings['name']);
    $logger->pushProcessor(new Monolog\Processor\UidProcessor());
    $logger->pushHandler(new Monolog\Handler\StreamHandler($settings['path'], $settings['level']));

    return $logger;
};


// * Error handler
$container['errorHandler'] = function ($container) {
    return function ($request, $response, $exception) use ($container) {
        $data = array(
            'message' => $exception->getMessage(),
            'error'   => true,
        );
        return $container['response']->withStatus(500)
                                     ->withHeader('Content-Type', 'application/json')
                                     ->write(json_encode($data));
    };
};

