<?php
return [
    'settings' => [
        'determineRouteBeforeAppMiddleware' => true,
        'displayErrorDetails'               => false, // set to false in production
        'addContentLengthHeader'            => false, // Allow the web server to send the content-length header
        'responseChunkSize'                 => 1024,

        // Monolog settings
        'logger' => [
            'name' => 'chuangmi-app',
            'path' => '../../../../log/webapp.log',
            'level' => \Monolog\Logger::DEBUG,
        ],
    ],
];

