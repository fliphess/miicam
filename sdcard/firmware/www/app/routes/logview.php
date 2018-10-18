<?php

    // **************************************************************
    // ** Logviewer                                                **
    // **************************************************************

    // * Log Viewer page
    $app->get('/logviewer', function ($request, $response, $args) {
         return $this->view->render($response, 'pages/logviewer.twig', [
             'title' => 'Log viewer'
         ]);
    })->setName('/logviewer');

    // * Ajax endpoint
    $app->get('/logviewer/{log}/refresh', function ($request, $response, $args) {
        $log  = $args['log'];
        $seek = $request->getQueryParam('seek');
        $data = LogTailer::Get($log, $seek);
        return $response->withJson($data);
    })->setName('/logviewer/refresh');

?>
