--TEST--
Test hdf_set_node() behavior
--SKIPIF--
<?php if (!extension_loaded("clearsilver")) print "skip clearsilver extension not loaded"; ?>
--FILE--
<?php
$hdf = hdf_init();

$hdf2 = hdf_init();

$array = array('null' => null,
               'bool' => array('true' => true,
                               'false'=> false),
               'integer' => array('positive' => 100,
                                  'negative' => -72),
               'float.positive' => 0.5321, // you can still use dotted notation 
               'float.negative' => -79.94134,
               'string' => 'Getting the picture?');

var_dump(hdf_set_node($hdf2, 'array-copy', $array));

var_dump(hdf_set_node($hdf, 'hdf-copy', $hdf2));
?>
--EXPECT--
bool(true)
bool(true)
