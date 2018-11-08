<?php

use Slim\Http\Request;
use Slim\Http\Response;


// **************************************************************
// ** Info page                                                **
// **************************************************************

$app->get('/info', function ($request, $response, $args) {
    return phpinfo();
})->setName('/info');



// **************************************************************
// ** Logviewer Ajax endpoint                                  **
// **************************************************************

$app->get('/logviewer/{log}/refresh', function ($request, $response, $args) {
    $log  = $args['log'];
    $seek = $request->getQueryParam('seek');
    $data = LogTailer::Get($log, $seek);
    return $response->withJson($data);
})->setName('/logviewer/refresh');



// **************************************************************
// ** Ping                                                     **
// **************************************************************

$app->get('/ping', function ($request, $response) {
    return $response->withJson(array("success" => true, "data" => "pong"), 200);
})->setName('/api/ping');



// **************************************************************
// ** Snapshot                                                 **
// **************************************************************

$app->get('/snapshot/create', function ($request,$response) {
    $output = Snapshot::Create();
    if ($output) {
        $data = array(
            "success"  => true,
            "output"   => $output,
            "filename" => Snapshot::$snapshot_destination,
            "url"      => '/snapshot.jpg',
        );
    } else {
        $data = array("success" => false);
    }
    return $response->withJson($data);
})->setName('/snapshot/create');

$app->get('/snapshot/save/new', function ($request, $response) {
    $filename = Snapshot::SaveNew();

    if (($filename) && is_file($filename)) {
        $data = array(
            "success"  => true,
            "filename" => $filename,
        );
    } else {
        $data = array("success" => false);
    }
    return $response->withJson($data);

})->setName('/snapshot/save/new');

$app->get('/snapshot/save/current', function ($request, $response) {
    $filename = Snapshot::SaveCurrent();

    if (($filename) && is_file($filename)) {
        $data = array(
            "success"  => true,
            "filename" => $filename,
        );
    } else {
        $data = array("success" => false);
    }
    return $response->withJson($data);
})->setName('/snapshot/save/current');



// **************************************************************
// ** WIFI                                                     **
// **************************************************************

$app->get('/wifi', function ($request,$response) {
    $wifistate = WIFI::DataDict();
    $data = ($wifistate) ? $wifistate : array("success" => false, "output" => "Failed to parse iwconfig output");
    return $response->withJson($data);
})->setName('/wifi');

$app->get('/wifi/ifconfig', function ($request,$response) {
    $state = WIFI::IfConfig();
    $data = ($state) ? $state : array("success" => false, "output" => "Failed to parse ifconfig output");
    return $response->withJson($data);
})->setName('/wifi/ifconfig');



// **************************************************************
// ** LEDS                                                     **
// **************************************************************

$app->group('/led', function () use ($app) {

    $app->get('/state', function ($request,$response) {
        $ledstate = LED::LedState();
        $data = ($ledstate) ? $ledstate : array("success" => false);
        return $response->withJson($data);
    })->setName('/led/state');

    // *********************************
    // ** Blue LED                    **
    // *********************************
    $app->get('/blue', function ($request, $response) {
        $data = array(
            "led"   => "blue_led",
            "state" => (Blue_Led::IsOn()) ? "on" : "off",
        );
        return $response->withJson($data);
    })->setName('/led/blue');

    $app->get('/blue/set/on', function ($request, $response) {
        Blue_Led::TurnOn();

        $data    = array(
            "led"     => "blue_led",
            "action"  => "TurnOn",
            "success" => (Blue_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/blue/set/on');

    $app->get('/blue/set/blink', function ($request, $response) {
       Blue_Led::TurnBlink();

        $data = array(
            "led"     => "blue_led",
            "action"  => "TurnBlink",
            "success" => (Blue_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/blue/set/blink');

    $app->get('/blue/set/off', function ($request, $response) {
        Blue_Led::TurnOff();

        $data = array(
            "led"     => "blue_led",
            "action"  => "TurnOff",
            "success" => (Blue_Led::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/blue/set/off');

    // *********************************
    // ** Yellow LED                  **
    // *********************************
    $app->get('/yellow', function ($request, $response) {
        $data = array(
            "led"   => "yellow_led",
            "state" => (Yellow_Led::IsOn()) ? "on" : "off",
        );
        return $response->withJson($data);
    })->setName('/led/yellow');

    $app->get('/yellow/set/on', function ($request, $response) {
        Yellow_Led::TurnOn();
        $data = array(
            "led"     => "yellow_led",
            "action"  => "TurnOn",
            "success" => (Yellow_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/yellow/set/on');

    $app->get('/yellow/set/blink', function ($request, $response) {
        Yellow_Led::TurnBlink();
        $data    = array(
            "led"     => "yellow_led",
            "action"  => "TurnBlink",
            "success" => (Yellow_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/yellow/set/blink');

    $app->get('/yellow/set/off', function ($request, $response) {
        Yellow_Led::TurnOff();
        $data    = array(
            "led"     => "yellow_led",
            "action"  => "TurnOff",
            "success" => (Yellow_Led::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/yellow/set/off');

    // *********************************
    // ** IR LED                      **
    // *********************************
    $app->get('/ir', function ($request, $response) {
        $data = array(
            "led" => "ir_led",
            "state" => (IR_Led::IsOn()) ? "on" : "off",
        );
        return $response->withJson($data);
    })->setName('/led/ir');

    $app->get('/ir/set/on', function ($request, $response) {
        IR_Led::TurnOn();

        $data = array(
            "led"     => "ir_led",
            "action"  => "TurnOn",
            "success" => (IR_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/ir/set/on');

    $app->get('/ir/set/off', function ($request, $response) {
        IR_Led::TurnOff();

        $data = array(
            "led"     => "ir_led",
            "action"  => "TurnOff",
            "success" => (IR_Led::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/ir/set/off');
});



// **************************************************************
// ** Camera                                                   **
// **************************************************************

$app->group('/camera', function () use ($app) {

    $app->get('/state', function ($request,$response) {
        $camerastate = CameraState();
        $data = ($camerastate) ? $camerastate : array("success" => false);
        return $response->withJson($data);
    })->setName('/camera');

    // *********************************
    // ** ISP328                      **
    // *********************************
    $app->get('/isp328', function ($request, $response) {
        $data = array(
            "keys"   => ISP328::$all_isp_keys,
        );
        return $response->withJson($data);
    })->setName('/camera/isp328');

    $app->get('/isp328/get/{key}', function ($request, $response, $args) {
        $key   = $args['key'];
        $value = ISP328::Get($key);

        $data = array(
            "key"     => $key,
            "value"   => $value,
            "success" => ($value) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/isp328/get');

    $app->get('/isp328/set/{key}/{value}', function ($request, $response, $args) {
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
    })->setName('/camera/isp328/set');

    // *********************************
    // ** IR Cut                      **
    // *********************************
    $app->get('/ir_cut', function ($request, $response, $args) {
        $data = array(
             "key" => "ir_cut",
             "state" => IR_Cut::IsOn() ? "on" : "off",
         );
         return $response->withJson($data);
    })->setName('/camera/ir_cut');

    $app->get('/ir_cut/set/on', function ($request, $response, $args) {
        IR_Cut::TurnOn();

        $data = array(
            "key"     => "ir_cut",
            "action"  => "TurnOn",
            "success" => (IR_Cut::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/ir_cut/set/on');

    $app->get('/ir_cut/set/off', function ($request, $response, $args) {
        IR_Cut::TurnOff();
        $data = array(
            "led"     => "ir_cut",
            "action"  => "TurnOff",
            "success" => (IR_Cut::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/ir_cut/set/off');

    // *********************************
    // ** Night Mode                  **
    // *********************************
    $app->get('/night_mode', function ($request, $response, $args) {
        $data = array(
             "key" => "night_mode",
             "state" => NightMode::IsOn() ? "on" : "off",
         );
         return $response->withJson($data);
    })->setName('/camera/night_mode');

    $app->get('/night_mode/set/on', function ($request, $response, $args) {
        NightMode::TurnOn();

        $data = array(
            "key"     => "night_mode",
            "action"  => "TurnOn",
            "success" => (NightMode::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/night_mode/set/on');

    $app->get('/night_mode/set/off', function ($request, $response, $args) {
        NightMode::TurnOff();
        $data = array(
            "led"     => "night_mode",
            "action"  => "TurnOff",
            "success" => (NightMode::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/night_mode/set/off');

    // *********************************
    // ** Flip Mode                   **
    // *********************************
    $app->get('/flip_mode', function ($request, $response, $args) {
        $data = array(
             "key" => "flip_mode",
             "state" => FlipMode::IsOn() ? "on" : "off",
         );
         return $response->withJson($data);
    })->setName('/camera/flip_mode');

    $app->get('/flip_mode/set/on', function ($request, $response, $args) {
        FlipMode::TurnOn();
        $data = array(
            "key"     => "flip_mode",
            "action"  => "TurnOn",
            "success" => (FlipMode::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/flip_mode/set/on');

    $app->get('/flip_mode/set/off', function ($request, $response, $args) {
        FlipMode::TurnOff();
        $data = array(
            "led"     => "flip_mode",
            "action"  => "TurnOff",
            "success" => (FlipMode::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/flip_mode/set/off');

    // *********************************
    // ** Mirror Mode                 **
    // *********************************
    $app->get('/mirror_mode', function ($request, $response, $args) {
        $data = array(
             "key" => "mirror_mode",
             "state" => MirrorMode::IsOn() ? "on" : "off",
         );
         return $response->withJson($data);
    })->setName('/camera/mirror_mode');

    $app->get('/mirror_mode/set/on', function ($request, $response, $args) {
        MirrorMode::TurnOn();
        $data = array(
            "key"     => "mirror_mode",
            "action"  => "TurnOn",
            "success" => (MirrorMode::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/mirror_mode/set/on');

    $app->get('/mirror_mode/set/off', function ($request, $response, $args) {
        MirrorMode::TurnOff();
        $data = array(
            "led"     => "mirror_mode",
            "action"  => "TurnOff",
            "success" => (MirrorMode::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/mirror_mode/set/off');
});



// **************************************************************
// ** NVRAM                                                    **
// **************************************************************

$app->group('/nvram', function () use ($app) {

    $app->get('/state', function ($request,$response) {
        $nvram = NVRAM::Show();
        $data  = ($nvram) ? $nvram : array("success" => false);
        return $response->withJson($data);
    })->setName('/nram');

    $app->get('/get/{key}', function ($request, $response, $args) {
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
    })->setName('/nvram/get');

    $app->get('/set/{key}/{value}', function ($request, $response, $args) {
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
    })->setName('/nvram/set');

    $app->get('/unset/{key}', function ($request, $response, $args) {
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
    })->setName('/nvram/unset');

    $app->get('/overwrite/{key}/{value}', function ($request, $response, $args) {
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
    })->setName('/nvram/overwrite');

});



// **************************************************************
// ** System                                                   **
// **************************************************************

$app->group('/system', function () use ($app) {

    $app->get('/proclist', function ($request, $response) {
        $proclist = OS::Proclist();
        $success  = ($proclist) ? true : false;
        $data     = array(
            "output"  => $proclist,
            "success" => $success,
        );
        return $response->withJson($data);
    });

    $app->get('/dmesg', function ($request, $response) {
        $dmesg    = OS::Dmesg();
        $success  = ($dmesg) ? true : false;
        $data     = array(
            "output"  => $dmesg,
            "success" => $success,
        );

        return $response->withJson($data);
    })->setName('/system/dmesg');

    $app->get('/diskusage', function ($request, $response) {
        $diskusage = OS::DiskUsage();
        $success   = ($diskusage) ? true : false;
        $data      = array(
            "output"  => $diskusage,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/diskusage');

    $app->get('/uptime', function ($request, $response) {
        $uptime   = OS::Uptime();
        $success  = ($uptime) ? true : false;
        $data     = array(
            "output"  => $uptime,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/uptime');

    $app->get('/meminfo', function ($request, $response) {
        $meminfo   = OS::MemInfo();
        $success  = ($meminfo) ? true : false;
        $data     = array(
            "output"  => $meminfo,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/meminfo');

    $app->get('/memfree', function ($request, $response) {
        $memfree   = OS::MemFree();
        $success  = ($memfree) ? true : false;
        $data     = array(
            "output"  => $memfree,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/memfree');

    $app->get('/cpuinfo', function ($request, $response) {
        $cpuinfo   = OS::CpuInfo();
        $success  = ($cpuinfo) ? true : false;
        $data     = array(
            "output"  => $cpuinfo,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/cpuinfo');

    $app->get('/ifconfig', function ($request, $response) {
        $ifconfig   = OS::IfConfig();
        $success  = ($ifconfig) ? true : false;
        $data     = array(
            "output"  => $ifconfig,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/ifconfig');

    $app->get('/iwconfig', function ($request, $response) {
        $iwconfig   = OS::IwConfig();
        $success  = ($iwconfig) ? true : false;
        $data     = array(
            "output"  => $iwconfig,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/iwconfig');

    $app->get('/reboot', function ($request, $response, $args) {
        $reboot   = OS::Reboot();
        $success  = ($reboot) ? true : false;
        $data     = array(
            "output"  => $reboot,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/reboot');

    $app->get('/shutdown', function ($request, $response, $args) {
        $shutdown = OS::Shutdown();
        $success  = ($shutdown) ? true : false;
        $data = array(
            "output"  => $uptime,
            "success" => $success,
        );
        return $response->withJson($data);
    })->setName('/system/shutdown');

});



// **************************************************************
// ** Services                                                 **
// **************************************************************

$app->group('/services', function () use ($app) {
    $app->get('/state', function ($request, $response) {
        $data = Services::ServicesState();
        return $response->withJson($data);
    })->setName('/services');

    $app->get('/{service}/running', function ($request, $response, $args) {
        $service = $args['service'];
        $data    = array(
             "service" => $service,
             "state" => (Services::IsRunning($service)) ? "on" : "off",
         );
        return $response->withJson($data);
    })->setName('/service/running');

    $app->get('/{service}/status', function ($request, $response, $args) {
        $service = $args['service'];
        $output  = Services::Status($service);

        $data    = array(
            "service" => $service,
            "output"  => $output,
            "success" => ($output) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/service/status');

    $app->get('/{service}/start', function ($request, $response, $args) {
        $service = $args['service'];
        $output  = Services::Start($service);

        $data    = array(
            "service" => $service,
            "output"  => $output,
            "success" => ($output) ? true : false,
        );

        return $response->withJson($data);
    })->setName('/service/start');

    $app->get('/{service}/stop', function ($request, $response, $args) {
        $service = $args['service'];
        $output  = Services::Stop($service);

        $data    = array(
            "service" => $service,
            "output"  => $output,
            "success" => ($output) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/service/stop');

    $app->get('/{service}/restart', function ($request, $response, $args) {
        $service = $args['service'];
        $output  = Services::Restart($service);

        $data    = array(
            "service" => $service,
            "output"  => $output,
            "success" => ($output) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/service/restart');

});



// **************************************************************
// ** Last route (404)                                         **
// **************************************************************

$app->map(['GET', 'POST', 'PUT', 'DELETE', 'PATCH'], '/{routes:.+}', function($req, $res) {
    $handler = $this->notFoundHandler; // handle using the default Slim page not found handler
    return $handler($req, $res);
});


