<?php

// **************************************************************
// ** GPIO Functions                                           **
// **************************************************************

class GPIO
{
    public function __construct() {}

    public function Get(int $pin) {
        $command = sprintf("cat '/sys/class/gpio/gpio%d/value' 2>&1", $pin);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error retrieving GPIO value: %s', implode(" ", $output))
            );
        }

        return trim(implode(" ", $output));
    }

    public function Set(int $pin, int $value) {

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

    public function Get(string $key) {
        $command = sprintf('%s get %s 2>&1', self::$nvram, escapeshellarg($key));

        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error retrieving NVRAM value for key %s: %s', $key, implode(" ", $output))
            );
        }

        return trim(implode("", $output));
    }

    public function Set(string $key, string $value) {
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

    public function Exists(string $key) {
        $data = self::Show();

        if ($data) {
            return array_key_exists($key, $data);
        } else {
            return false;
        }
    }

    public function Show() {
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

    public function Unset_Key(string $key) {
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
        return self::RunCommand("/sbin/reboot -d 5 2>&1");
    }

    public static function Shutdown() {
        return self::RunCommand("/sbin/poweroff -d 5 2>&1");
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
        return self::RunCommand('echo x > /proc/frammap/free_pages 2>&1');
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
