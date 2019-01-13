<?php

// **************************************************************
// ** ISP 328 Functions                                        **
// **************************************************************

class ISP328
{
    public function __construct() { }

    public static $all_isp_keys = [  // * All available options for the isp328 kernel module
        "adjust_all",
        "adjust_blc",
        "adjust_ce",
        "adjust_dpc",
        "adjust_gamma",
        "adjust_nr",
        "ae_en",
        "af_en",
        "awb_en",
        "brightness",
        "contrast",
        "daynight",
        "denoise",
        "dr_mode",
        "drc_strength",
        "flip",
        "gen_bin",
        "hue",
        "mirror",
        "mrnr_en",
        "mrnr_y_str",
        "reload_cfg",
        "saturation",
        "sen_exp",
        "sen_fps",
        "sen_gain",
        "sen_reg",
        "sen_size",
        "sharpness",
        "size",
        "sp_en",
        "tmnr_en",
        "white_clip",
    ];

    private static function ReadValue() {
        $output = @file_get_contents("/proc/isp328/command");
        if ($output == false) {
            throw new \Exception(
                'Error retrieving isp328 output from /proc/isp328/command'
            );
        }
        return trim($output);
    }

    public static function Get(string $key) {
        if (!in_array($key, self::$all_isp_keys)) {
            throw new \Exception(
                sprintf('Invalid input: Key %s not in known keys.', escapeshellarg($key))
            );
        }

        $command = sprintf("echo r %s > /proc/isp328/command", escapeshellarg($key));

        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        return self::ReadValue();
    }

    public static function Set(string $key, string $value) {
        if (!in_array($key, self::$all_isp_keys)) {
            throw new \Exception(
                'Invalid input: Key %s not in known keys.'
            );
        }

        $current_value = self::Get($key);

        if ($current_value == $value) return $value;

        $command = sprintf("echo w %s %s > /proc/isp328/command 2>&1", escapeshellarg($key), escapeshellarg($value));

        exec($command, $output, $return);
        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        return self::ReadValue();
    }
}


// **************************************************************
// ** IR Cut                                                   **
// **************************************************************

class IR_Cut extends ISP328
{
    public static function IsOn() {
        $state = GPIO::Get(14);
        return ($state == "1") ? true : false;
    }

    public static function IsOff() {
        $state = GPIO::Get(14);
        return ($state == "0") ? true : false;
    }

    public static function TurnOn() {
        if (!self::IsOn()) {
            GPIO::Set(14, 1);
            GPIO::Set(15, 0);
            return true;
        }
        else return false;
    }

    public static function TurnOff() {
        if (self::IsOn()) {
            GPIO::Set(14, 0);
            GPIO::Set(15, 1);
            return true;
        }
        else return false;
    }
}


// **************************************************************
// ** Night Mode                                               **
// **************************************************************

class NightMode extends ISP328
{
    public static function IsOn() {
        $current = self::Get('daynight');
        return (($_ENV['AUTO_NIGHT_MODE'] == 1) || ($current == 'NIGHT_MODE')) ? true : false;
    }

    public static function IsOff() {
        $current = self::Get('daynight');
        return (($_ENV['AUTO_NIGHT_MODE'] == 0) && ($current == 'DAY_MODE')) ? true : false;
    }

    public static function TurnOn() {
        if ($_ENV['AUTO_NIGHT_MODE'] == 1) return false;

        if (!self::IsOn()) {
            IR_Led::TurnOn();
            IR_Cut::TurnOff();
            self::Set('daynight', 1);
            return true;
        }
        else return false;
    }

    public static function TurnOff() {
        if ($_ENV['AUTO_NIGHT_MODE'] == 1) return false;

        if (self::IsOn()) {
            IR_Led::TurnOff();
            IR_Cut::TurnOn();
            self::Set('daynight', 0);
            return true;
        }
        else return false;
    }
}


// **************************************************************
// ** Flip Mode                                                **
// **************************************************************

class FlipMode extends ISP328
{
    public static function IsOn() {
        $current = self::Get('flip');
        return ($current == 1) ? true : false;
    }

    public static function IsOff() {
        $current = self::Get('flip');
        return ($current == 0) ? true : false;
    }

    public static function TurnOn() {
        if (!self::IsOn()) {
            self::Set('flip', 1);
            return true;
        } else {
            return false;
        }
    }

    public static function TurnOff() {
        if (self::IsOn()) {
            self::Set('flip', 0);
            return true;
        } else {
            return false;
        }
    }
}


// **************************************************************
// ** Mirror Mode                                              **
// **************************************************************

class MirrorMode extends ISP328
{
    public static function IsOn() {
        $current = self::Get('mirror');
        return ($current == 1) ? true : false;
    }

    public static function IsOff() {
        $current = self::Get('mirror');
        return ($current == 0) ? true : false;
    }

    public static function TurnOn() {
        if (!self::IsOn()) {
            self::Set('mirror', 1);
            return true;
        } else {
            return false;
        }
    }

    public static function TurnOff() {
        if (self::IsOn()) {
            self::Set('mirror', 0);
            return true;
        } else {
            return false;
        }

    }
}


// **************************************************************
// ** Get Camera State                                         **
// **************************************************************

function CameraState() {
    $default = 'unknown';

    return array(
        "ir_cut"      => IR_Cut::IsOn(),
        "night_mode"  => NightMode::IsOn(),
        "flip_mode"   => FlipMode::IsOn(),
        "mirror_mode" => MirrorMode::IsOn(),
    );
}
