<?php

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
            $nvram = NVRAM::Show();
            $pass = $nvram['miio_passwd'];
            $ssid = $nvram['miio_ssid'];
        }

        return array(
            "wifi_ssid" => $ssid,
            "wifi_pass" => $pass,
        );
    }

    // * Reset wifi credentials
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

?>
