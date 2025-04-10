function shouldBe(actual, expected)
{
    if (actual !== expected)
        throw new Error('bad value: ' + actual);
}

function test(object)
{
    return Object.getOwnPropertyNames(object);
}
noInline(test);

var object = {};
for (var i = 0; i < testLoopCount; ++i) {
    var result = test(object);
    shouldBe(result.length, 0);
    shouldBe(result[0], undefined);
    result[0] = i;
    shouldBe(result.length, 1);
    shouldBe(result[0], i);
}

object[0] = 42;
for (var i = 0; i < testLoopCount; ++i) {
    var result = test(object);
    shouldBe(result.length, 1);
    shouldBe(result[0], '0');
}
