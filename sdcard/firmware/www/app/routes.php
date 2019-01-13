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

$app->get('/snapshot/create', function ($request, $response) {
    $filename = Snapshot::Create();

    if ($filename and is_file($filename)) {
        $data = array(
            "success"  => true,
            "output"   => "Snapshot created",
            "filename" => $filename,
            "url"      => Snapshot::GetUrl($filename),
        );
    } else {
        $data = array("success" => false, "message" => "Failed to request snapshot");
    }
    return $response->withJson($data);
})->setName('/snapshot/create');

$app->get('/snapshot/get_last', function ($request, $response) {
    $filename = Snapshot::GetLast();

    if (($filename) && is_file($filename)) {
        $data = array(
            "success"  => true,
            "filename" => $filename,
        );
    } else {
        $data = array("success" => false, "message" => "Failed to request snapshot path");
    }
    return $response->withJson($data);

})->setName('/snapshot/get_last');

$app->get('/snapshot/get_url', function ($request, $response) {
    $url = Snapshot::GetLastUrl();

    if ($url) {
        $data = array(
            "success" => true,
            "url"     => $url,
        );
    } else {
        $data = array("success" => false, "message" => "Failed to request last snapshot url");
    }
    return $response->withJson($data);
})->setName('/snapshot/get_url');


// **************************************************************
// ** Record                                                   **
// **************************************************************

$app->get('/record/create', function ($request, $response) {
    $filename = VideoRecording::Create();

    if ($filename and is_file($filename)) {
        $data = array(
            "success"  => true,
            "output"   => "VideoRecording created",
            "filename" => $filename,
            "url"      => VideoRecording::GetUrl($filename),
        );
    } else {
        $data = array("success" => false);
    }
    return $response->withJson($data);
})->setName('/record/create');

$app->get('/record/get_last', function ($request, $response) {
    $filename = VideoRecording::GetLast();

    if (($filename) && is_file($filename)) {
        $data = array(
            "success"  => true,
            "filename" => $filename,
        );
    } else {
        $data = array("success" => false, "message" => "Failed to request last video url");
    }
    return $response->withJson($data);

})->setName('/record/get_last');

$app->get('/record/get_url', function ($request, $response) {
    $url = VideoRecording::GetLastUrl();

    if ($url) {
        $data = array(
            "success" => true,
            "url"     => $url,
        );
    } else {
        $data = array("success" => false, "message" => "Failed to request last video url");
    }
    return $response->withJson($data);
})->setName('/record/get_url');


// **************************************************************
// ** WIFI                                                     **
// **************************************************************

$app->get('/wifi', function ($request, $response) {
    $wifistate = WIFI::DataDict();
    $data = ($wifistate) ? $wifistate : array("success" => false, "output" => "Failed to parse iwconfig output");
    return $response->withJson($data);
})->setName('/wifi');

$app->get('/wifi/ifconfig', function ($request, $response) {
    $state = WIFI::IfConfig();
    $data = ($state) ? $state : array("success" => false, "output" => "Failed to parse ifconfig output");
    return $response->withJson($data);
})->setName('/wifi/ifconfig');



// **************************************************************
// ** LEDS                                                     **
// **************************************************************

$app->group('/led', function () use ($app) {

    $app->get('/state', function ($request, $response) {
        $ledstate = LED::LedState();
        $data = ($ledstate) ? $ledstate : array("success" => false);
        return $response->withJson($data);
    })->setName('/led/state');

    // *********************************
    // ** Blue LED                    **
    // *********************************

    $app->get('/blue_led', function ($request, $response) {
        $data = array(
            "led"   => "blue_led",
            "state" => (Blue_Led::IsOn()) ? "on" : "off",
        );
        return $response->withJson($data);
    })->setName('/led/blue_led');

    $app->get('/blue_led/set/on', function ($request, $response) {
        Blue_Led::TurnOn();

        $data    = array(
            "led"     => "blue_led",
            "action"  => "TurnOn",
            "success" => (Blue_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/blue_led/on');

    $app->get('/blue_led/set/blink', function ($request, $response) {
       Blue_Led::TurnBlink();

        $data = array(
            "led"     => "blue_led",
            "action"  => "TurnBlink",
            "success" => (Blue_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/blue_led/blink');

    $app->get('/blue_led/set/off', function ($request, $response) {
        Blue_Led::TurnOff();

        $data = array(
            "led"     => "blue_led",
            "action"  => "TurnOff",
            "success" => (Blue_Led::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/blue_led/off');


    // *********************************
    // ** Yellow LED                  **
    // *********************************

    $app->get('/yellow_led', function ($request, $response) {
        $data = array(
            "led"   => "yellow_led",
            "state" => (Yellow_Led::IsOn()) ? "on" : "off",
        );
        return $response->withJson($data);
    })->setName('/led/yellow_led');

    $app->get('/yellow_led/set/on', function ($request, $response) {
        Yellow_Led::TurnOn();
        $data = array(
            "led"     => "yellow_led",
            "action"  => "TurnOn",
            "success" => (Yellow_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/yellow_led/on');

    $app->get('/yellow_led/set/blink', function ($request, $response) {
        Yellow_Led::TurnBlink();
        $data    = array(
            "led"     => "yellow_led",
            "action"  => "TurnBlink",
            "success" => (Yellow_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/yellow_led/blink');

    $app->get('/yellow_led/set/off', function ($request, $response) {
        Yellow_Led::TurnOff();
        $data    = array(
            "led"     => "yellow_led",
            "action"  => "TurnOff",
            "success" => (Yellow_Led::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/yellow_led/off');


    // *********************************
    // ** IR LED                      **
    // *********************************

    $app->get('/ir_led', function ($request, $response) {
        $data = array(
            "led" => "ir_led",
            "state" => (IR_Led::IsOn()) ? "on" : "off",
        );
        return $response->withJson($data);
    })->setName('/led/ir_led');

    $app->get('/ir_led/set/on', function ($request, $response) {
        IR_Led::TurnOn();

        $data = array(
            "led"     => "ir_led",
            "action"  => "TurnOn",
            "success" => (IR_Led::IsOn()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/ir_led/on');

    $app->get('/ir_led/set/off', function ($request, $response) {
        IR_Led::TurnOff();

        $data = array(
            "led"     => "ir_led",
            "action"  => "TurnOff",
            "success" => (IR_Led::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/led/ir_led/off');

});



// **************************************************************
// ** Camera                                                   **
// **************************************************************

$app->group('/camera', function () use ($app) {

    $app->get('/state', function ($request, $response) {
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
    })->setName('/camera/isp328');

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
    })->setName('/camera/ir_cut/on');

    $app->get('/ir_cut/set/off', function ($request, $response, $args) {
        IR_Cut::TurnOff();
        $data = array(
            "led"     => "ir_cut",
            "action"  => "TurnOff",
            "success" => (IR_Cut::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/ir_cut/off');

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
    })->setName('/camera/night_mode/on');

    $app->get('/night_mode/set/off', function ($request, $response, $args) {
        NightMode::TurnOff();
        $data = array(
            "led"     => "night_mode",
            "action"  => "TurnOff",
            "success" => (NightMode::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/night_mode/off');

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
    })->setName('/camera/flip_mode/on');

    $app->get('/flip_mode/set/off', function ($request, $response, $args) {
        FlipMode::TurnOff();
        $data = array(
            "led"     => "flip_mode",
            "action"  => "TurnOff",
            "success" => (FlipMode::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/flip_mode/off');

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
    })->setName('/camera/mirror_mode/on');

    $app->get('/mirror_mode/set/off', function ($request, $response, $args) {
        MirrorMode::TurnOff();
        $data = array(
            "led"     => "mirror_mode",
            "action"  => "TurnOff",
            "success" => (MirrorMode::IsOff()) ? true : false,
        );
        return $response->withJson($data);
    })->setName('/camera/mirror_mode/off');
});


// **************************************************************
// ** Config                                                   **
// **************************************************************

$app->get('/config/read', function ($request, $response) {
    $content = Configuration::Read();
    $success =  ($content) ? true : false;
    $data    = array("success" => $success, "content" => $content, "message" => "Succesfully read config file");
    return $response->withJson($data);
})->setName('/config/read');

$app->get('/config/write', function ($request, $response) {
    return $response->withJson(array("success" => false, "message" => "Use a POST request to update"));
})->setName('/config/write/get');

$app->post('/config/write', function ($request, $response) {
    $post    = $request->getParsedBody();
    $content = $post['content'];
    $success = Configuration::Write($content);
    $data    = array("success" => $success, "message" => "Config written succesfully");

    return $response->withJson($data);
})->setName('/config/write');

$app->get('/config/test', function ($request, $response) {
    $data = Configuration::Test();
    return $response->withJson($data);
})->setName('/config/test');

$app->get('/config/backup/list', function ($request, $response) {
    $data = Configuration::ListBackups();
    return $response->withJson($data);
})->setName('/config/backup/list');

$app->get('/config/backup/create', function ($request, $response) {
    $filename = Configuration::Backup();
    $data  = ($filename) ? array("message" => sprintf("New backup created: %s", $filename), "success" => true) : array("success" => false, "message" => "Backup creation failed");
    return $response->withJson($data);
})->setName('/config/backup/create');

$app->get('/config/backup/remove', function ($request, $response) {
    $success = Configuration::RemoveBackups();
    $data    = array("success" => true, "message" => "Backups deleted");
    return $response->withJson($data);
})->setName('/config/backup/remove');

$app->get('/config/restore/{filename}', function ($request, $response, $args) {
    $filename = $args['filename'];
    $message  = Configuration::BackupRestore($filename);
    $data     = array("message" => $message, "success" => true);
    return $response->withJson($data);
})->setName('/config/backup/restore');


// **************************************************************
// ** NVRAM                                                    **
// **************************************************************

$app->group('/nvram', function () use ($app) {

    $app->get('/state', function ($request, $response) {
        $nvram = NVRAM::Show();
        $data  = ($nvram) ? $nvram : array("success" => false);
        return $response->withJson($data);
    })->setName('/nvram');

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


