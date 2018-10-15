<?php
return [
    'settings' => [
        'displayErrorDetails'    => false, // set to false in production
        'addContentLengthHeader' => false, // Allow the web server to send the content-length header
        'routerCacheFile'        => '/tmp/sd/firmware/tmp/routes.cache',
        'responseChunkSize'      => 1024,

        // Renderer settings
        'renderer' => [
            'template_path' => '../app/templates/',
        ],

        // Monolog settings
        'logger' => [
            'name' => 'chuangmi-app',
            'path' => '../../../log/webapp.log',
            'level' => \Monolog\Logger::DEBUG,
        ],
    ],
];
