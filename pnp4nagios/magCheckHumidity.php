<?php

// $this->DS
// $this->MACRO

define("WCOLOR", "#FFFF00");
define("CCOLOR", "#FF0000");
define("AREA", "#256AEF");
define("LINE", "#000000");
define("GSTART", "#F0F0F0");
define("GEND", "#0000A0");

$opt[1] = '--vertical-label "Percentage" --title "Humidity ' . $this->MACRO["DISP_HOSTNAME"] . ' / ' . $this->MACRO["DISP_SERVICEDESC"] . '"';

$data = $this->DS[0];

$def[1] = rrd::def("var1", $data["RRDFILE"], $data["DS"], "AVERAGE");
$def[1] .= rrd::gradient("var1", constant("GSTART"), constant("GEND"), rrd::cut($data["NAME"], 16), 20);
$def[1] .= rrd::line1("var1", constant("LINE"));
$def[1] .= rrd::gprint("var1", array("LAST", "MAX", "AVERAGE"), "%3.2lf perc.");

if (!empty($data["WARN"]))
{
  $def[1] .= rrd::hrule($data["WARN"], constant("WCOLOR"), "Warning " . $data["WARN"] . "\\n");
}

if (!empty($data["CRIT"]))
{
  $def[1] .= rrd::hrule($data["CRIT"], constant("CCOLOR"), "Critical " . $data["CRIT"] . "\\n");
}

$def[1] .= rrd::comment("Template " . $data["TEMPLATE"] . "\\r");


