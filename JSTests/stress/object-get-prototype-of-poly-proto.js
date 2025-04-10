function shouldBe(actual, expected) {
    if (actual !== expected)
        throw new Error('bad value: ' + actual);
}

function makePolyProtoObject() {
    function foo() {
        class C {
            constructor() {
                this._field = 42;
                this.hello = 33;
            }
        };
        return new C;
    }
    for (let i = 0; i < 15; ++i)
        foo();
    return foo();
}

function target(object)
{
    return [object.hello, Object.getPrototypeOf(object)];
}
noInline(target);

var polyProtoObject = makePolyProtoObject();
var prototype = Reflect.getPrototypeOf(polyProtoObject);
for (var i = 0; i < testLoopCount; ++i)
    shouldBe(target(polyProtoObject)[1], prototype);
