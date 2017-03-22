--TEST--
utypes recv init
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
utypes(function(string $type, $value){
	var_dump($type, $value);

	return true;
});

function test(some $vars = null) {
	
}

test(10, 40);
?>
--EXPECT--
string(4) "some"
int(10)


