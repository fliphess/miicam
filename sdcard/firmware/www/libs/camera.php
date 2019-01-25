<?php


// **************************************************************
// ** ISP 328 Functions                                        **
// **************************************************************

class ISP328
{
    public function __construct() { }

    public static $all_isp_keys = [  // * All available options for the isp328 kernel module
        "brightness",
        "contrast",
        "saturation",
        "sharpness",
        "denoise",
        "hue",

        "daynight",     // * Nightmode
        "flip",         // * Flipmode
        "mirror",       // * Mirrormode

        "ae_en",        // * Auto exposure
        "af_en",        // * Auto Focus
        "awb_en",       // * Auto White Balance
        "sp_en",        // * Auto Sharpen

        "size",         // * Image size
        "white_clip",   // * White Clip Protection

        "adjust_blc",   // * Black level Correction
        "adjust_ce",    // * Contrast Enhancement
        "adjust_gamma", // * Adjust Gamma Curve Correction
        "adjust_nr",    // * Adjust Noise Reduction

        "sen_exp",      // * Sensor Exposure
        "sen_fps",      // * Sensor FPS
        "sen_gain",     // * Sensor Gain
        "sen_size",     // * Sensor Size (HxW)
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

        $command = sprintf("echo r %s > /proc/isp328/command 2>&1", escapeshellarg($key));

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

        return self::Get($key);
    }
}


// **************************************************************
// ** IR Cut                                                   **
// **************************************************************

class IR_Cut extends ISP328
{

    public static function GetState() {
        $command = sprintf("/tmp/sd/firmware/bin/ir_cut -s | awk '{print \$NF}' 2>&1");

        exec($command, $output, $return);
        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        return (implode("", $output) == "on") ? true : false;
    }

    public static function IsOn() {
        $state = self::GetSTate();
        return ($state) ? true : false;
    }

    public static function IsOff() {
        $state = self::GetSTate();
        return (!$state) ? true : false;
    }

    public static function TurnOn() {
        $command = sprintf("/tmp/sd/firmware/bin/ir_cut -e 2>&1");

        exec($command, $output, $return);
        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        return true;
    }

    public static function TurnOff() {
        $command = sprintf("/tmp/sd/firmware/bin/ir_cut -d 2>&1");

        exec($command, $output, $return);
        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        return true;
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

function state_to_bool($state) {
    if ($state == "enable") {
        return true;
    }
    else if ($state == "disable") {
        return false;
    } else {
        return "unknown";
    }
}


function CameraState() {
    $default = 'unknown';

    return array(
        "ir_cut"                 => IR_Cut::IsOn(),
        "night_mode"             => NightMode::IsOn(),
        "flip_mode"              => FlipMode::IsOn(),
        "mirror_mode"            => MirrorMode::IsOn(),

        "brightness"             => (int)ISP328::Get('brightness'),
        "contrast"               => (int)ISP328::Get('contrast'),
        "hue"                    => (int)ISP328::Get('hue'),
        "saturation"             => (int)ISP328::Get('saturation'),
        "denoise"                => (int)ISP328::Get('denoise'),
        "sharpness"              => (int)ISP328::Get('sharpness'),

        "auto_exposure"          => state_to_bool(ISP328::Get("ae_en")),
        "auto_focus"             => state_to_bool(ISP328::Get("af_en")),
        "auto_white_balance"     => state_to_bool(ISP328::Get("awb_en")),
        "auto_sharpen"           => state_to_bool(ISP328::Get("sp_en")),
        "black_level_correction" => state_to_bool(ISP328::Get("adjust_blc")),
        "contrast_enhancement"   => state_to_bool(ISP328::Get("adjust_ce")),
        "gamma_curve_correction" => state_to_bool(ISP328::Get("adjust_gamma")),
        "noise_reduction"        => state_to_bool(ISP328::Get("adjust_nr")),
    ) + LED::LedState();
}



// **************************************************************
// ** Reset Camera State                                       **
// **************************************************************

function CameraReset() {
    ISP328::Set('brightness',   128);
    ISP328::Set('contrast',     128);
    ISP328::Set('hue',          128);
    ISP328::Set('saturation',   128);
    ISP328::Set('denoise',      128);
    ISP328::Set('sharpness',    128);

    ISP328::Set("ae_en",        1);
    ISP328::Set("af_en",        1);
    ISP328::Set("awb_en",       1);
    ISP328::Set("sp_en",        1);

    ISP328::Set("adjust_blc",   1);
    ISP328::Set("adjust_ce",    1);
    ISP328::Set("adjust_gamma", 1);
    ISP328::Set("adjust_nr",    1);

    return true;
}



function SaveState() {
    $command = sprintf("/tmp/sd/firmware/etc/init/S99restore_state save 2>&1");
    exec($command, $output, $return);

    if ($return != 0) {
        throw new \Exception(
            sprintf('Error executing %s: %s', $command, implode(" ", $output))
        );
    }
    return implode(" ", $output);
}
