<?php

$container = $app->getContainer();

// * Register component on container
$container['view'] = function ($container) {
    $view = new \Slim\Views\Twig('../app/templates', ['cache' => false]);
    $basePath = rtrim(str_ireplace('index.php', '', $container->get('request')->getUri()->getBasePath()), '/');
    $view->addExtension(new Slim\Views\TwigExtension($container->get('router'), $basePath));

    return $view;
};


// * Configure view renderer
$container['renderer'] = function ($container) {
    $settings = $container->get('settings')['renderer'];
    $renderer = new Slim\Views\PhpRenderer($settings['template_path']);

    return $rendered;
};


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

