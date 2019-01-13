<?php


// **************************************************************
// ** Config Manipulation                                      **
// **************************************************************

class Configuration
{
    public function __construct() {}

    public static $config_path  = '/tmp/sd/config.cfg';

    public static $backupdir = '/tmp/sd/config.bak';

    public static $required_config = [
        "AUTO_NIGHT_MODE",
        "DISABLE_CLOUD",
        "DISABLE_HACK",
        "DISABLE_OTA",
        "ENABLE_CRON",
        "ENABLE_FTPD",
        "ENABLE_HTTPD",
        "ENABLE_MQTT",
        "ENABLE_RESTARTD",
        "ENABLE_RTSP",
        "ENABLE_SSHD",
        "ENABLE_TELNETD",
        "HOSTNAME",
        "MOSQUITTOOPTS",
        "MOSQUITTOPUBOPTS",
        "MQTT_HOST",
        "MQTT_PASS",
        "MQTT_PORT",
        "MQTT_STATUSINTERVAL",
        "MQTT_TOPIC",
        "MQTT_USER",
        "NTP_SERVER",
        "PING_IP",
        "PING_RETRIES",
        "PING_WAIT",
        "PURGE_LOGFILES_AT_BOOT",
        "ROOT_PASSWORD",
        "TIMEZONE",
        "WAIT_FOR_NETWORK",
        "WIFI_PASS",
        "WIFI_SSID",
    ];

    public static function Read() {
        // * Read the configuration and return as a single-key array

        $content = file_get_contents(self::$config_path);

        if (!$content) {
            throw new \Exception(
                sprintf('Error reading configuration file %s', self::$config_path)
            );
        }

        return $content;
    }

    public static function Write($content) {
        // * Save the content to the configuration file

        self::Verify($content);
        self::Backup();

        if (!file_put_contents(self::$config_path, $content, LOCK_EX)) {
            throw new \Exception(
                sprintf('Failed to write new content to %s configuration content: %s (line: %s)', $line, $lines_count)
            );
        }
        return true;
    }

    public static function Verify($content) {
        $lines = explode("\n", $content);
        $lines_count = 0;

        foreach ($lines as $line) {
            $lines_count++;
            if (!preg_match('/^#.+$|^$/', $line) and !preg_match('/^\w+=\"?([a-zA-Z0-9_\-\ \/\$\.]+)?\"?$/', $line)) {
                throw new \Exception(
                    sprintf('Invalid input in configuration content: %s (line: %s)', $line, $lines_count)
                );
            }
        }

        foreach (self::$required_config as $element) {
            $regex = sprintf('/%s=/', $element);
            if (!preg_match($regex, $content)) {
                throw new \Exception(
                    sprintf('Invalid configuration: %s not found in content!', $element)
                );
            }
        }

        return true;
    }

    public static function Test() {
        // * Verify the syntax of the current config file on the command line

        $command = sprintf('/bin/busybox ash -n %s 2>&1 && echo "Syntax OK"', self::$config_path);
        exec($command, $output, $return);
        return array("success" => ($return == 0) ? true : false, "message" => ($output) ? implode(" ", $output) : "No syntax errors detected in configuration file");
    }

    public static function ListBackups() {
        // * Return a list of all available backup files in backup dir

        if (!is_dir(self::$backupdir)) {
            return [];
        }

        $files = glob(sprintf("%s/config_*", self::$backupdir));

        $file_list = [];
        foreach ($files as $file) {
            array_push($file_list, basename($file));
        }
        return $file_list;
    }

    public static function Backup() {
        // * Backup the current configuration to backupdir
        $timestamp = strftime("%Y%m%d_%H%M%S");

        if (!is_dir(self::$backupdir))
            OS::CreateDir(self::$backupdir);

        $filename = sprintf('%s/config_%s', self::$backupdir, $timestamp);

        if (!copy(self::$config_path, $filename)) {
            throw new \Exception(
                sprintf('Failed to copy %s to %s', self::$config_path, $filename)
            );
        }

        return sprintf('config_%s', $timestamp);
    }

    public static function RemoveBackups() {
        // * Leave N backups and cleanup the rest

        $command = sprintf("rm -rf %s 2>&1", escapeshellarg(self::$backupdir));
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error removing directory %s: %s', self::$backupdir, implode(" ", $output))
            );
        }
        return true;
    }

    public static function BackupRestore($filename) {
        // * Restore a given backup file as the current config
        $sourcefile = sprintf('%s/%s', self::$backupdir, $filename);

        if (!is_file($sourcefile)) {
            throw new \Exception(
                sprintf('Failed to restore %s to %s: File not found!', $sourcefile, self::$config_path)
            );
        }

        self::Backup();

        if (!copy($sourcefile, self::$config_path)) {
            throw new \Exception(
                sprintf('Failed to restore %s to %s: Copy Failed', $sourcefile, self::$config_path)
            );
        }

        return sprintf("Config %s restored to %s", $sourcefile, self::$config_path);
    }
}


// **************************************************************
// ** GPIO Functions                                           **
// **************************************************************

class GPIO
{
    public function __construct() {}

    public static function Get(int $pin) {
        $command = sprintf("cat '/sys/class/gpio/gpio%d/value' 2>&1", $pin);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error retrieving GPIO value: %s', implode(" ", $output))
            );
        }

        return trim(implode(" ", $output));
    }

    public static function Set(int $pin, int $value) {

        $current = self::Get($pin);

        if ($current == $value) return true;

        $command = sprintf("echo %d > '/sys/class/gpio/gpio%d/value' 2>&1", $value, $pin);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error setting GPIO value for pin %d: %s', $pin, implode(" ", $output))
            );
        }

        return true;
    }
}


// **************************************************************
// ** NVRAM Functions                                          **
// **************************************************************

class NVRAM
{
    public static $nvram = '/usr/sbin/nvram';

    public function __construct() {}

    public static function Get(string $key) {
        $command = sprintf('%s get %s 2>&1', self::$nvram, escapeshellarg($key));

        exec($command, $output, $return);

        if ($return != 0) {
            $message = sprintf("Error retrieving NVRAM value for key %s: ", $key);
            $message .= (!empty($output)) ? trim(implode(" ", $output)) : "Not found";
            throw new \Exception($message);
        }

        return trim(implode("", $output));
    }

    public static function Set(string $key, string $value) {
        $current = self::Get($key);

        if ($current == $value) return true;

        $command = sprintf('%s set %s=%s 2>&1', self::$nvram, escapeshellarg($key), escapeshellarg($value));
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error setting NVRAM value for key %s: %s', $key, implode(" ", $output))
            );
        }

        $command = sprintf('%s commit 2>&1', self::$nvram);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error committing NVRAM changes: %s', implode(" ", $output))
            );
        }

        return true;
    }

    public static function Exists(string $key) {
        $data = self::Show();

        if ($data) {
            return array_key_exists($key, $data);
        } else {
            return false;
        }
    }

    public static function Show() {
        $command = sprintf("%s show 2>&1", self::$nvram);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error retrieving NVRAM show data: %s', implode(" ", $output))
            );
        }

        $data = array();
        foreach ($output as $line) {
            list($key, $value) = explode("=", $line);
            $data[$key] = trim($value);
        }
        return $data;
    }

    public static function Unset_Key(string $key) {
        if (!self::Exists($key)) return false;

        $command = sprintf("%s unset %s 2>&1", self::$nvram, escapeshellarg($key));
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error unsetting NVRAM key %s: %s', $key, implode(" ", $output))
            );
        }

        $command = sprintf('%s commit 2>&1', self::$nvram);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error committing NVRAM changes: %s', implode(" ", $output))
            );
        }

        return true;
    }
}


// **************************************************************
// ** OS System functions                                      **
// **************************************************************

class OS
{
    public static function RunCommand($command) {
        exec(sprintf('%s 2>&1', $command), $output, $return);

        if ($return != 0) {
            throw new \Exception(sprintf(
                'Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        return $output;
    }

    public static function Reboot() {
        return self::RunCommand("/sbin/reboot -d 5 2>&1 && echo 'Scheduled REBOOT'");
    }

    public static function Shutdown() {
        return self::RunCommand("/sbin/poweroff -d 5 2>&1 && echo 'Scheduled SHUTDOWN'");
    }

    public static function Uptime() {
        if (is_executable('/bin/uptime')) {
            $uptimecmd = '/bin/uptime 2>&1';
        } else {
            $uptimecmd = '/usr/bin/uptime 2>&1';
        }
        return self::RunCommand($uptimecmd);
    }

    public static function Dmesg() {
        return self::RunCommand("/bin/dmesg 2>&1");
    }

    public static function Proclist() {
        return self::RunCommand("/bin/ps -ef 2>&1");
    }

    public static function DiskUsage() {
        return self::RunCommand("/bin/df -h 2>&1");
    }

    public static function CpuInfo() {
        return self::RunCommand("cat /proc/cpuinfo 2>&1");
    }

    public static function MemInfo() {
        return self::RunCommand("cat /proc/meminfo 2>&1");
    }

    public static function MemFree() {
        return self::RunCommand("/usr/bin/free -m 2>&1");
    }

    public static function IfConfig() {
        return self::RunCommand("/sbin/ifconfig -a 2>&1");
    }

    public static function IwConfig() {
        return self::RunCommand("/sbin/iwconfig 2>&1");
    }

    public static function FreeMemory() {
        return self::RunCommand('echo 1 > /proc/frammap/free_pages 2>&1 && echo OK');
    }

    public function CreateDir($path) {
        if (is_dir($path))
            return true;

        if (!mkdir($path, 0750, true)) {
            throw new \Exception(
                'Failed to create: ' . $directory
            );
        }
        return true;
    }
}

?>
