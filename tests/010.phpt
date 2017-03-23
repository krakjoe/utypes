--TEST--
utypes verify return ref mismatch
--SKIPIF--
<?php if (!extension_loaded("utypes")) print "skip"; ?>
--FILE--
<?php 
interface IFunction
{
	public function & IFunction() : int;
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

var_dump(test(function () : int {

}));
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 1 passed to test() must implement interface IFunction, instance of Closure given, called in %s on line 22 and defined in %s:16
Stack trace:
#0 %s(22): test(Object(Closure))
#1 {main}
  thrown in %s on line 16

