--TEST--
utypes return const
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
utypes\handler(function(string $type, $value){
	var_dump($type, $value);

	return true;
});

function test() : some {
	return 10;
}

test();
?>
--EXPECT--
string(4) "some"
int(10)
