<?php

use Psr\Http\Message\RequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;


// * Trailing slash middleware
$app->add(function (Request $request, Response $response, callable $next) {
    $uri = $request->getUri();
    $path = $uri->getPath();
    if ($path != '/' && substr($path, -1) == '/') {
        $uri = $uri->withPath(substr($path, 0, -1));

        if($request->getMethod() == 'GET') {
            return $response->withRedirect((string)$uri, 301);
        }
        else {
            return $next($request->withUri($uri), $response);
        }
    }
    return $next($request, $response);
});


// * Log al requests
$app->add(function (Request $request, Response $response, callable $next) {
    $route = $request->getAttribute('route');
    $response = $next($request, $response);
    $uri = $request->getUri();

    $logline = array(
        "remote_ip" => getenv('REMOTE_ADDR'),
        "method"    => $request->getMethod(),
        "status"    => $response->getStatusCode(),
        "reason"    => $response->getReasonPhrase(),
        "host"      => $uri->getHost(),
        "port"      => $uri->getPort(),
        "uri"       => $uri->getPath(),
        "query"     => $uri->getQuery(),
    );

    $this->logger->info(json_encode($logline));
    return $response;
});
