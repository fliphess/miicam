<?php

// **************************************************************
// ** Blue led control                                         **
// **************************************************************

class Blue_Led
{
    public static function IsOn() {
        $command = "/tmp/sd/firmware/bin/blue_led -s | awk '{print \$NF}'";
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
        $command = "/tmp/sd/firmware/bin/blue_led -e";
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
        return true;
    }

    public static function TurnOff() {
        $command = "/tmp/sd/firmware/bin/blue_led -d";
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
        return true;
    }

    public static function TurnBlink() {
        $command = "/tmp/sd/firmware/bin/blue_led -b";
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
// ** Yellow led control                                       **
// **************************************************************

class Yellow_Led
{
    public static function IsOn() {
        $command = "/tmp/sd/firmware/bin/yellow_led -s | awk '{print \$NF}'";
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
        $command = "/tmp/sd/firmware/bin/yellow_led -e";
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
        return true;
    }

    public static function TurnOff() {
        $command = "/tmp/sd/firmware/bin/yellow_led -d";
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
        return true;
    }

    public static function TurnBlink() {
        $command = "/tmp/sd/firmware/bin/yellow_led -b";
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
        $command = "/tmp/sd/firmware/bin/ir_led -e";
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
        return true;
    }

    public static function TurnOff() {
        $command = "/tmp/sd/firmware/bin/ir_led -d";
        exec($command, $output, $return);

        if ($return != 0) {
            throw new \Exception(
                sprintf('Error executing %s: %s', $command, implode(" ", $output))
            );
        }
        return true;
    }
}


function LedState() {
    return array(
        "blue_led"   => Blue_Led::IsOn(),
        "yellow_led" => Yellow_Led::IsOn(),
        "ir_led"     => IR_Led::IsOn(),
    );
}
