var createCustomTestGetterSetter = $vm.createCustomTestGetterSetter;

function assert(b, m) {
    if (!b)
        throw new Error("Bad:" + m);
}

class Class { };

let items = [
    new Class,
    new Class,
    new Class,
    new Class,
];

let customGetterSetter = createCustomTestGetterSetter();
items.forEach((x) => {
    x.__proto__ = customGetterSetter;
    assert(x.__proto__ === customGetterSetter);
});

function validate(x, valueResult, accessorResult) {
    assert(x.customValue === valueResult);

    assert(x.customAccessor === accessorResult);

    let o = {};
    x.customValue = o;
    assert(o.result === undefined);
    assert(x.customValue === o);

    o = {};
    x.customAccessor = o;
    assert(o.result === accessorResult);

    assert(x.randomProp === 42 || x.randomProp === undefined);
    x.customValue = valueResult;
}
noInline(validate);


let start = Date.now();
for (let i = 0; i < testLoopCount; ++i) {
    for (let i = 0; i < items.length; ++i) {
        validate(items[i], customGetterSetter, items[i]);
    }
}

customGetterSetter.randomProp = 42;

for (let i = 0; i < testLoopCount; ++i) {
    for (let i = 0; i < items.length; ++i) {
        validate(items[i], customGetterSetter, items[i]);
    }
}

function validate2(x, valueResult, accessorResult) {
    assert(x.customValue === valueResult);

    assert(x.customAccessor === accessorResult);

    let o = {};
    x.customValue = o;
    assert(o.result === valueResult);

    o = {};
    x.customAccessor = o;
    assert(o.result === accessorResult);

    assert(x.randomProp === 42 || x.randomProp === undefined);
}
noInline(validate2);

items.forEach((x) => {
    delete x.customValue;
    Reflect.setPrototypeOf(x, {
        get customValue() { return 42; },
        get customAccessor() { return 22; },
        set customValue(x) { x.result = 42; },
        set customAccessor(x) { x.result = 22; },
    });
});

for (let i = 0; i < testLoopCount; ++i) {
    for (let i = 0; i < items.length; ++i) {
        validate2(items[i], 42, 22);
    }
}
