<?php

$app->group('/api', function () use ($app) {

    // **************************************************************
    // ** API docs	                                               **
    // **************************************************************
    $app->get('', function ($request, $response, $args) {
        $this->logger->addInfo('Serving route for /api to: ' . $_SERVER['REMOTE_ADDR']);

        return $this->view->render($response, 'apidocs.twig', [
            'title' => 'API Docs Page'
        ]);
    })->setName('/api');

    // **************************************************************
    // ** Ping                                                     **
    // **************************************************************
    $app->get('/ping', function ($request, $response) {
        $this->logger->addInfo('Serving route for /api/ping to: ' . $_SERVER['REMOTE_ADDR']);

        return $response->withJson(array("success" => true, "data" => "pong"), 200);
    })->setName('/api/ping');

    // **************************************************************
    // ** WIFI                                                     **
    // **************************************************************
    $app->group('/wifi', function () use ($app) {

        $app->get('', function ($request,$response) {
            $this->logger->addInfo('Serving route for /api/wifi to: ' . $_SERVER['REMOTE_ADDR']);

            $wifistate = WIFI::DataDict();
            $data = ($wifistate) ? $wifistate : array("success" => false);
            return $response->withJson($data);
        })->setName('/api/wifi');

    });

    // **************************************************************
    // ** LEDS                                                     **
    // **************************************************************
    $app->group('/led', function () use ($app) {

        $app->get('', function ($request,$response) {
            $this->logger->addInfo('Serving route for /api/led to: ' . $_SERVER['REMOTE_ADDR']);

            $ledstate = LED::LedState();
            $data = ($ledstate) ? $ledstate : array("success" => false);
            return $response->withJson($data);
        })->setName('/api/led');

        // *********************************
        // ** Blue LED                    **
        // *********************************
        $app->get('/blue', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/blue to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                "led"   => "blue_led",
                "state" => (Blue_Led::IsOn()) ? "on" : "off",
            );
            return $response->withJson($data);
        })->setName('/api/led/blue');

        $app->get('/blue/set/on', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/blue/set/on to: ' . $_SERVER['REMOTE_ADDR']);

            Blue_Led::TurnOn();

            $data    = array(
                "led"     => "blue_led",
                "action"  => "TurnOn",
                "success" => (Blue_Led::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/blue/set/on');

        $app->get('/blue/set/blink', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/blue/set/blink to: ' . $_SERVER['REMOTE_ADDR']);

           Blue_Led::TurnBlink();

            $data = array(
                "led"     => "blue_led",
                "action"  => "TurnBlink",
                "success" => (Blue_Led::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/blue/set/blink');

        $app->get('/blue/set/off', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/blue/set/off to: ' . $_SERVER['REMOTE_ADDR']);

            Blue_Led::TurnOff();

            $data = array(
                "led"     => "blue_led",
                "action"  => "TurnOff",
                "success" => (Blue_Led::IsOff()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/blue/set/off');

        // *********************************
        // ** Yellow LED                  **
        // *********************************
        $app->get('/yellow', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/yellow to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                "led"   => "yellow_led",
                "state" => (Yellow_Led::IsOn()) ? "on" : "off",
            );
            return $response->withJson($data);
        })->setName('/api/led/yellow');

        $app->get('/yellow/set/on', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/yellow/set/on to: ' . $_SERVER['REMOTE_ADDR']);

            Yellow_Led::TurnOn();
            $data = array(
                "led"     => "yellow_led",
                "action"  => "TurnOn",
                "success" => (Yellow_Led::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/yellow/set/on');

        $app->get('/yellow/set/blink', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/yellow/set/blink to: ' . $_SERVER['REMOTE_ADDR']);

            Yellow_Led::TurnBlink();
            $data    = array(
                "led"     => "yellow_led",
                "action"  => "TurnBlink",
                "success" => (Yellow_Led::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/yellow/set/blink');

        $app->get('/yellow/set/off', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/yellow/set/off to: ' . $_SERVER['REMOTE_ADDR']);

            Yellow_Led::TurnOff();
            $data    = array(
                "led"     => "yellow_led",
                "action"  => "TurnOff",
                "success" => (Yellow_Led::IsOff()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/yellow/set/off');

        // *********************************
        // ** IR LED                      **
        // *********************************
        $app->get('/ir', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/ir to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                "led" => "ir_led",
                "state" => (IR_Led::IsOn()) ? "on" : "off",
            );
            return $response->withJson($data);
        })->setName('/api/led/ir');

        $app->get('/ir/set/on', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/ir/set/on to: ' . $_SERVER['REMOTE_ADDR']);

            IR_Led::TurnOn();

            $data = array(
                "led"     => "ir_led",
                "action"  => "TurnOn",
                "success" => (IR_Led::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/ir/set/on');

        $app->get('/ir/set/off', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/led/ir/set/off to: ' . $_SERVER['REMOTE_ADDR']);

            IR_Led::TurnOff();

            $data = array(
                "led"     => "ir_led",
                "action"  => "TurnOff",
                "success" => (IR_Led::IsOff()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/led/ir/set/off');
    });


    // **************************************************************
    // ** Camera                                                   **
    // **************************************************************
    $app->group('/camera', function () use ($app) {

        $app->get('', function ($request,$response) {
            $this->logger->addInfo('Serving route for /api/camera to: ' . $_SERVER['REMOTE_ADDR']);

            $camerastate = CameraState();
            $data = ($camerastate) ? $camerastate : array("success" => false);
            return $response->withJson($data);
        })->setName('/api/camera');

        // *********************************
        // ** ISP328                      **
        // *********************************
        $app->get('/isp328', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/camera/isp328 to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                "keys"   => ISP328::$all_isp_keys,
            );
            return $response->withJson($data);
        })->setName('/api/camera/isp328');

        $app->get('/isp328/get/{key}', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/isp328/get to: ' . $_SERVER['REMOTE_ADDR']);

            $key   = $args['key'];
            $value = ISP328::Get($key);

            $data = array(
                "key"     => $key,
                "value"   => $value,
                "success" => ($value) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/isp328/get');

        $app->get('/isp328/set/{key}/{value}', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/isp32/set to: ' . $_SERVER['REMOTE_ADDR']);

            $key    = $args['key'];
            $value  = $args['value'];

            $changed   = ISP328::Set($key, $value);
            $success =  ($changed) ? true : false;

            $data = array(
                "key"     => $key,
                "value"   => $changed,
                "success" => $success,
            );
            return $response->withJson($data);
        })->setName('/api/camera/isp328/set');

        // *********************************
        // ** IR Cut                      **
        // *********************************
        $app->get('/ir_cut', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/ir_cut to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                 "key" => "ir_cut",
                 "state" => IR_Cut::IsOn() ? "on" : "off",
             );
             return $response->withJson($data);
        })->setName('/api/camera/ir_cut');

        $app->get('/ir_cut/set/on', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/ir_cut/set/on to: ' . $_SERVER['REMOTE_ADDR']);

            IR_Cut::TurnOn();

            $data = array(
                "key"     => "ir_cut",
                "action"  => "TurnOn",
                "success" => (IR_Cut::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/ir_cut/set/on');

        $app->get('/ir_cut/set/off', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/ir_cut/set/off to: ' . $_SERVER['REMOTE_ADDR']);

            IR_Cut::TurnOff();
            $data = array(
                "led"     => "ir_cut",
                "action"  => "TurnOff",
                "success" => (IR_Cut::IsOff()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/ir_cut/set/off');

        // *********************************
        // ** Night Mode                  **
        // *********************************
        $app->get('/night_mode', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/night_mode to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                 "key" => "night_mode",
                 "state" => NightMode::IsOn() ? "on" : "off",
             );
             return $response->withJson($data);
        })->setName('/api/camera/night_mode');

        $app->get('/night_mode/set/on', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/night_mode/set/on to: ' . $_SERVER['REMOTE_ADDR']);

            NightMode::TurnOn();

            $data = array(
                "key"     => "night_mode",
                "action"  => "TurnOn",
                "success" => (NightMode::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/night_mode/set/on');

        $app->get('/night_mode/set/off', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/night_mode_set/off to: ' . $_SERVER['REMOTE_ADDR']);

            NightMode::TurnOff();
            $data = array(
                "led"     => "night_mode",
                "action"  => "TurnOff",
                "success" => (NightMode::IsOff()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/night_mode/set/off');

        // *********************************
        // ** Flip Mode                   **
        // *********************************
        $app->get('/flip_mode', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/flip_mode to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                 "key" => "flip_mode",
                 "state" => FlipMode::IsOn() ? "on" : "off",
             );
             return $response->withJson($data);
        })->setName('/api/camera/flip_mode');

        $app->get('/flip_mode/set/on', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/flip_mode/set/on to: ' . $_SERVER['REMOTE_ADDR']);

            FlipMode::TurnOn();
            $data = array(
                "key"     => "flip_mode",
                "action"  => "TurnOn",
                "success" => (FlipMode::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/flip_mode/set/on');

        $app->get('/flip_mode/set/off', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/flip_mode/set/off to: ' . $_SERVER['REMOTE_ADDR']);

            FlipMode::TurnOff();
            $data = array(
                "led"     => "flip_mode",
                "action"  => "TurnOff",
                "success" => (FlipMode::IsOff()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/flip_mode/set/off');

        // *********************************
        // ** Mirror Mode                 **
        // *********************************
        $app->get('/mirror_mode', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/mirror_mode to: ' . $_SERVER['REMOTE_ADDR']);

            $data = array(
                 "key" => "mirror_mode",
                 "state" => MirrorMode::IsOn() ? "on" : "off",
             );
             return $response->withJson($data);
        })->setName('/api/camera/mirror_mode');

        $app->get('/mirror_mode/set/on', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/mirror_mode/set/on to: ' . $_SERVER['REMOTE_ADDR']);

            MirrorMode::TurnOn();
            $data = array(
                "key"     => "mirror_mode",
                "action"  => "TurnOn",
                "success" => (MirrorMode::IsOn()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/mirror_mode/set/on');

        $app->get('/mirror_mode/set/off', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/camera/mirror_mode/set/off to: ' . $_SERVER['REMOTE_ADDR']);

            MirrorMode::TurnOff();
            $data = array(
                "led"     => "mirror_mode",
                "action"  => "TurnOff",
                "success" => (MirrorMode::IsOff()) ? true : false,
            );
            return $response->withJson($data);
        })->setName('/api/camera/mirror_mode/set/off');
    });


    // **************************************************************
    // ** NVRAM                                                    **
    // **************************************************************
    $app->group('/nvram', function () use ($app) {

        $app->get('', function ($request,$response) {
            $this->logger->addInfo('Serving route for /api/nvram to: ' . $_SERVER['REMOTE_ADDR']);

            $nvram = NVRAM::Show();
            $data  = ($nvram) ? $nvram : array("success" => false);
            return $response->withJson($data);
        });

        $app->get('/get/{key}', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/nvram/get to: ' . $_SERVER['REMOTE_ADDR']);

            $key = $args['key'];
            $exists = NVRAM::Exists($key);

            if ($exists) {
                $value = NVRAM::Get($key);
                $data  = array(
                    "key"   => $key,
                    "value" => $value,
                    "success" => ($value) ? true : false,
                );
            } else {
                $data = array(
                    "success" => false,
                    "exists"  => $exists,
                );
            }
            return $response->withJson($data);
        });

        $app->get('/set/{key}/{value}', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/nvram/set to: ' . $_SERVER['REMOTE_ADDR']);

            $key    = $args['key'];
            $value  = $args['value'];

            if (NVRAM::Exists($key)) {
                $data = array(
                    "success" => false,
                    "exists"  => true,
                );
            } else {
                $data = array(
                    "success" => NVRAM::Set($key, $value),
                    "key"     => $key,
                    "value"   => $value,
                );
            }
            return $response->withJson($data);
        });

        $app->get('/unset/{key}', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/nvram/unset to: ' . $_SERVER['REMOTE_ADDR']);

            $key = $args['key'];

            if (!NVRAM::Exists($key)) {
                $data = array(
                    "success" => false
                );
            } else {
                $data = array(
                    "success" => NVRAM::Unset_Key($key));
            }
            return $response->withJson($data);
        });

        $app->get('/overwrite/{key}/{value}', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/nvram/overwrite to: ' . $_SERVER['REMOTE_ADDR']);

            $key    = $args['key'];
            $value  = $args['value'];

            if (!NVRAM::Exists($key)) {
                $success = false;
            } else {
                $success = NVRAM::Set($key, $value);
            }

            $data = array(
                "success" => $success,
                "key"     => $key,
                "value"   => $value,
            );
            return $response->withJson($data);
        });

    });


    // **************************************************************
    // ** System                                                   **
    // **************************************************************
    $app->group('/system', function () use ($app) {

        $app->get('/proclist', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/system/proclist to: ' . $_SERVER['REMOTE_ADDR']);

            $proclist = OS::Proclist();
            $success  = ($proclist) ? true : false;
            $data     = array(
                "output"  => $proclist,
                "success" => $success,
            );
            return $response->withJson($data);
        });

        $app->get('/dmesg', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/system/dmesg to: ' . $_SERVER['REMOTE_ADDR']);

            $dmesg    = OS::Dmesg();
            $success  = ($dmesg) ? true : false;
            $data     = array(
                "output"  => $dmesg,
                "success" => $success,
            );

            return $response->withJson($data);
        });

        $app->get('/diskusage', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/system/diskusage to: ' . $_SERVER['REMOTE_ADDR']);

            $diskusage = OS::DiskUsage();
            $success   = ($diskusage) ? true : false;
            $data      = array(
                "output"  => $diskusage,
                "success" => $success,
            );

            return $response->withJson($data);
        });

        $app->get('/uptime', function ($request, $response) {
            $this->logger->addInfo('Serving route for /api/system/uptime to: ' . $_SERVER['REMOTE_ADDR']);

            $uptime   = OS::Uptime();
            $success  = ($uptime) ? true : false;
            $data     = array(
                "output"  => $uptime,
                "success" => $success,
            );

            return $response->withJson($data);
        });

        $app->get('/load', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/system/load to: ' . $_SERVER['REMOTE_ADDR']);

            $load    = OS::Load();
            $success = ($load) ? true : false;
            $data    = array(
                "output"  => $load,
                "success" => $success,
            );
            return $response->withJson($data);
        });

        $app->get('/reboot', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/system/reboot to: ' . $_SERVER['REMOTE_ADDR']);

            $reboot   = OS::Reboot();
            $success  = ($reboot) ? true : false;
            $data     = array(
                "output"  => $reboot,
                "success" => $success,
            );
            return $response->withJson($data);
        });

        $app->get('/shutdown', function ($request, $response, $args) {
            $this->logger->addInfo('Serving route for /api/system/shutdown to: ' . $_SERVER['REMOTE_ADDR']);

            $shutdown = OS::Shutdown();
            $success  = ($shutdown) ? true : false;
            $data = array(
                "output"  => $uptime,
                "success" => $success,
            );
            return $response->withJson($data);
        });

    });


});
