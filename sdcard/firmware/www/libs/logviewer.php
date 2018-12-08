<?php

class LogTailer
{
    public function __construct() { }

    static $file_mapping = array(
        "syslog"   => '/tmp/sd/log/syslog',
        "webapp"   => '/tmp/sd/log/webapp.log',
        "bootlog"  => '/tmp/sd/log/ft_boot.log',
        "lighttpd" => '/tmp/sd/log/lighttpd.log',
    );

    static function Get($log, $seek) {

        if (!getenv('ENABLE_LOGGING') == 1) {
            throw new \Exception(
                'Logging is disabled in config.cfg. Set ENABLE_LOGGING to 1'
            );
        }

        if (!array_key_exists($log, self::$file_mapping)) {
            throw new \Exception(
                sprintf('Log type not known: %s', $log)
            );
        }

        $logfile = self::$file_mapping[$log];

        if (!is_file($logfile) ) {
            throw new \Exception(
                sprintf('Log file not found: %s', $logfile)
            );
        }

        $lines  = [];
        $handle = fopen($logfile, 'rb');

        if ($seek > 0) {
            fseek($handle, $seek);
        }

        while (($line = fgets($handle, 4096)) !== false) {
            $lines[] = $line;
        }

        $seek = ftell($handle);

        return array('seek' => $seek, 'lines' => $lines);
    }
}


