--TEST--
utypes verify return match
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
interface IFunction
{
	public function IFunction() : stdClass;
}

utypes\handler(function(string $type, $value){
	switch ($type) {
		case "IFunction":
			return utypes\verify($value, IFunction::class);
	}

	return false;
});

function test(IFunction $num) {
	return $num;
}

var_dump(test(function () : sTdClass {

}));
?>
--EXPECT--
object(Closure)#2 (0) {
}


