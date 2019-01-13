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

    public static function GetState(string $led) {
        // * Get the state of a led
        if (!in_array($led, self::$leds)) return;
        return NVRAM::Get($led);
    }

    public static function SetState(string $led, string $state) {
        // * Set the state of a led
        if (!in_array($led, self::$leds) || !in_array($state, self::$states)) return;
        return NVRAM::Set($led, $state);
    }

    public static function Ctl(string $arguments) {
        $command = sprintf("%s %s 2>&1", self::$chuangmi_ctrl, $arguments);
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
    }

    public static function IsLedOn(string $led) {
        // * Returns True if led is on
        $state = self::GetState($led);
        return (($state == "on") || ($state == "blink")) ? true : false;
    }

    // * Get Led Json status
    public static function LedState() {
        $data  = array();
        foreach (self::$leds as $led) {
            $data[$led] = self::IsLedOn($led);
        }
        return $data;
    }
}


// **************************************************************
// ** Blue led control                                         **
// **************************************************************

class Blue_Led extends LED
{
    public static function IsOn() {
        return parent::IsLedOn('blue_led');
    }

    public static function IsOff() {
        return (parent::IsLedOn('blue_led') == true) ? false : true;
    }

    public static function TurnOn() {
        if (!self::IsOn()) {
            self::Ctl('LEDSTATUS 0 0');
            self::SetState('blue_led', 'on');
            return self::IsOn();
        }
        else return false;
    }

    public static function TurnOff() {
        if (self::IsOn()) {
            self::Ctl('LEDSTATUS 0 1');
            self::SetState('blue_led', 'off');
            return self::IsOff();
        }
        else return false;
    }

    public static function TurnBlink() {
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
    public static function IsOn() {
        return parent::IsLedOn('yellow_led');
    }

    public static function IsOff() {
        return (parent::IsLedOn('yellow_led') == true) ? false : true;
    }

    public static function TurnOn() {
        if (!self::IsOn()) {
            self::Ctl('LEDSTATUS 1 0');
            self::SetState('yellow_led', 'on');
            return self::IsOn();
        }
        else return false;
    }

    public static function TurnOff() {
        if (self::IsOn()) {
            self::Ctl('LEDSTATUS 1 1');
            self::SetState('yellow_led', 'off');
            return self::IsOff();
        }
        else return false;
    }

    public static function TurnBlink() {
        self::Ctl('LEDSTATUS 1 2');
        self::SetState('yellow_led', 'blink');
        return self::IsOn();
    }
}


// **************************************************************
// ** Infra Red led control                                    **
// **************************************************************

class IR_Led
{
    public static function IsOn() {
        $command = "/tmp/sd/firmware/bin/ir_led -s | awk '{print \$NF}'";
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }

        if (($output) && (rtrim($output[0]) == "on")) {
            return true;
        } else { 
            return false;
        }
    }

    public static function IsOff() {
        return (self::IsOn() == true) ? false : true;
    }

    public static function TurnOn() {
        if (!self::IsOn()) {
            $command = "/tmp/sd/firmware/bin/ir_led -e";
            exec($command, $output, $return);
    
            if ($return != 0) {
                throw new \Exception(
                    sprintf('Error executing %s: %s', $command, implode(" ", $output))
                );
            }
            return self::IsOn();
        }
    }

    public static function TurnOff() {
        if (self::IsOn()) {
            $command = "/tmp/sd/firmware/bin/ir_led -d";
            exec($command, $output, $return);
    
            if ($return != 0) {
                throw new \Exception(
                    sprintf('Error executing %s: %s', $command, implode(" ", $output))
                );
            }
            return self::IsOff();
        }
    }
}
