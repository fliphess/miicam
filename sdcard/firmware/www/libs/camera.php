<?php

// **************************************************************
// ** led functions                                            **
// **************************************************************

class LED
{
    private static $leds = [
        "blue_led",
        "yellow_led",
        "ir_led",
    ];

    private static $states = [
       "on",
       "off",
       "blink",
    ];

    private static $default_state = 'unknown';

    public static $chuangmi_ctrl = '/tmp/sd/firmware/bin/chuangmi_ctrl';

    public function __construct() { }

    // * Get the state of a led
    public function GetState(string $led) {
        if (!in_array($led, self::$leds)) return;
        return NVRAM::Get($led);
    }

    // * Set the state of a led
    public function SetState(string $led, string $state) {
        if (!in_array($led, self::$leds) || !in_array($state, self::$states)) return;
        return NVRAM::Set($led, $state);
    }

    public static function Ctl(string $arguments) {
        $command = sprintf("%s %s", self::$chuangmi_ctrl, $arguments);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
    }

    // * Returns True if led is on
    public function IsLedOn(string $led) {
        $state = self::GetState($led);
        return (($state == "on") || ($state == "blink")) ? true : false;
    }

    // * Get Led Json status
    public function LedState() {
        $data  = array();
        foreach (self::$leds as $led) {
            $data[$led] = self::GetState($led) ?? self::$default_state;
        }
        return $data;
    }
}


// **************************************************************
// ** Blue led control                                         **
// **************************************************************

class Blue_Led extends LED
{
    public function IsOn() {
        return parent::IsLedOn('blue_led');
    }

    public function IsOff() {
        return (parent::IsLedOn('blue_led') == true) ? false : true;
    }

    public function TurnOn() {
        if (!self::IsOn()) {
            self::Ctl('LEDSTATUS 0 0');
            self::SetState('blue_led', 'on');
            return self::IsOn();
        }
        else return false;
    }

    public function TurnOff() {
        if (self::IsOn()) {
            self::Ctl('LEDSTATUS 0 1');
            self::SetState('blue_led', 'off');
            return self::IsOff();
        }
        else return false;
    }

    public function TurnBlink() {
        self::Ctl('LEDSTATUS 0 2');
        self::SetState('blue_led', 'blink');
        return self::IsOn();
    }
}


// **************************************************************
// ** Yellow led control                                       **
// **************************************************************

class Yellow_Led extends LED
{
    public function IsOn() {
        return parent::IsLedOn('yellow_led');
    }

    public function IsOff() {
        return (parent::IsLedOn('yellow_led') == true) ? false : true;
    }

    public function TurnOn() {
        if (!self::IsOn()) {
            self::Ctl('LEDSTATUS 1 0');
            self::SetState('yellow_led', 'on');
            return self::IsOn();
        }
        else return false;
    }

    public function TurnOff() {
        if (self::IsOn()) {
            self::Ctl('LEDSTATUS 1 1');
            self::SetState('yellow_led', 'off');
            return self::IsOff();
        }
        else return false;
    }

    public function TurnBlink() {
        self::Ctl('LEDSTATUS 1 2');
        self::SetState('yellow_led', 'blink');
        return self::IsOn();
    }
}


// **************************************************************
// ** Infra Red led control                                    **
// **************************************************************

class IR_Led extends LED
{
    public function IsOn() {
        return parent::IsLedOn('ir_led');
    }

    public function IsOff() {
        return (parent::IsLedOn('ir_led') == true) ? false : true;
    }

    public function TurnOn() {
        if (!self::IsOn()) {
            self::Ctl('IRLED 255');
            self::SetState('ir_led', 'on');
            return self::IsOn();
        }
    }

    public function TurnOff() {
        if (self::IsOn()) {
            self::Ctl('IRLED 0');
            self::SetState('ir_led', 'off');
            return self::IsOff();
        }
    }
}


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

    private function ReadValue() {
        $output = @file_get_contents("/proc/isp328/command");
        if ($output == false) {
            throw new \Exception(
                'Error retrieving isp328 output from /proc/isp328/command'
            );
        }
        return trim($output);
    }

    public function Get(string $key) {
        if (!in_array($key, self::$all_isp_keys)) {
            throw new \Exception(
                sprintf('Invalid input: Key %s not in known keys.', $key)
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

    public function Set(string $key, string $value) {
        if (!in_array($key, self::$all_isp_keys)) {
            throw new \Exception(
                'Invalid input: Key %s not in known keys.'
            );
        }

        $current_value = self::Get($key);

        if ($current_value == $value) return $value;

        $command = sprintf("echo w %s %s > /proc/isp328/command", escapeshellarg($key), escapeshellarg($value));

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
    public function IsOn() {
        $state = NVRAM::Get("ir_cut");
        return (($state == "on") || ($state == "auto")) ? true : false;
    }

    public function IsOff() {
        $state = NVRAM::Get("ir_cut");
        return ($state == "off") ? true : false;
    }

    public function TurnOn() {
        if (!self::IsOn()) {
            GPIO::Set(14, 1);
            GPIO::Set(15, 0);
            NVRAM::Set("ir_cut", "on");
            return true;
        }
        else return false;
    }

    public function TurnOff() {
        if (self::IsOn()) {
            GPIO::Set(14, 0);
            GPIO::Set(15, 1);
            NVRAM::Set("ir_cut", "off");
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
    public function IsOn() {
        $current = self::Get('daynight');
        return (($_ENV['AUTO_NIGHT_MODE'] == 1) || ($current == 'NIGHT_MODE')) ? true : false;
    }

    public function IsOff() {
        $current = self::Get('daynight');
        return (($_ENV['AUTO_NIGHT_MODE'] == 0) && ($current == 'DAY_MODE')) ? true : false;
    }

    public function TurnOn() {
        if ($_ENV['AUTO_NIGHT_MODE'] == 1) return false;

        if (!self::IsOn()) {
            IR_Led::TurnOn();
            IR_Cut::TurnOff();
            self::Set('daynight', 1);
            return true;
        }
        else return false;
    }

    public function TurnOff() {
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
    public function IsOn() {
        $current = self::Get('flip');
        return ($current == 1) ? true : false;
    }

    public function IsOff() {
        $current = self::Get('flip');
        return ($current == 0) ? true : false;
    }

    public function TurnOn() {
        if (!self::IsOn()) {
            self::Set('flip', 1);
            return true;
        } else {
            return false;
        }
    }

    public function TurnOff() {
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
    public function IsOn() {
        $current = self::Get('mirror');
        return ($current == 1) ? true : false;
    }

    public function IsOff() {
        $current = self::Get('mirror');
        return ($current == 0) ? true : false;
    }

    public function TurnOn() {
        if (!self::IsOn()) {
            self::Set('mirror', 1);
            return true;
        } else {
            return false;
        }
    }

    public function TurnOff() {
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
        "ir_cut"      => (IR_Cut::IsOn())     ? "on" : "off",
        "night_mode"  => (NightMode::IsOn())  ? "on" : "off",
        "flip_mode"   => (FlipMode::IsOn())   ? "on" : "off",
        "mirror_mode" => (MirrorMode::IsOn()) ? "on" : "off",
    );
}
