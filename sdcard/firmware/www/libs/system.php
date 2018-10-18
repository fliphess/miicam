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

        $command = sprintf("echo %d > '/sys/class/gpio/gpio%d/value'", $value, $pin);
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
// ** WIFI Functions                                           **
// **************************************************************

/**
* Configuration settings of a wifi network interface.
*
* @package  Net_Wifi
* @author   Christian Weiske <cweiske@php.net>
* @license  http://www.gnu.org/copyleft/lesser.html  LGPL License 2.1
* @link     http://pear.php.net/package/Net_Wifi
*/

class Net_Wifi_Config
{
    var $activated = true;
    var $ap = null;
    var $associated = false;
    var $mode = null;
    var $nick = null;
    var $noise = null;
    var $packages_invalid_misc = 0;
    var $packages_missed_beacon = 0;
    var $packages_rx_invalid_crypt = null;
    var $packages_rx_invalid_frag = null;
    var $packages_rx_invalid_nwid = null;
    var $packages_tx_excessive_retries = null;
    var $power = null;
    var $protocol = null;
    var $rate = null;
    var $rssi = null;
    var $ssid = null;
    var $frequency = null;
    var $sensitivity = null;
    var $link_quality = null;
}


class Net_Wifi
{
    var $REG_ACCESS_POINT         = '/Access Point: ([0-9:A-F]{17})/';
    var $REG_FREQUENCY            = '/Frequency:([0-9.]+) GHz /';
    var $REG_SENSITIVITY          = '/Sensitivity:([0-9.\/]+)/';
    var $REG_LINK_QUALITY         = '/Link Quality[:=]([0-9.\/]+)/';

    var $REG_BIT_RATE             = '/Bit Rate[:=]([0-9.]+) [mk]b\\/s/i';
    var $REG_ESSID                = '/ESSID:"([^"]+)"/';
    var $REG_INVALID_MISC         = '/Invalid misc[:=](-?[0-9]+)/';
    var $REG_MISSED_BEACON        = '/Missed beacon[:=](-?[0-9]+)/';
    var $REG_NICKNAME             = '/Nickname:"([^"]+)"/';
    var $REG_NOISE_LEVEL          = '/Noise level[:=](-?[0-9]+) dBm/';
    var $REG_POWER                = '/Power[:=]([0-9]+) dBm/';
    var $REG_PROTOCOL_1           = '/IEEE ([0-9.]+[a-z])/';
    var $REG_PROTOCOL_2           = '/([0-9.]+[a-z])\s+linked\s+ESSID/';
    var $REG_RATES                = '|([0-9.]+) Mb/s|';
    var $REG_GROUP_CIPHER         = '|Group Cipher : (.*)|';
    var $REG_PAIRWISE_CIPHERS     = '|Pairwise Ciphers \([0-9]+\) : (.*)|';
    var $REG_AUTH_SUITES          = '|Authentication Suites \([0-9]+\) : (.*)|';
    var $REG_RX_INVALID_CRYPT     = '/Rx invalid crypt[:=](-?[0-9]+)/';
    var $REG_RX_INVALID_FRAG      = '/Rx invalid frag[:=](-?[0-9]+)/';
    var $REG_RX_INVALID_NWID      = '/Rx invalid nwid[:=](-?[0-9]+)/';
    var $REG_SIGNAL_LEVEL         = '/Signal level[:=](-?[0-9]+) dBm/';
    var $REG_TX_EXCESSIVE_RETRIES = '/Tx excessive retries[:=](-?[0-9]+)/';
    var $REG_WPA_IE_STRING        = 'WPA Version 1';
    var $REG_WPA2_IE_STRING       = 'IEEE 802.11i/WPA2 Version 1';

    function get(string $strInterface)
    {
        $arLines = array();
        $command = sprintf('/sbin/iwconfig %s 2>&1', escapeshellarg($strInterface));
        exec($command, $arLines, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error retrieving iwconfig output: %s', implode(" ", $arLines))
            );
        }

        $strAll = implode("\n", $arLines);

        return $this->parse($strAll);
    }


    private function parse(string $strAll)
    {
        $objConfig = new Net_Wifi_Config();

        $arMatches = array();
        if (preg_match($this->REG_ESSID, $strAll, $arMatches)) {
            $objConfig->ssid = $arMatches[1];
        }
        if (preg_match($this->REG_LINK_QUALITY, $strAll, $arMatches)) {
            $objConfig->link_quality = $arMatches[1];
        }
        if (preg_match($this->REG_FREQUENCY, $strAll, $arMatches)) {
            $objConfig->frequency = $arMatches[1];
        }
        if (preg_match($this->REG_SENSITIVITY, $strall, $armatches)) {
            $objconfig->sensitivity = $armatches[1];
        }
        if (preg_match($this->REG_ACCESS_POINT, $strAll, $arMatches)) {
            $objConfig->ap = $arMatches[1];
        }
        if (preg_match($this->REG_NICKNAME, $strAll, $arMatches)) {
            $objConfig->nick = $arMatches[1];
        }
        if (strpos($strAll, 'Mode:Managed')) {
            $objConfig->mode = 'managed';
        } else if (strpos($strAll, 'Mode:Ad-Hoc')) {
            $objConfig->mode = 'ad-hoc';
        }
        if (preg_match($this->REG_BIT_RATE, $strAll, $arMatches)) {
            $objConfig->rate = $arMatches[1];
        }
        if (preg_match($this->REG_POWER, $strAll, $arMatches)) {
            $objConfig->power = $arMatches[1];
        }
        if (preg_match($this->REG_SIGNAL_LEVEL, $strAll, $arMatches)) {
            $objConfig->rssi = $arMatches[1];
        }
        if (preg_match($this->REG_NOISE_LEVEL, $strAll, $arMatches)) {
            $objConfig->noise = $arMatches[1];
        }
        if (preg_match($this->REG_PROTOCOL_1, $strAll, $arMatches)) {
            $objConfig->protocol = $arMatches[1];
        } elseif (preg_match($this->REG_PROTOCOL_2, $strAll, $arMatches)) {
            $objConfig->protocol = $arMatches[1];
        }
        if (preg_match($this->REG_RX_INVALID_NWID, $strAll, $arMatches)) {
            $objConfig->packages_rx_invalid_nwid = $arMatches[1];
        }
        if (preg_match($this->REG_RX_INVALID_CRYPT, $strAll, $arMatches)) {
            $objConfig->packages_rx_invalid_crypt = $arMatches[1];
        }
        if (preg_match($this->REG_RX_INVALID_FRAG, $strAll, $arMatches)) {
            $objConfig->packages_rx_invalid_frag = $arMatches[1];
        }
        if (preg_match($this->REG_TX_EXCESSIVE_RETRIES, $strAll, $arMatches)) {
            $objConfig->packages_tx_excessive_retries = $arMatches[1];
        }
        if (preg_match($this->REG_INVALID_MISC, $strAll, $arMatches)) {
            $objConfig->packages_invalid_misc = $arMatches[1];
        }
        if (preg_match($this->REG_MISSED_BEACON, $strAll, $arMatches)) {
            $objConfig->packages_missed_beacon = $arMatches[1];
        }

        if (strpos($strAll, 'unassociated') === false && $objConfig->ap != null && $objConfig->ap != '00:00:00:00:00:00') {
            $objConfig->associated = true;
        }

        return $objConfig;
    }

    function IfConfig() {
        // from http://www.highonphp.com/regex-pattern-parsing-ifconfig
        $interfaces = array();
        foreach (preg_split("/\n\n/", $data) as $int) {

            preg_match("/^([A-z]*\d)\s+Link\s+encap:([A-z]*)\s+HWaddr\s+([A-z0-9:]*).*" .
                    "inet addr:([0-9.]+).*Bcast:([0-9.]+).*Mask:([0-9.]+).*" .
                    "MTU:([0-9.]+).*Metric:([0-9.]+).*" .
                    "RX packets:([0-9.]+).*errors:([0-9.]+).*dropped:([0-9.]+).*overruns:([0-9.]+).*frame:([0-9.]+).*" .
                    "TX packets:([0-9.]+).*errors:([0-9.]+).*dropped:([0-9.]+).*overruns:([0-9.]+).*carrier:([0-9.]+).*" .
                    "RX bytes:([0-9.]+).*\((.*)\).*TX bytes:([0-9.]+).*\((.*)\)" .
                    "/ims", $int, $regex);

            if (!empty($regex)) {
                $interface = array();
                $interface['name'] = $regex[1];
                $interface['type'] = $regex[2];
                $interface['mac'] = $regex[3];
                $interface['ip'] = $regex[4];
                $interface['broadcast'] = $regex[5];
                $interface['netmask'] = $regex[6];
                $interface['mtu'] = $regex[7];
                $interface['metric'] = $regex[8];

                $interface['rx']['packets'] = (int) $regex[9];
                $interface['rx']['errors'] = (int) $regex[10];
                $interface['rx']['dropped'] = (int) $regex[11];
                $interface['rx']['overruns'] = (int) $regex[12];
                $interface['rx']['frame'] = (int) $regex[13];
                $interface['rx']['bytes'] = (int) $regex[19];
                $interface['rx']['hbytes'] = (int) $regex[20];

                $interface['tx']['packets'] = (int) $regex[14];
                $interface['tx']['errors'] = (int) $regex[15];
                $interface['tx']['dropped'] = (int) $regex[16];
                $interface['tx']['overruns'] = (int) $regex[17];
                $interface['tx']['carrier'] = (int) $regex[18];
                $interface['tx']['bytes'] = (int) $regex[21];
                $interface['tx']['hbytes'] = (int) $regex[22];

                $interfaces[] = $interface;
            }
            return $interfaces;
        }
    }

}

class WIFI
{
    public function __construct() {}

    public function WifiConfig() {
        $wifi = new Net_Wifi();
        return $wifi->get("wlan0");
    }
    public function IfConfig() {
        $wifi = new Net_Wifi();
        return $wifi->IfConfig();
    }

    public function DataDict() {
        $config = self::WifiConfig();

        return array(
            "activated"                     => $config->activated,
            "ap"                            => $config->ap,
            "associated"                    => $config->associated,
            "mode"                          => $config->mode,
            "nick"                          => $config->nick,
            "noise"                         => $config->noise,
            "packages_invalid_misc"         => $config->packages_invalid_misc,
            "packages_missed_beacon"        => $config->packages_missed_beacon,
            "packages_rx_invalid_crypt"     => $config->packages_rx_invalid_crypt,
            "packages_rx_invalid_frag"      => $config->packages_rx_invalid_frag,
            "packages_rx_invalid_nwid"      => $config->packages_rx_invalid_nwid,
            "packages_tx_excessive_retries" => $config->packages_tx_excessive_retries,
            "power"                         => $config->power,
            "protocol"                      => $config->protocol,
            "rate"                          => $config->rate,
            "rssi"                          => $config->rssi,
            "ssid"                          => $config->ssid,
            "frequency"                     => $config->frequency,
            "sensitivity"                   => $config->sensitivity,
            "link_quality"                  => $config->link_quality,
        );
    }

    public function WifiPass() {
        $pass = $_ENV['WIFI_PASS'];
        $ssid = $_ENV['WIFI_SSID'];

        if (empty($pass) || empty($ssid)) {
            $wifi = NVRAM::Show();
            $pass = $wifi['miio_passwd'];
            $ssid = $wifi['miio_ssid'];
        }

        return array(
            "wifi_ssid" => $ssid,
            "wifi_pass" => $pass,
        );
    }

    public function ResetWifiCredentials() {
        if  (NVRAM::Unset_Key('miio_ssid')     && \
             NVRAM::Unset_Key('miio_key_mgmt') && \
             NVRAM::Unset_Key('miio_passwd')) {
             return true;

        } else return false;
    }

    // * Get the SSID of the wifi connection
    public function Ssid() {
        $config = self::WifiConfig();
        return $config->ssid;
    }

    // * Get the bitrate of the wifi connection
    public function Bitrate() {
        $config = self::WifiConfig();
        return $config->rate;
    }

    // * Get the noise level of the wifi connection
    public function NoiseLevel() {
        $config = self::WifiConfig();
        return $config->noise;
    }

    // * Get the link quality value of the wifi connection
    public function LinkQuality() {
        $config = self::WifiConfig();
        return $config->link_quality;
    }

    // * Get the signal level of the wifi connection
    public function SignalLevel() {
        $config = self::WifiConfig();
        return $config->rssi;
    }
}

// **************************************************************
// ** OS System functions                                      **
// **************************************************************

class OS
{
   public static function Reboot() {
        exec("/sbin/reboot -d 5 2>&1", $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing reboot: %s', implode(" ", $output))
            );
        }

        return implode("\n", $output);
    }

    public static function Shutdown() {
        exec("/sbin/poweroff -d 5 2>&1", $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing poweroff: %s', implode(" ", $output))
            );
        }

        return implode("\n", $output);
    }

    public static function Uptime() {
        // https://www.aboutcher.co.uk/2013/08/updated-php-uptime-function/
        $uptime = @file_get_contents( "/proc/uptime");

        if ($uptime == false) {
            throw new \Exception(
                'Error retrieving uptime from /proc/uptime'
            );
        }

        $uptime = explode(" ",$uptime);
        $uptime = $uptime[0];
        $days = explode(".",(($uptime % 31556926) / 86400));
        $hours = explode(".",((($uptime % 31556926) % 86400) / 3600));
        $minutes = explode(".",(((($uptime % 31556926) % 86400) % 3600) / 60));
        $time = ".";

        if ($minutes > 0)
            $time=$minutes[0]." mins".$time;

        if ($minutes > 0 && ($hours > 0 || $days > 0))
            $time = ", ".$time;

        if ($hours > 0)
            $time = $hours[0]." hours".$time;

        if ($hours > 0 && $days > 0)
            $time = ", ".$time;

        if ($days > 0)
            $time = $days[0]." days".$time;

        return $time;
    }

    public static function Load() {
        $content = @file_get_contents("/proc/loadavg");

        if (empty($content) || ($content == false)) {
            throw new \Exception(
                'Error retrieving load averages from /proc/loadavg'
            );
        }

        list($now, $five, $ten, $procs, $_) = explode(" ", $content);
        list($now_procs, $all_procs) = explode("/", $procs);

        return array(
            "00"            => $now,
            "05"            => $five,
            "10"            => $ten,
            "procs_running" => $now_procs,
            "procs_total"   => $all_procs,
        );
    }

    public static function Dmesg() {
        exec("/bin/dmesg", $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing dmesg: %s', implode(" ", $output))
            );
        }

        return implode("\n", $output);
    }

    public static function Proclist() {
        exec("/bin/ps -ef 2>&1", $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing ps -ef: %s', implode(" ", $output))
            );
        }

        return implode("\n", $output);
    }

    public static function DiskUsage() {
        exec("/bin/df -h 2>&1", $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing df -h: %s', implode(" ", $output))
            );
        }

        return implode("\n", $output);
    }
}

?>
