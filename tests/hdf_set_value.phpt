--TEST--
Test hdf_set_value() behavior
--SKIPIF--
<?php if (!extension_loaded("clearsilver")) print "skip clearsilver extension not loaded"; ?>
--FILE--
<?php
$hdf = hdf_init();

var_dump(hdf_set_value($hdf, 'data.null', null));
var_dump(hdf_set_value($hdf, 'data.bool.true', true));
var_dump(hdf_set_value($hdf, 'data.bool.false', false));
var_dump(hdf_set_value($hdf, 'data.integer.positive', +100));
var_dump(hdf_set_value($hdf, 'data.integer.negative', -72));
var_dump(hdf_set_value($hdf, 'data.float.positive', 0.5321));
var_dump(hdf_set_value($hdf, 'data.float.negative', -79.94134));
var_dump(hdf_set_value($hdf, 'data.array', array(1, 2, 3)));
var_dump(hdf_set_value($hdf, 'data.string', 'Getting the picture?'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
