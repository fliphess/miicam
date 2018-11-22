<?php

// **************************************************************
// **                                            **
// **************************************************************


class Services
{
    // * A listing of all available services
    // ... And whether they can be started/stopped over http
    //
    public static $all_services = array(
        "auto_night_mode" => true,
        "crond"           => true,
        "dropbear"        => true,
        "ftpd"            => true,
        "klogd"           => false,  // Kernel logging can't be stopped for debugging purposes
        "lighttpd"        => false,  // Let's not stop the webserver and paint ourselves in the corner
        "messagebus"      => false,  // Used by isp328
        "mqtt-control"    => true,
        "mqtt-interval"   => true,
        "ntpd"            => false,
        "restartd"        => true,
        "rtsp"            => true,
        "samba"           => true,
        "syslogd"         => false,  // Syslogging can't be stopped for now for debugging reasons
        "telnet"          => true,
        "udhcpc"          => false,
        "watchdog"        => false,
        "wpa_supplicant"  => false,
    );

    public static $all_actions = [
        "start",
        "stop",
        "restart",
        "status",
    ];

    public function __construct() {}

    public static function IsRunning(string $name) {
        $filename = sprintf('/var/run/%s.pid', $name);

        if (is_file($filename)) {
            $pid = @file_get_contents($filename);
            $command = sprintf("/usr/bin/pstree -p %s 2>&1", $pid);
        } else {
            $command = sprintf("/usr/bin/pgrep %s 2>&1", $name);
        }

        exec($command, $output, $return);

        if (!empty($output) && ($return == 0)) {
            return true;
        } else {
            return false;
        }
    }

    public function Run(string $service, string $action) {

        if (!in_array($action, self::$all_actions)) {
            throw new \Exception(
                sprintf('Invalid input: Action %s not known.', $action)
            );
        }

        if (in_array($service, self::$all_services) && (self::$all_services[$service] !== true)) {
            throw new \Exception(
                sprintf('Invalid input: Service %s cannot be managed over http.', $service)
            );
        }

        // * Only use S99 services that have their own init scripts in /etc/init
        $init_script = sprintf('/etc/init/S99%s', $service);

        if (!is_file($init_script) || !is_executable($init_script)) {
            throw new \Exception(
                sprintf('Invalid init script: Init script %s not found or not executable!', $init_script)
            );
        }

        $command = sprintf('%s %s', $init_script, $action);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        return trim(implode(" ", $output));
    }

    public static function Start(string $service) {
        if (self::IsRunning($service)) {
            throw new \Exception(
                sprintf('Error: service %s is already running!', $service)
            );
        }
        return self::Run($service, 'start');
    }

    public static function Stop(string $service) {
        if (!self::IsRunning($service)) {
            throw new \Exception(
                sprintf('Error: service %s not running!', $service)
            );
        }
        return self::Run($service, 'stop');
    }

    public function Restart(string $service) {
        if (!self::IsRunning($service)) {
            throw new \Exception(
                sprintf('Error: service %s not running!', $service)
            );
        }
        return self::Run($service, 'restart');
    }

    public static function Status(string $service) {
        return self::Run($service, 'status');
    }

    public static function ServicesState() {
        $services = [];

        foreach (self::$all_services as $service => $managed) {
            $services[] = array(
                "name"    => $service,
                "running" => self::IsRunning($service),
                "managed" => $managed,
             );
        }
        return $services;
    }
}

?>
