--TEST--
utypes recv skip
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
utypes\handler(function(string $type, $value){
	var_dump($type, $value);

	return true;
});

function test($first, second $second) {
	
}

test(10, 40);
?>
--EXPECT--
string(6) "second"
int(40)

