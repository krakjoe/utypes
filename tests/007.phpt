--TEST--
utypes return var
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
utypes(function(string $type, $value){
	var_dump($type, $value);

	return true;
});

function test() : some {
	return new stdClass;
}

test();
?>
--EXPECT--
string(4) "some"
object(stdClass)#2 (0) {
}

